#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/IntrinsicInst.h"

#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/PostDominators.h"

#include "llvm/IR/Dominators.h"

#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"

#include "llvm/Support/raw_ostream.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/DenseMap.h"

#include <algorithm>
#include <set>
#include <vector>

using namespace llvm;

namespace {

// Informazioni sul guard di un loop.
// IsGuarded: true se esiste un branch condizionale esterno che entra/salta il loop.
// HasRealStatement: true se il guard contiene anche codice reale, es. puts/store/call.
// Se HasRealStatement è true, il loop può essere guarded, ma non adiacente al precedente.
struct GuardInfo {
    bool IsGuarded = false;
    bool HasRealStatement = false;

    BasicBlock *GuardBlock = nullptr;        // blocco del branch condizionale
    BasicBlock *LoopEntry = nullptr;         // ramo che entra nel loop
    BasicBlock *NonLoopSuccessor = nullptr;  // ramo che salta il loop
};

// Stampa un BasicBlock anche quando non ha nome testuale.
// Utile perché getName() spesso stampa vuoto per blocchi tipo %3, %10, ecc.
void printBB(StringRef Label, BasicBlock *BB) {
    errs() << Label;

    if (!BB) {
        errs() << "null\n";
        return;
    }

    BB->printAsOperand(errs(), false);
    errs() << "\n";
}

/*
 * Ritorna true se BB è un blocco "vuoto" con solo:
 *
 *     br label %succ
 *
 * Quindi non deve contenere call, store, add, icmp, phi, ecc.
 */
// True se BB contiene solo un branch incondizionato.
// Questi blocchi sono considerati "vuoti" e non rompono l'adiacenza.
bool isEmptyUnconditionalBlock(BasicBlock *BB) {
    if (!BB)
        return false;

    BranchInst *BI = dyn_cast<BranchInst>(BB->getTerminator());
    if (!BI || !BI->isUnconditional())
        return false;

    for (Instruction &I : *BB) {
        if (&I == BB->getTerminator())
            continue;

        if (isa<DbgInfoIntrinsic>(&I))
            continue;

        return false;
    }

    return true;
}

/*
 * Ritorna true se il basic block contiene istruzioni reali,
 * cioè istruzioni che rappresentano statement del programma.
 *
 * Esempi:
 *   - call @puts        -> reale
 *   - call @printf      -> reale
 *   - store             -> reale
 *   - load              -> reale
 *
 * Invece una icmp usata solo per il branch di guardia non viene considerata
 * "statement reale".
 */
bool hasRealStatement(BasicBlock *BB) {
    if (!BB)
        return true;

    for (Instruction &I : *BB) {
        if (&I == BB->getTerminator())
            continue;

        if (isa<DbgInfoIntrinsic>(&I))
            continue;

        if (I.mayHaveSideEffects() || I.mayReadOrWriteMemory())
            return true;
    }

    return false;
}

/*
 * Controlla se da From si può arrivare a To attraversando solo
 * blocchi vuoti con branch incondizionato.
 *
 * Serve per accettare casi come:
 *
 *     exit L0:
 *       br label %guardL1
 *
 * oppure:
 *
 *     exit L0:
 *       br label %middle
 *
 *     middle:
 *       br label %guardL1
 *
 * purché i blocchi intermedi siano davvero vuoti.
 */
// Serve per accettare piccoli blocchi intermedi generati da LLVM.
bool emptyPathToTarget(BasicBlock *From, BasicBlock *To) {
    if (!From || !To)
        return false;

    BasicBlock *Cur = From;

    for (unsigned Steps = 0; Steps < 8; ++Steps) {
        if (!isEmptyUnconditionalBlock(Cur))
            return false;

        BranchInst *BI = cast<BranchInst>(Cur->getTerminator());
        BasicBlock *Succ = BI->getSuccessor(0);

        if (Succ == To)
            return true;

        Cur = Succ;
    }

    return false;
}

/*
 * Per un loop, la continuation è il punto in cui il controllo arriva
 * quando il loop termina.
 *
 * Se l'exit block è un blocco vuoto:
 *
 *     exit:
 *       br label %cont
 *
 * allora la continuation è %cont.
 *
 * Altrimenti la continuation coincide con l'exit block.
 */
// Ritorna il punto raggiunto dopo la fine del loop.
// Se l'exit è un blocco vuoto, ritorna il suo successore.
BasicBlock *getLoopContinuation(Loop *L) {
    BasicBlock *Exit = L->getExitBlock();

    if (!Exit)
        return nullptr;

    if (isEmptyUnconditionalBlock(Exit)) {
        BranchInst *BI = cast<BranchInst>(Exit->getTerminator());
        return BI->getSuccessor(0);
    }

    return Exit;
}

/*
 * Riconosce se un loop è guarded guardando il CFG.
 *
 * Un loop è guarded se esiste un blocco esterno al loop che:
 *
 *   1. termina con un branch condizionale;
 *   2. un ramo entra nel loop;
 *   3. l'altro ramo salta il loop e va alla continuation del loop;
 *   4. il blocco di guardia non contiene statement reali.
 */
GuardInfo getGuardInfo(Loop *L, LoopInfo &LI) {
    GuardInfo GI;

    BasicBlock *Preheader = L->getLoopPreheader();
    if (!Preheader)
        return GI;

    BasicBlock *Continuation = getLoopContinuation(L);
    if (!Continuation)
        return GI;

    for (BasicBlock *Pred : predecessors(Preheader)) {
         // Un guard deve essere esterno al loop.
        if (L->contains(Pred))
            continue;

        // Evita di scambiare un blocco di un altro loop per guard.
        if (LI.getLoopFor(Pred) != nullptr)
            continue;

        BranchInst *BI = dyn_cast<BranchInst>(Pred->getTerminator());
        if (!BI || !BI->isConditional())
            continue;

        // Se il blocco contiene una call tipo puts/printf/store/load ecc.,
        // allora non è un guard puro.
        GI.HasRealStatement = hasRealStatement(Pred);

        BasicBlock *Succ0 = BI->getSuccessor(0);
        BasicBlock *Succ1 = BI->getSuccessor(1);

        bool Succ0EntersLoop = (Succ0 == Preheader) || L->contains(Succ0);
        bool Succ1EntersLoop = (Succ1 == Preheader) || L->contains(Succ1);

        // Succ0 entra nel loop, Succ1 lo salta.
        if (Succ0EntersLoop && !Succ1EntersLoop) {
            if (Succ1 != Continuation)
                continue;

            GI.IsGuarded = true;
            GI.HasRealStatement = hasRealStatement(Pred);
            GI.GuardBlock = Pred;
            GI.LoopEntry = Succ0;
            GI.NonLoopSuccessor = Succ1;
            return GI;
        }

         // Succ1 entra nel loop, Succ0 lo salta.
        if (!Succ0EntersLoop && Succ1EntersLoop) {
            if (Succ0 != Continuation)
                continue;

            GI.IsGuarded = true;
            GI.HasRealStatement = hasRealStatement(Pred);
            GI.GuardBlock = Pred;
            GI.LoopEntry = Succ1;
            GI.NonLoopSuccessor = Succ0;
            return GI;
        }
    }

    return GI;
}

/*
 * Controlla l'adiacenza tra due loop L0 e L1.
 *
 * Gestisce quattro casi:
 *
 *   1. L0 non guarded, L1 non guarded
 *   2. L0 non guarded, L1 guarded
 *   3. L0 guarded, L1 non guarded
 *   4. L0 guarded, L1 guarded
 * 
 * Due loop sono adiacenti se tra la fine di L0 e l'ingresso/guard di L1
 * non viene eseguito nessuno statement reale.
 */
bool areAdjacent(Loop *L0, Loop *L1, LoopInfo &LI) {
    GuardInfo G0 = getGuardInfo(L0, LI);
    GuardInfo G1 = getGuardInfo(L1, LI);

    BasicBlock *Exit0 = L0->getExitBlock();

    if (!Exit0) {
        errs() << "L0 non ha un unico exit block\n";
        return false;
    }

    printBB("L0 header: ", L0->getHeader());
    printBB("L1 header: ", L1->getHeader());

    errs() << "L0 guarded? " << (G0.IsGuarded ? "yes" : "no") << "\n";
    errs() << "L1 guarded? " << (G1.IsGuarded ? "yes" : "no") << "\n";

    if (G0.IsGuarded)
        errs() << "L0 guard has real statement? "
           << (G0.HasRealStatement ? "yes" : "no") << "\n";

    if (G1.IsGuarded)
        errs() << "L1 guard has real statement? "
           << (G1.HasRealStatement ? "yes" : "no") << "\n";

    /*
     * Caso 1:
     * entrambi non guarded.
     *
     * Condizione:
     *   exit block di L0 == preheader di L1
     *
     * Però il blocco deve essere vuoto. Se contiene una puts/call/store,
     * allora tra i due loop c'è uno statement reale e non sono adiacenti.
     */
    if (!G0.IsGuarded && !G1.IsGuarded) {
        BasicBlock *Preheader1 = L1->getLoopPreheader();

        if (!Preheader1) {
            errs() << "L1 non ha preheader\n";
            return false;
        }

        printBB("Exit L0: ", Exit0);
        printBB("Preheader L1: ", Preheader1);

        return Exit0 == Preheader1 &&
               isEmptyUnconditionalBlock(Exit0);
    }

    /*
     * Caso 2:
     * L0 non guarded, L1 guarded.
     *
     * Condizione:
     *   l'uscita di L0 deve arrivare direttamente al guard di L1,
     *   oppure tramite soli blocchi vuoti.
     */
    if (!G0.IsGuarded && G1.IsGuarded) {
        printBB("Exit L0: ", Exit0);
        printBB("Guard L1: ", G1.GuardBlock);

        return !G1.HasRealStatement &&
       (Exit0 == G1.GuardBlock ||
        emptyPathToTarget(Exit0, G1.GuardBlock));
    }

    /*
     * Caso 3:
     * L0 guarded, L1 non guarded.
     *
     * Condizioni:
     *   - se salto L0, devo arrivare al preheader di L1;
     *   - se eseguo L0, l'exit di L0 deve arrivare al preheader di L1;
     *   - il preheader di L1 non deve contenere statement reali.
     */
    if (G0.IsGuarded && !G1.IsGuarded) {
        BasicBlock *Preheader1 = L1->getLoopPreheader();

        if (!Preheader1) {
            errs() << "L1 non ha preheader\n";
            return false;
        }

        printBB("NonLoopSuccessor L0: ", G0.NonLoopSuccessor);
        printBB("Exit L0: ", Exit0);
        printBB("Preheader L1: ", Preheader1);

        bool SkipL0GoesToL1 =
            G0.NonLoopSuccessor == Preheader1;

        bool ExitL0GoesToL1 =
            (Exit0 == Preheader1 && isEmptyUnconditionalBlock(Exit0)) ||
            emptyPathToTarget(Exit0, Preheader1);

        bool PreheaderIsEmpty =
            isEmptyUnconditionalBlock(Preheader1);

        return SkipL0GoesToL1 &&
               ExitL0GoesToL1 &&
               PreheaderIsEmpty;
    }

    /*
     * Caso 4:
     * entrambi guarded.
     *
     * Condizioni:
     *   - se salto L0, devo arrivare al guard di L1;
     *   - se eseguo L0, l'exit di L0 deve arrivare al guard di L1;
     *   - sono ammessi blocchi vuoti intermedi.
     */
    if (G0.IsGuarded && G1.IsGuarded) {
        printBB("NonLoopSuccessor L0: ", G0.NonLoopSuccessor);
        printBB("Exit L0: ", Exit0);
        printBB("Guard L1: ", G1.GuardBlock);

        bool SkipL0GoesToGuardL1 =
            G0.NonLoopSuccessor == G1.GuardBlock;

        bool ExitL0GoesToGuardL1 =
            Exit0 == G1.GuardBlock ||
            emptyPathToTarget(Exit0, G1.GuardBlock);

        return !G1.HasRealStatement &&
       SkipL0GoesToGuardL1 &&
       ExitL0GoesToGuardL1;
    }

    return false;
}

bool haveSameTripCount(Loop *L0, Loop *L1, ScalarEvolution &SE) {
    const SCEV *TripCount0 = SE.getBackedgeTakenCount(L0);
    const SCEV *TripCount1 = SE.getBackedgeTakenCount(L1);

    if (isa<SCEVCouldNotCompute>(TripCount0) || isa<SCEVCouldNotCompute>(TripCount1)) {
        errs() << "Trip count non calcolabile\n";
        return false;
    }

    errs() << "Trip count L0: " << *TripCount0 << "\n";
    errs() << "Trip count L1: " << *TripCount1 << "\n";

    if (TripCount0 == TripCount1) 
        return true;

    return SE.isKnownPredicate(ICmpInst::ICMP_EQ, TripCount0, TripCount1);

}

struct LoopFusionPass : public PassInfoMixin<LoopFusionPass> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        LoopInfo &LI = AM.getResult<LoopAnalysis>(F);

        // Le recupero già ora perché ti serviranno nei prossimi step
        // dell'assignment: control-flow equivalence e trip count.
        DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
        PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
        ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);

        (void)DT;
        (void)PDT;
        (void)SE;

        errs() << "\nFunction: " << F.getName() << "\n";

        std::vector<Loop *> TopLevelLoops = LI.getTopLevelLoops();

        if (TopLevelLoops.size() < 2) {
            errs() << "Meno di due top-level loop: niente da confrontare\n";
            return PreservedAnalyses::all();
        }

        // Ordino i top-level loop in base alla posizione dell'header nella funzione.
        // Non assumo che LoopInfo li restituisca già nell'ordine del CFG.
        DenseMap<BasicBlock *, unsigned> BlockOrder;
        unsigned Order = 0;

        for (BasicBlock &BB : F) {
            BlockOrder[&BB] = Order++;
        }

        std::sort(TopLevelLoops.begin(), TopLevelLoops.end(),
                  [&](Loop *A, Loop *B) {
                      return BlockOrder[A->getHeader()] <
                             BlockOrder[B->getHeader()];
                  });

        for (size_t i = 0; i + 1 < TopLevelLoops.size(); i++) {
            Loop *L0 = TopLevelLoops[i];
            Loop *L1 = TopLevelLoops[i + 1];

            errs() << "\nAnalizzo coppia di loop " << i
                   << " e " << i + 1 << "\n";

            if (areAdjacent(L0, L1, LI)) {
                errs() << "Loop " << i << " e loop " << i + 1
                       << " sono ADIACENTI\n";
            } else {
                errs() << "Loop " << i << " e loop " << i + 1
                       << " NON sono adiacenti\n";
            }

            if(!haveSameTripCount(L0, L1, SE)) {
                errs() << "Loop " << i << " e loop " << i + 1
                       << " NON hanno lo stesso trip count\n";
            } else {
                errs() << "Loop " << i << " e loop " << i + 1
                       << " hanno lo stesso trip count\n";
            }
        }

        return PreservedAnalyses::all();
    }
};

} // namespace

llvm::PassPluginLibraryInfo getLoopFusionPassPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "LoopFusionPass", LLVM_VERSION_STRING,
            [](PassBuilder &PB) {
                PB.registerPipelineParsingCallback(
                    [](StringRef Name,
                       FunctionPassManager &FPM,
                       ArrayRef<PassBuilder::PipelineElement>) {
                        if (Name == "loopfusion-pass") {
                            FPM.addPass(LoopFusionPass());
                            return true;
                        }

                        return false;
                    });
            }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return getLoopFusionPassPluginInfo();
}