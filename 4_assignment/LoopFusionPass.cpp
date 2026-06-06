#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Operator.h"

#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/ValueTracking.h"

#include "llvm/IR/Dominators.h"

#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"

#include "llvm/Support/raw_ostream.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/DenseMap.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <set>
#include <vector>

using namespace llvm;

namespace {

// Informazioni sul guard di un loop.
struct GuardInfo {
    bool IsGuarded = false;
    bool HasRealStatement = false;

    BasicBlock *GuardBlock = nullptr;        // blocco del branch condizionale
    BasicBlock *LoopEntry = nullptr;         // ramo che entra nel loop
    BasicBlock *NonLoopSuccessor = nullptr;  // ramo che salta il loop
};

enum class DependenceResult {
    Safe,
    Negative,
    UnknownUnsafe
};

enum class SimpleDepResult {
    Safe,
    Negative,
    Unknown
};

struct LinearExpr {
    int64_t Coeff = 0;
    int64_t Const = 0;
};

struct SimpleAccess {
    Instruction *Inst = nullptr;
    Value *Base = nullptr;
    int64_t Offset = 0;
    bool IsLoad = false;
    bool IsStore = false;
};

// Stampa un BasicBlock anche quando non ha nome testuale.
void printBB(StringRef Label, BasicBlock *BB) {
    errs() << Label;

    if (!BB) {
        errs() << "null\n";
        return;
    }

    BB->printAsOperand(errs(), false);
    errs() << "\n";
}

// True se BB contiene solo un branch incondizionato.
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

// True se BB contiene istruzioni reali del programma.
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

// True se From arriva a To passando solo da blocchi vuoti.
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

bool sameOrEmptyPathToTarget(BasicBlock *From, BasicBlock *To) {
    if (!From || !To)
        return false;

    if (From == To)
        return true;

    return emptyPathToTarget(From, To);
}

// Ritorna il punto raggiunto dopo la fine del loop.
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

// Riconosce se L è guarded guardando solo la struttura del CFG.
GuardInfo getGuardInfo(Loop *L, LoopInfo &LI) {
    GuardInfo GI;

    BasicBlock *Preheader = L->getLoopPreheader();
    if (!Preheader)
        return GI;

    for (BasicBlock *Pred : predecessors(Preheader)) {
        // Un guard deve essere esterno al loop.
        if (L->contains(Pred))
            continue;

        // Evita di scambiare un blocco appartenente a un altro loop per guard.
        if (LI.getLoopFor(Pred) != nullptr)
            continue;

        BranchInst *BI = dyn_cast<BranchInst>(Pred->getTerminator());
        if (!BI || !BI->isConditional())
            continue;

        BasicBlock *Succ0 = BI->getSuccessor(0);
        BasicBlock *Succ1 = BI->getSuccessor(1);

        bool Succ0EntersLoop = (Succ0 == Preheader) || L->contains(Succ0);
        bool Succ1EntersLoop = (Succ1 == Preheader) || L->contains(Succ1);

        // Succ0 entra nel loop, Succ1 lo salta.
        if (Succ0EntersLoop && !Succ1EntersLoop) {
            GI.IsGuarded = true;
            GI.HasRealStatement = hasRealStatement(Pred);
            GI.GuardBlock = Pred;
            GI.LoopEntry = Succ0;
            GI.NonLoopSuccessor = Succ1;
            return GI;
        }

        // Succ1 entra nel loop, Succ0 lo salta.
        if (!Succ0EntersLoop && Succ1EntersLoop) {
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

// Controlla l'adiacenza tra due loop.
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

    BasicBlock *Entry1 = nullptr;

    if (G1.IsGuarded)
        Entry1 = G1.GuardBlock;
    else
        Entry1 = L1->getLoopPreheader();

    if (!Entry1) {
        errs() << "Entry di L1 non trovata\n";
        return false;
    }

    printBB("Exit L0: ", Exit0);
    printBB("Entry L1: ", Entry1);

    bool ExitGoesToEntry1 = sameOrEmptyPathToTarget(Exit0, Entry1);

    bool Entry1IsClean = true;

    if (G1.IsGuarded)
        Entry1IsClean = !G1.HasRealStatement;
    else
        Entry1IsClean = isEmptyUnconditionalBlock(Entry1);

    if (ExitGoesToEntry1 && Entry1IsClean) {
        errs() << "Adiacenza locale verificata: exit L0 arriva a entry L1 senza statement reali\n";
        return true;
    }

    errs() << "Adiacenza locale fallita\n";
    return false;
}

// Controlla se i due loop hanno lo stesso backedge-taken count.
bool haveSameTripCount(Loop *L0, Loop *L1, ScalarEvolution &SE) {
    const SCEV *TripCount0 = SE.getBackedgeTakenCount(L0);
    const SCEV *TripCount1 = SE.getBackedgeTakenCount(L1);

    if (isa<SCEVCouldNotCompute>(TripCount0) ||
        isa<SCEVCouldNotCompute>(TripCount1)) {
        errs() << "Trip count non calcolabile\n";
        return false;
    }

    errs() << "Trip count L0: " << *TripCount0 << "\n";
    errs() << "Trip count L1: " << *TripCount1 << "\n";

    if (TripCount0 == TripCount1)
        return true;

    return SE.isKnownPredicate(ICmpInst::ICMP_EQ, TripCount0, TripCount1);
}

// Control-flow equivalence: L0 domina L1 e L1 postdomina L0.
bool areControlFlowEquivalent(Loop *L0,
                              Loop *L1,
                              DominatorTree &DT,
                              PostDominatorTree &PDT) {
    BasicBlock *Header0 = L0->getHeader();
    BasicBlock *Header1 = L1->getHeader();

    bool Dom = DT.dominates(Header0, Header1);
    bool PostDom = PDT.dominates(Header1, Header0);

    errs() << "L0 domina L1? " << (Dom ? "yes" : "no") << "\n";
    errs() << "L1 post-domina L0? " << (PostDom ? "yes" : "no") << "\n";

    return Dom && PostDom;
}

// Raccoglie solo load/store nel loop.
void collectMemoryInstructions(Loop *L, SmallVectorImpl<Instruction *> &Insts) {
    for (BasicBlock *BB : L->blocks()) {
        for (Instruction &I : *BB) {
            if (isa<LoadInst>(&I) || isa<StoreInst>(&I))
                Insts.push_back(&I);
        }
    }
}

void printDependenceDirection(unsigned Dir) {
    errs() << "Direzione dipendenza: ";

    bool Printed = false;

    if (Dir & Dependence::DVEntry::LT) {
        errs() << "LT ";
        Printed = true;
    }

    if (Dir & Dependence::DVEntry::EQ) {
        errs() << "EQ ";
        Printed = true;
    }

    if (Dir & Dependence::DVEntry::GT) {
        errs() << "GT ";
        Printed = true;
    }

    if (!Printed)
        errs() << "unknown";

    errs() << "\n";
}

// Prova a trovare una IV del tipo: i = phi [0, preheader], [i + 1, latch].
PHINode *findSimpleIV(Loop *L) {
    if (PHINode *IV = L->getCanonicalInductionVariable())
        return IV;

    BasicBlock *Header = L->getHeader();
    BasicBlock *Preheader = L->getLoopPreheader();
    BasicBlock *Latch = L->getLoopLatch();

    if (!Header || !Preheader || !Latch)
        return nullptr;

    for (Instruction &I : *Header) {
        PHINode *Phi = dyn_cast<PHINode>(&I);
        if (!Phi)
            continue;

        if (!Phi->getType()->isIntegerTy())
            continue;

        Value *StartValue = Phi->getIncomingValueForBlock(Preheader);
        Value *LatchValue = Phi->getIncomingValueForBlock(Latch);

        ConstantInt *Start = dyn_cast<ConstantInt>(StartValue);
        if (!Start || !Start->isZero())
            continue;

        BinaryOperator *BO = dyn_cast<BinaryOperator>(LatchValue);
        if (!BO || BO->getOpcode() != Instruction::Add)
            continue;

        Value *Op0 = BO->getOperand(0);
        Value *Op1 = BO->getOperand(1);

        ConstantInt *C0 = dyn_cast<ConstantInt>(Op0);
        ConstantInt *C1 = dyn_cast<ConstantInt>(Op1);

        if (Op0 == Phi && C1 && C1->isOne())
            return Phi;

        if (Op1 == Phi && C0 && C0->isOne())
            return Phi;
    }

    return nullptr;
}

// Riconosce espressioni lineari semplici rispetto alla IV.
std::optional<LinearExpr> getLinearExprFromIV(Value *V, PHINode *IV) {
    if (!V || !IV)
        return std::nullopt;

    if (V == IV)
        return LinearExpr{1, 0};

    if (ConstantInt *C = dyn_cast<ConstantInt>(V))
        return LinearExpr{0, C->getSExtValue()};

    if (CastInst *Cast = dyn_cast<CastInst>(V))
        return getLinearExprFromIV(Cast->getOperand(0), IV);

    if (BinaryOperator *BO = dyn_cast<BinaryOperator>(V)) {
        Value *LHS = BO->getOperand(0);
        Value *RHS = BO->getOperand(1);

        auto L = getLinearExprFromIV(LHS, IV);
        auto R = getLinearExprFromIV(RHS, IV);

        if (!L || !R)
            return std::nullopt;

        if (BO->getOpcode() == Instruction::Add) {
            return LinearExpr{
                L->Coeff + R->Coeff,
                L->Const + R->Const
            };
        }

        if (BO->getOpcode() == Instruction::Sub) {
            return LinearExpr{
                L->Coeff - R->Coeff,
                L->Const - R->Const
            };
        }

        if (BO->getOpcode() == Instruction::Mul) {
            if (L->Coeff == 0) {
                return LinearExpr{
                    R->Coeff * L->Const,
                    R->Const * L->Const
                };
            }

            if (R->Coeff == 0) {
                return LinearExpr{
                    L->Coeff * R->Const,
                    L->Const * R->Const
                };
            }
        }
    }

    return std::nullopt;
}

// Estrae base e offset da una load/store del tipo A[i + c].
std::optional<SimpleAccess> getSimpleAccessInfo(Instruction *I, Loop *L) {
    PHINode *IV = findSimpleIV(L);
    if (!IV)
        return std::nullopt;

    Value *Ptr = nullptr;
    bool IsLoad = false;
    bool IsStore = false;

    if (LoadInst *LI = dyn_cast<LoadInst>(I)) {
        Ptr = LI->getPointerOperand();
        IsLoad = true;
    } else if (StoreInst *SI = dyn_cast<StoreInst>(I)) {
        Ptr = SI->getPointerOperand();
        IsStore = true;
    } else {
        return std::nullopt;
    }

    GEPOperator *GEP = dyn_cast<GEPOperator>(Ptr);
    if (!GEP)
        return std::nullopt;

    Value *Base = getUnderlyingObject(GEP->getPointerOperand())->stripPointerCasts();

    Value *Index = nullptr;

    for (auto Idx = GEP->idx_begin(); Idx != GEP->idx_end(); ++Idx)
        Index = Idx->get();

    if (!Index)
        return std::nullopt;

    auto Expr = getLinearExprFromIV(Index, IV);
    if (!Expr)
        return std::nullopt;

    // Gestiamo solo accessi A[i + c].
    if (Expr->Coeff != 1)
        return std::nullopt;

    SimpleAccess A;
    A.Inst = I;
    A.Base = Base;
    A.Offset = Expr->Const;
    A.IsLoad = IsLoad;
    A.IsStore = IsStore;

    return A;
}

// Classificazione semplice per i casi Levels == 0.
SimpleDepResult classifySimpleDependence(Instruction *I0,
                                          Instruction *I1,
                                          Loop *L0,
                                          Loop *L1) {
    auto A0 = getSimpleAccessInfo(I0, L0);
    auto A1 = getSimpleAccessInfo(I1, L1);

    if (!A0 || !A1)
        return SimpleDepResult::Unknown;

    // Se le basi sono due alloca diverse, le considero indipendenti.
    if (A0->Base != A1->Base) {
        if (isa<AllocaInst>(A0->Base) && isa<AllocaInst>(A1->Base))
            return SimpleDepResult::Safe;

        return SimpleDepResult::Unknown;
    }

    errs() << "Analisi semplice sugli indici:\n";
    errs() << "  I0: " << *I0 << "\n";
    errs() << "  I1: " << *I1 << "\n";
    errs() << "  Offset L0: " << A0->Offset << "\n";
    errs() << "  Offset L1: " << A1->Offset << "\n";

    // Caso: store A[i] nel loop 0 e load A[j + k] nel loop 1 con k > 0.
    if (A1->Offset > A0->Offset) {
        errs() << "Dipendenza negativa riconosciuta dagli offset\n";
        return SimpleDepResult::Negative;
    }

    return SimpleDepResult::Safe;
}

// Controllo dipendenze: LLVM DA + fallback semplice sugli indici.
DependenceResult checkDependences(Loop *L0,
                                  Loop *L1,
                                  DependenceInfo &DI) {
    SmallVector<Instruction *, 16> MemInsts0;
    SmallVector<Instruction *, 16> MemInsts1;

    collectMemoryInstructions(L0, MemInsts0);
    collectMemoryInstructions(L1, MemInsts1);

    errs() << "MemInsts in L0: " << MemInsts0.size() << "\n";
    errs() << "MemInsts in L1: " << MemInsts1.size() << "\n";

    bool FoundUnknownUnsafe = false;

    for (Instruction *I0 : MemInsts0) {
        for (Instruction *I1 : MemInsts1) {
            std::unique_ptr<Dependence> Dep = DI.depends(I0, I1, true);

            if (!Dep)
                continue;

            errs() << "Dipendenza trovata tra:\n";
            errs() << "I0: " << *I0 << "\n";
            errs() << "I1: " << *I1 << "\n";

            unsigned Levels = Dep->getLevels();

            if (Levels == 0) {
                errs() << "Dipendenza non classificabile da LLVM, provo analisi semplice\n";

                SimpleDepResult Simple = classifySimpleDependence(I0, I1, L0, L1);

                if (Simple == SimpleDepResult::Negative) {
                    errs() << "Dipendenza negativa/backward trovata\n";
                    return DependenceResult::Negative;
                }

                if (Simple == SimpleDepResult::Safe) {
                    errs() << "Dipendenza semplice classificata come sicura\n";
                    continue;
                }

                errs() << "Dipendenza ancora non classificabile\n";
                FoundUnknownUnsafe = true;
                continue;
            }

            for (unsigned Level = 1; Level <= Levels; ++Level) {
                unsigned Dir = Dep->getDirection(Level);

                errs() << "Direzione livello " << Level << ": ";

                bool HasKnownDirectionAtThisLevel = false;

                if (Dir & Dependence::DVEntry::LT) {
                    errs() << "LT ";
                    HasKnownDirectionAtThisLevel = true;
                }

                if (Dir & Dependence::DVEntry::EQ) {
                    errs() << "EQ ";
                    HasKnownDirectionAtThisLevel = true;
                }

                if (Dir & Dependence::DVEntry::GT) {
                    errs() << "GT ";
                    HasKnownDirectionAtThisLevel = true;
                }

                errs() << "\n";

                const SCEV *Dist = Dep->getDistance(Level);

                if (Dist)
                    errs() << "Distanza: " << *Dist << "\n";
                else
                    errs() << "Distanza non calcolabile\n";

                if (Dir & Dependence::DVEntry::GT) {
                    errs() << "Dipendenza negativa/backward trovata\n";
                    return DependenceResult::Negative;
                }

                if (!HasKnownDirectionAtThisLevel) {
                    errs() << "Direzione non chiara: salvo UnknownUnsafe\n";
                    FoundUnknownUnsafe = true;
                    continue;
                }
            }
        }
    }

    if (FoundUnknownUnsafe) {
        errs() << "Trovate dipendenze non classificabili, ma nessuna negativa\n";
        return DependenceResult::UnknownUnsafe;
    }

    errs() << "Nessuna dipendenza pericolosa trovata\n";
    return DependenceResult::Safe;
}

struct LoopFusionPass : public PassInfoMixin<LoopFusionPass> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        LoopInfo &LI = AM.getResult<LoopAnalysis>(F);

        DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
        PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
        ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
        DependenceInfo &DI = AM.getResult<DependenceAnalysis>(F);

        errs() << "\nFunction: " << F.getName() << "\n";

        std::vector<Loop *> TopLevelLoops = LI.getTopLevelLoops();

        if (TopLevelLoops.size() < 2) {
            errs() << "Meno di due top-level loop: niente da confrontare\n";
            return PreservedAnalyses::all();
        }

        // Ordino i top-level loop in base alla posizione dell'header nella funzione.
        DenseMap<BasicBlock *, unsigned> BlockOrder;
        unsigned Order = 0;

        for (BasicBlock &BB : F)
            BlockOrder[&BB] = Order++;

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

            // 1. Adiacenza
            if (areAdjacent(L0, L1, LI)) {
                errs() << "Loop " << i << " e loop " << i + 1
                       << " sono ADIACENTI\n";
            } else {
                errs() << "Loop " << i << " e loop " << i + 1
                       << " NON sono adiacenti\n";
                continue;
            }

            // 2. Trip count
            if (!haveSameTripCount(L0, L1, SE)) {
                errs() << "Loop " << i << " e loop " << i + 1
                       << " NON hanno lo stesso trip count\n";
                continue;
            }

            errs() << "Loop " << i << " e loop " << i + 1
                   << " hanno lo stesso trip count\n";

            // 3. Control-flow equivalence
            if (!areControlFlowEquivalent(L0, L1, DT, PDT)) {
                errs() << "Loop " << i << " e loop " << i + 1
                       << " NON sono control-flow equivalent\n";
                continue;
            }

            errs() << "Loop " << i << " e loop " << i + 1
                   << " sono control-flow equivalent\n";

            // 4. Dependence analysis
            DependenceResult DepRes = checkDependences(L0, L1, DI);

            if (DepRes == DependenceResult::Negative) {
                errs() << "Loop NON fondibili: dipendenza negativa/backward\n";
                continue;
            }

            if (DepRes == DependenceResult::UnknownUnsafe) {
                errs() << "Loop NON fondibili: dipendenza non classificabile\n";
                continue;
            }

            // Tutte le condizioni sono passate.
            errs() << "Loop fondibili: posso procedere alla trasformazione\n";
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