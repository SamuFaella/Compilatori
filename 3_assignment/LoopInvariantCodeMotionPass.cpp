#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include <set>

using namespace llvm;

namespace {

struct LoopInvariantCodeMotionPass : public PassInfoMixin<LoopInvariantCodeMotionPass> {

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
        DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);

        for (Loop *L : LI) {
            //Controllo di avere un preHeader valido per poi spostare le istruzioni
            if (!L->getLoopPreheader()) continue;

            //Mi salvo il preHeader, dove verranno messe le istruzioni che posso spostare
            BasicBlock *Preheader = L->getLoopPreheader();
            //set vuoto che conterrà i puntatori alle istruzioni che sono state riconosciute come Loop-Invariant
            std::set<Instruction*> LoopInvariantInsts;

            //Serve ad identificare tutte le istruzioni Loop-Invariant, perchè alcune istruzioni lo possono diventare solo dopo che lo diventano le istruzioni
            //da cui dipendono
            bool Changed;
            do {
                Changed = false;

                //Scorro tutti i BB che fanno parte del Loop
                for (BasicBlock *BB : L->blocks()) {
                    //Scorro tutte le istruzioni nel blocco 
                    for (Instruction &I : *BB) {
                        errs() << "Analizzo: " << I << "\n";
                        
                        //Controllo se l'istruzione su cui lavoro è prensente nel set di istruzioni LoopInvariant, se è presente salto all'istruzione successiva
                        //altrimenti, ci lavoro e provo a verificare se può diventare LoopInvariant
                        if (LoopInvariantInsts.count(&I)) {
                            errs() << " - Skipped (already invariant): " << I << "\n";
                            continue;
                        }
                        if (I.getParent() == Preheader) {
                            errs() << " - Skipped (already in preheader): " << I << "\n";
                            continue;
                        }
                        if (!isLoopInvariant(I, L, LoopInvariantInsts)) {
                            errs() << " - Skipped (not loop invariant): " << I << "\n";
                            continue;
                        }
                        if (!isSafeToMove(I, L)) {
                            errs() << " - Skipped (not safe to move): " << I << "\n";
                            continue;
                        }
                            
                        /*
                        //I.getParent() --> restituisce il Basic block che contiene l'istruzione I
                        if (!dominatesAllExits(I.getParent(), L, DT)) {
                            errs() << " - Skipped (doesn't dominate all exits): " << I << "\n";
                            continue;
                        }*/
                        
                        if (!dominatesAllUses(&I, L, DT)) {
                            errs() << " - Skipped (doesn't dominate all uses): " << I << "\n";
                            continue;
                        }
                        if (!isOnlyDefinitionInLoop(&I, L)) {
                            errs() << " - Skipped (not only definition): " << I << "\n";
                            continue;
                        }
                        

                        LoopInvariantInsts.insert(&I);
                        Changed = true;
                    }
                }
            } while (Changed);
            
            errs() << "Istruzioni loop-invariant al quale è possibile applicare la code motion trovate:\n";
            for (Instruction *I : LoopInvariantInsts) {
                errs() << " - " << *I << "\n";
            }
            if (LoopInvariantInsts.empty()) {
                errs() << " - Nessuna istruzione trovata come loop-invariant al quale è possibile applicare la code motion.\n";
            }

            // Sposta le istruzioni nel preheader
            IRBuilder<> Builder(Preheader->getTerminator());
            for (Instruction *I : LoopInvariantInsts) {
                errs() << "Sposto: " << *I << "\n";
                I->moveBefore(Preheader->getTerminator());
            }
        }

        return PreservedAnalyses::none();
    }

    //Funzione per verificare se l'istruzione su cui sto lavorando è LoopInvariant
    bool isLoopInvariant(Instruction &I, Loop *L, const std::set<Instruction*> &InvariantInsts) {
        //isTerminator() --> Serve a controllare se I non è un'istruzione che termina il basic block (br,ret,...)
        //mayHaveSideEffects() --> ritorna vero se l'istruzione che chiama questa funzione ha degli SideEffects, non può essere LoopInvariant
        //mayReadOrWriteMemory() --> ritorna vero se l'istruzione legge o scrive sulla memoria, non può essere LoopInvariant
        //Se si verifica almeno una di queste l'istruzione I non è LoopInvariant
        if (I.isTerminator() || I.mayHaveSideEffects()){
            errs() << " - Skipped (memory access): " << I << "\n";
            return false;
        }
        //Ciclo che itera su tutti gli operandi del'instruzione I
        for (Use &Op : I.operands()) {
            //cast dinamico per convertire Op in un oggetto di tipo Instuction, c'è si verifica se è possibile effetturare il cast
            if (Instruction *OpI = dyn_cast<Instruction>(Op)) {
                // con contains verifico se l'istruzione OpI fa parte del Loop corrente
                // Con count() controllo se l'istruzione non è già stata marcata come LoopInvariant --> se non è stata marcata non può essere spostata
                if (L->contains(OpI) && !InvariantInsts.count(OpI)){
                    errs() << "  --> OP non invariabile: " << *OpI << "\n";
                    return false; //se entrambe vere, abbiamo OpI che si trova all'interno del Loop, ma non è invariabile, quindi non può essere sposatata fuori dal loop
                }
            }
        }
        return true; //se arriva a questo punto, l'istruzione che stiamo controllando è LoopInvariant
    }

    //Controllo che l'istruzione I su cui sto lavorando è possibile spostarla --> ovvero non ha sideEffects e non legge o scrive in memoria
    bool isSafeToMove(Instruction &I, Loop *L) {
        // Verifica se l'istruzione ha side-effects (ad esempio, chiamate di funzione o operazioni che alterano lo stato)
        if (I.mayHaveSideEffects())
            return false;

        // Verifica se l'istruzione può leggere o scrivere sulla memoria
        if (I.mayReadFromMemory() || I.mayWriteToMemory())
            return false;

        // Se è un'istruzione Load, assicuriamoci che non ci siano Store nel loop che influenzano il valore caricato
        if (auto *LI = dyn_cast<LoadInst>(&I)) {
            Value *Ptr = LI->getPointerOperand();
            for (BasicBlock *BB : L->blocks()) {
                for (Instruction &Inst : *BB) {
                    if (auto *SI = dyn_cast<StoreInst>(&Inst)) {
                        if (SI->getPointerOperand() == Ptr)
                            return false;  // Se la stessa memoria viene modificata nel loop, non possiamo spostarla
                    }
                }
            }
        }

        return true;  // Se non sono stati trovati problemi, l'istruzione è sicura da spostare
        
    }

    //Funzione che verifica se il basic block dell'istruzione su cui stiamo lavorando domina tutte le uscite
    bool dominatesAllExits(BasicBlock *BB, Loop *L, DominatorTree &DT) {
        //vettore per conservare i puntatori ai blocchi base che sono uscite nel Loop
        SmallVector<BasicBlock*, 8> ExitBlocks;
        //Riempo nel vettore tutti i blocchi base che sono successori del Loop L
        L->getExitBlocks(ExitBlocks);
        
        //Ciclo su questi blocchi che mi sono salvato
        for (BasicBlock *Exit : ExitBlocks) {
            //Controlo se il basic block che contiene la mia istruzione corrente domini tutti i blocchi all'interno del Loop L
            if (!DT.dominates(BB, Exit))
                return false; //Entro qua se l'istruzione non è vera per tutti i blocchi --> il basic block passato non domina tutte le uscite
        }
        return true; //Il blocco BB domina tutte le uscite
    }

    //Funzione che server per controllare che l'istruzione passata, domini tutti gli usi all'interno del loop
    bool dominatesAllUses(Instruction *I, Loop *L, DominatorTree &DT) {
        //I->users() --> essendo che I produce un valore, questa funzione serve a sapere quali altre istruzione lo utilizzando come operando
        for (User *U : I->users()) {
            if (Instruction *UseI = dyn_cast<Instruction>(U)) {
                //L->contains(UseI) --> server a controllare che l'uso dell'istruzione I si trovi dentro il loop
                if (L->contains(UseI)) {
                    //Verifico se il basic block che contiene I domina quello che in cui si trova UseI
                    if (!DT.dominates(I->getParent(), UseI->getParent()))
                        return false; //se non lo domina ritorniamo falso e non si può spostare I fuori dal loop
                }
            }
        }
        return true;
    }

    //Funzione per verificare che l'istruzione I sia l'unica a definire un certo valore(con quel nome) all'interno del loop
    bool isOnlyDefinitionInLoop(Instruction *I, Loop *L) {
        //serve per ottenere il valore definito all'interno di I
        Value *Defined = &*I;
        //non mi baso sui nomi
        if (!Defined->hasName()) return true;

        //con i 2 for faccio in modo di scorrere le istruzione del loop
        for (BasicBlock *BB : L->blocks()) {
            for (Instruction &OtherI : *BB) {
                //serve per saltare se stessa e non trovare un falso-positivo
                if (&OtherI == I) continue;
                //Se trovo un'altra istruzione con lo stesso nome di I --> ci sono più definizioni per quella variabile --> restituisco falso
                if (OtherI.getName() == I->getName())
                    return false;
            }
        }
        return true;
    }
};

} // namespace

// Pass registration
llvm::PassPluginLibraryInfo getLoopInvariantCodeMotionPassPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "LoopInvariantCodeMotionPass", LLVM_VERSION_STRING,
            [](PassBuilder &PB) {
                PB.registerPipelineParsingCallback(
                    [](StringRef Name, FunctionPassManager &FPM,
                       ArrayRef<PassBuilder::PipelineElement>) {
                        if (Name == "licm-pass") {
                            FPM.addPass(LoopInvariantCodeMotionPass());
                            return true;
                        }
                        return false;
                    });
            }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return getLoopInvariantCodeMotionPassPluginInfo();
}
