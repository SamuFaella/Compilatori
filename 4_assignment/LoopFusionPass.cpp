#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/BasicBlock.h"
#include <set>
using namespace llvm;

namespace {

bool isGuardLoop(Loop *L, ScalarEvolution &SE){
    //SE.getBackedgeTakenCount(L) -->  Stimare quante volte viene eseguito il backedge durante l'esecuzione del loop
    const SCEV *TripCount = SE.getBackedgeTakenCount(L);

    errs() << "TripCount: " << *TripCount << "\n";

    //Serve a verificare se LLVM non è riuscito a calcolare il numero di iterazioni del loop usando l'analisi SCEV
    if(isa<SCEVCouldNotCompute>(TripCount)){
        errs() << "Could not compute trip count\n"; //Stampa un messaggio di errore
        return false;
    }

    // Caso: trip count è (0 smax %N)
    if(const SCEVSMaxExpr *SMax = dyn_cast<SCEVSMaxExpr>(TripCount)){
        const SCEV *Op0 = SMax->getOperand(0);
        const SCEV *Op1 = SMax->getOperand(1);

        // Controllo se uno degli operandi è zero costante
        auto isZeroConst = [](const SCEV *Op) {
            if (const SCEVConstant *C = dyn_cast<SCEVConstant>(Op))
                return C->getAPInt().isZero();
            return false;
        };

        if (isZeroConst(Op0) || isZeroConst(Op1)) {
            errs() << "TripCount è (0 smax %N): loop is guard\n";
            return true;
        }
        // max(X, Y) ma nessuno è zero → non guard
        return false;
    }
    
    //Controllo CFG: Il loop è protetto da un if(N>0)?
    BasicBlock *PreHeader = L->getLoopPreheader();
    if(!PreHeader){
        errs() << "Preheader non trovato\n";
        return false;
    }
    for(BasicBlock *Pred : predecessors(PreHeader)){
        if(BranchInst *BI = dyn_cast<BranchInst>(Pred->getTerminator())){
            if(BI->isConditional()){
                Value *Cond = BI->getCondition();
                if(ICmpInst *ICmp = dyn_cast<ICmpInst>(Cond)){
                    if(ICmp->isSigned()){
                        Value *Op0 = ICmp->getOperand(0);
                        Value *Op1 = ICmp->getOperand(1);

                        if(ConstantInt *CI = dyn_cast<ConstantInt>(Op1)){
                            if(CI->isZero() && ICmp->getPredicate() == ICmpInst::ICMP_SGT){
                                errs() << "Loop è protetto da un if(N>0)\n";
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

     // Caso semplice: trip count costante zero
    if (const SCEVConstant *C = dyn_cast<SCEVConstant>(TripCount)) {
        if (C->getAPInt().isZero()) {
            errs() << "TripCount è 0: Il loop non ha esecuzioni\n";
            return false;
        }
    }

    return false; 
}
BasicBlock *getActualPreheader(Loop *L) {
    if (BasicBlock *PH = L->getLoopPreheader())
        return PH;

    // Fallback: cerca predecessore fuori dal loop
    for (BasicBlock *Pred : predecessors(L->getHeader())) {
        if (!L->contains(Pred))
            return Pred;
    }

    return nullptr;
}

bool areAdjacent(Loop *L0, Loop *L1, LoopInfo &LI, DominatorTree &DT, ScalarEvolution &SE) {
    // Verifica preliminare: L1 non deve essere l'entry block della funzione
    BasicBlock *Header1 = L1->getHeader();
    if (Header1 == &Header1->getParent()->getEntryBlock()) {
        return false;
    }

    bool Guard0 = isGuardLoop(L0, SE);
    bool Guard1 = isGuardLoop(L1, SE);

    // === Caso: loop guarded ===
    if (Guard0 && Guard1) {
        BasicBlock *Preheader0 = L0->getLoopPreheader();
        if (!Preheader0) return false;

        // Trova il blocco guardia (predecessore del preheader)
        BasicBlock *GuardBlock = Preheader0->getSinglePredecessor();
        if (!GuardBlock) return false;

        BranchInst *BI = dyn_cast<BranchInst>(GuardBlock->getTerminator());
        if (!BI || !BI->isConditional()) return false;

        // Trova il successore che non entra nel loop (ramo else)
        BasicBlock *NonLoopSucc = nullptr;
        for (unsigned i = 0; i < 2; ++i) {
            BasicBlock *Succ = BI->getSuccessor(i);
            if (!L0->contains(Succ)) {
                NonLoopSucc = Succ;
                break;
            }
        }

        if (!NonLoopSucc) return false;

        // Verifica che il successore non-loop sia l'entry di L1
        BasicBlock *L1Entry = L1->getLoopPreheader() ? L1->getLoopPreheader() : L1->getHeader();
        return NonLoopSucc == L1Entry;
    }
    // === Caso: loop non guarded ===
    else if (!Guard0 && !Guard1) {
        BasicBlock *FirstLoopExit = L0->getExitBlock();  // Uscita canonica
        BasicBlock *SecondLoopPreheader = L1->getLoopPreheader();
        errs() << "FirstLoopExit: " << *FirstLoopExit << "\n";
        errs() << "SecondLoopPreheader: " << *SecondLoopPreheader << "\n";

        if(!FirstLoopExit || !SecondLoopPreheader) return false;
        else if (FirstLoopExit == SecondLoopPreheader) 
            return true;
    }
    // === Caso misto (uno guarded e l'altro no) ===
    else {
        return false;
    }
    return 0;
}



struct LoopFusionPass : public PassInfoMixin<LoopFusionPass> {

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
        DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
        PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
        ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);

        std::vector<Loop *> TopLevelLoops = LI.getTopLevelLoops();


        for(size_t i = 0; i + 1 < TopLevelLoops.size(); i++)
        {
            Loop *L0 = TopLevelLoops[i];
            Loop *L1 = TopLevelLoops[i+1];

            
            if (areAdjacent(L0, L1, LI, DT, SE)) {
                errs() << "Loop " << i << " e loop " << i + 1 << " sono ADIACENTI\n";
            } else {
                errs() << "Loop " << i << " e loop " << i + 1 << " NON sono adiacenti\n";
            }
        }

        return PreservedAnalyses::all();
        }
  
    };

} // namespace


// Pass registration
llvm::PassPluginLibraryInfo getLoopFusionPassPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "LoopFusionPass", LLVM_VERSION_STRING,
            [](PassBuilder &PB) {
                PB.registerPipelineParsingCallback(
                    [](StringRef Name, FunctionPassManager &FPM,
                       ArrayRef<PassBuilder::PipelineElement>) {
                        if (Name == "loopfusion-pass") {
                            FPM.addPass(LoopFusionPass());
                            return true;
                        }
                        return false;
                    });
            }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return getLoopFusionPassPluginInfo();
}
