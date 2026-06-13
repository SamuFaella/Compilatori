#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/IR/PatternMatch.h"
#include <cmath>

using namespace llvm;
using namespace llvm::PatternMatch;

namespace {

struct OptimizationPass : public PassInfoMixin<OptimizationPass> {

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    bool changed = false;

    for (auto &BB : F) {
        //  Prima raccogli tutte le istruzioni per evitare problemi mentre si modificano/cancellano le istruzioni all'interno
        //  di un BasicBlock. Posso fare L'eraseFromParent() in modo sicuro
        std::vector<Instruction*> InstList;
        for (auto &I : BB)
            InstList.push_back(&I);

        // Ora puoi iterare in modo sicuro
        for (Instruction *I : InstList) {
            errs() << "Analizzo istruzione: ";
            I->print(errs());
            errs() << "\n";

            Value *X;

            // 1. Algebraic Identity
            if (match(I, m_Add(m_Value(X), m_Zero())) || match(I, m_Add(m_Zero(), m_Value(X))) ||
                match(I, m_Mul(m_Value(X), m_One())) || match(I, m_Mul(m_One(), m_Value(X)))) {
                errs() << "Algebraic Identity: ";
                I->print(errs());
                errs() << "\n";
                I->replaceAllUsesWith(X); // Sostituisco l'istruzione con il suo operando
                I->eraseFromParent(); // Cancella l'istruzione
                changed = true;
                continue;
            }

            // 2. Strength Reduction per moltiplicazioni/divisioni con costanti
            ConstantInt *C;

            // Riconosce moltiplicazioni del tipo:
            // x * C oppure C * x
            if (match(I, m_Mul(m_Value(X), m_ConstantInt(C))) || match(I, m_Mul(m_ConstantInt(C), m_Value(X)))) {
                uint64_t val = C->getZExtValue(); // Estrae il valore della costante come intero unsigned a 64 bit

                // Caso: x * 2^n  ==>  x << n
                // Controlla che val sia diverso da 0 e sia una potenza di 2
                if ((val & (val - 1)) == 0) {
                    
                    errs() << "Strength Reduction Multiplication: " << val << " è potenza di 2 → SHL\n";

                    // n = log2(val), cioè il numero di posizioni dello shift
                    unsigned n = Log2_64(val); 
                    // IRBuilder permette di inserire nuove istruzioni prima di I
                    IRBuilder<> Builder(I);

                    // Crea l'istruzione: x << n
                    Value *NewVal =
                        Builder.CreateShl(X, ConstantInt::get(X->getType(), n));

                    // Sostituisce tutti gli usi della moltiplicazione con il nuovo valore
                    I->replaceAllUsesWith(NewVal);

                    // Elimina la vecchia istruzione mul
                    I->eraseFromParent();

                    changed = true;
                    continue;

                // Caso: x * (2^n - 1)  ==>  (x << n) - x
                // Esempio: x * 15 = x * (16 - 1) = (x << 4) - x
                } else if (((val + 1) & val) == 0) {
                    
                    errs() << "Strength Reduction Multiplication: " << val << " è 2^n - 1 → (x << n) - x\n";
                    unsigned n = Log2_64(val + 1);
                    IRBuilder<> Builder(I);
                    // Crea x << n
                    Value *Shift =
                        Builder.CreateShl(X, ConstantInt::get(X->getType(), n));

                    // Crea (x << n) - x
                    Value *NewVal = Builder.CreateSub(Shift, X);
                    I->replaceAllUsesWith(NewVal);
                    I->eraseFromParent();
                    changed = true;
                    continue;
                }
            }
            // Strength Reduction per divisioni intere
            // Gestisce divisioni per potenze di 2:
            // udiv x, 2^n  ==>  lshr x, n
            // sdiv x, 2^n  ==>  ashr x, n solo se x è sicuramente non negativo
            //getOpcode() --> Restituisce il codice operativo dell'istruzione, ovvero serve per identificare che tipo di istruzione è
            if (I->getOpcode() == Instruction::UDiv || I->getOpcode() == Instruction::SDiv) {
                Value *X;
                ConstantInt *C;

                // Riconosce una divisione binaria con secondo operando costante:
                // x / C
                if (match(I, m_BinOp(m_Value(X), m_ConstantInt(C)))) {
                    uint64_t val = C->getZExtValue(); //Estrae il divisore

                     // La divisione viene ottimizzata solo se il divisore è una potenza di 2
                    if (val != 0 && (val & (val - 1)) == 0) {

                        // n = log2(val), cioè lo shift equivalente alla divisione
                        unsigned n = Log2_64(val);

                        IRBuilder<> Builder(I);

                        // Costante che indica di quante posizioni fare lo shift
                        Value *ShiftAmount = ConstantInt::get(X->getType(), n);
                        Value *NewVal = nullptr;

                        // Divisione unsigned:
                        // udiv x, 8  ==>  lshr x, 3
                        if (I->getOpcode() == Instruction::UDiv) {
                            errs() << "Strength Reduction UDiv: / " << val << " -> LShr " << n << "\n";
                            NewVal = Builder.CreateLShr(X, ShiftAmount);
                        }

                        // Divisione signed:
                        // sdiv x, 8 può diventare ashr x, 3 solo se x >= 0
                        else if (I->getOpcode() == Instruction::SDiv) {
                            bool XIsNonNegative = false;

                            // Caso semplice: il dividendo è una costante nota a compile-time
                            if (auto *XC = dyn_cast<ConstantInt>(X)) {
                                XIsNonNegative = XC->getSExtValue() >= 0;
                            }

                            // Se non posso dimostrare che x è non negativo, non ottimizzo
                            if (!XIsNonNegative) {
                                errs() << "Strength Reduction SDiv non applicata: "
                                    << "non posso garantire che il dividendo sia >= 0\n";
                                continue;
                            }

                            errs() << "Strength Reduction SDiv: / "
                                << val << " -> AShr " << n << "\n";

                            NewVal = Builder.CreateAShr(X, ShiftAmount);
                        }

                        I->replaceAllUsesWith(NewVal);
                        I->eraseFromParent();
                        changed = true;
                        continue;
                    }
                }
            }

            // 3. Multi-Instruction Optimization
            // Riconosce sequenze di istruzioni che si annullano a vicenda:
            //
            // a = b + C
            // c = a - C
            //
            // quindi:
            // c = b
            if(I->getOpcode() == Instruction::Sub){

                // Primo operando della sub, cioè il valore da cui si sottrae
                Value *A = I->getOperand(0);

                ConstantInt *C1;

                // Riconosce una sottrazione del tipo:
                // X - C1
                if (match(I, m_Sub(m_Value(X), m_ConstantInt(C1)))) {

                    // Controlla se X è il risultato di un'istruzione precedente
                    Instruction *Prev = dyn_cast<Instruction>(A);

                    // Verifica che l'istruzione precedente sia una add
                    if (Prev && Prev->getOpcode() == Instruction::Add){
                        Value *B;
                        ConstantInt *C2;

                        // Riconosce:
                        // B + C2
                        //
                        // Se poi ho:
                        // (B + C2) - C1
                        //
                        // e C1 == C2, allora il risultato è B
                        if (match(Prev, m_Add(m_Value(B), m_ConstantInt(C2))) && C1->getValue() == C2->getValue()) {
                            errs() << "Multi-Instruction Optimization: ";
                            I->print(errs());
                            errs() << "\n";
                            errs() << "Sostituito con: ";
                            B->print(errs());
                            errs() << "\n";

                            // Sostituisce la sub con il valore originale B
                            I->replaceAllUsesWith(B);

                            // Elimina la sub
                            I->eraseFromParent();
                            changed = true;
                            continue;
                        }
                    }
                }
            }
            // Caso simmetrico della Multi-Instruction Optimization:
            //
            // a = b - C
            // c = a + C
            //
            // quindi:
            // c = b
            else if(I->getOpcode() == Instruction::Add){
                Value *A = I->getOperand(0);
                ConstantInt *C1;
                if (match(I, m_Add(m_Value(X), m_ConstantInt(C1)))) {
                    Instruction *Prev = dyn_cast<Instruction>(A);
                    if (Prev && Prev->getOpcode() == Instruction::Sub){
                        Value *B;
                        ConstantInt *C2;
                        if (match(Prev, m_Sub(m_Value(B), m_ConstantInt(C2))) && C1->getValue() == C2->getValue()) {
                            errs() << "Multi-Instruction Optimization: ";
                            I->print(errs());
                            errs() << "\n";
                            errs() << "Sostituito con: ";
                            B->print(errs());
                            errs() << "\n";
                            I->replaceAllUsesWith(B);
                            I->eraseFromParent();
                            changed = true;
                            continue;
                        }
                    }
                }
            }

        }
    }

    return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
    }
};


} //namespace

// Pass registration
llvm::PassPluginLibraryInfo getOptimizationPassPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "OptimizationPass", LLVM_VERSION_STRING,
            [](PassBuilder &PB) {
                PB.registerPipelineParsingCallback(
                    [](StringRef Name, FunctionPassManager &FPM,
                       ArrayRef<PassBuilder::PipelineElement>) {
                        if (Name == "opt-pass") {
                            FPM.addPass(OptimizationPass());
                            return true;
                        }
                        return false;
                    });
            }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return getOptimizationPassPluginInfo();
}