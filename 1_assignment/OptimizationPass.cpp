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

            // 2. Strength Reduction
            ConstantInt *C;
            if (match(I, m_Mul(m_Value(X), m_ConstantInt(C))) || match(I, m_Mul(m_ConstantInt(C), m_Value(X)))) {
                uint64_t val = C->getZExtValue(); // Estrarre il valore numerico da una costante

                if ((val & (val - 1)) == 0) {
                    // Potenza di 2
                    errs() << "Strength Reduction Multiplication: " << val << " è potenza di 2 → SHL\n";
                    unsigned n = Log2_64(val); // Calcola il logaritmo in base 2
                    IRBuilder<> Builder(I); // Crea un IRBuilder per costruire nuove istruzioni, come ad esempio add,shl,mul,ecc..
                    //CreateShl() --> Crea un'istruzione di shift a sinistra
                    Value *NewVal = Builder.CreateShl(X, ConstantInt::get(X->getType(), n));
                    I->replaceAllUsesWith(NewVal);
                    I->eraseFromParent();
                    changed = true;
                    continue;
                } else if (((val + 1) & val) == 0) {
                    // 2^n - 1
                    errs() << "Strength Reduction Multiplication: " << val << " è 2^n - 1 → (x << n) - x\n";
                    unsigned n = Log2_64(val + 1);
                    IRBuilder<> Builder(I);
                    Value *Shift = Builder.CreateShl(X, ConstantInt::get(X->getType(), n));
                    //CreateSub() --> Crea un'istruzione di sottrazione
                    //Shift è il risultato di un'istruzione di shift a sinistra, che viene fatta sopra
                    Value *NewVal = Builder.CreateSub(Shift, X);
                    I->replaceAllUsesWith(NewVal);
                    I->eraseFromParent();
                    changed = true;
                    continue;
                }
            }
            //getOpcode() --> Restituisce il codice operativo dell'istruzione, ovvero serve per identificare che tipo di istruzione è
            if ((I->getOpcode() == Instruction::UDiv || I->getOpcode() == Instruction::SDiv)) {
                Value *X;
                ConstantInt *C;
                //m_BinOp() --> Restituisce un'istruzione binaria, in questo caso una divisione
                if (match(I, m_BinOp(m_Value(X), m_ConstantInt(C)))) {
                    uint64_t val = C->getZExtValue();
                    if ((val & (val - 1)) == 0) {
                        errs() << "Strength Reduction division with value -->  " << val;
                        errs() << "\n";

                        unsigned n = Log2_64(val);
                        IRBuilder<> Builder(I);
                        //CreateLShr() --> Crea un'istruzione di shift a destra
                        //X --> è il valore che viene passato come operando
                        //ConstantInt::get() --> Crea un'oggetto di tipo ConstantInt
                        // X->getType() --> Restituisce il tipo di X
                        //n --> valore di bit di shift
                        Value *NewVal = Builder.CreateLShr(X, ConstantInt::get(X->getType(), n));
                        I->replaceAllUsesWith(NewVal);
                        I->eraseFromParent();
                        changed = true;
                        continue;
                    }
                }
            }

            //3 Caso -->  Multi-Instruction Optimization ---> a = b+C, c = a-C --> b=c
            if(I->getOpcode() == Instruction::Sub){
                Value *A = I->getOperand(0);
                ConstantInt *C1;
                if (match(I, m_Sub(m_Value(X), m_ConstantInt(C1)))) {
                    Instruction *Prev = dyn_cast<Instruction>(A);
                    if (Prev && Prev->getOpcode() == Instruction::Add){
                        Value *B;
                        ConstantInt *C2;
                        if (match(Prev, m_Add(m_Value(B), m_ConstantInt(C2))) && C1->getValue() == C2->getValue()) {
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
