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

#include "llvm/Transforms/Utils/BasicBlockUtils.h"

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

// Descrive la induction variable di un loop
struct SimpleIVInfo {
    PHINode *IV = nullptr;
    const SCEV *Start = nullptr;
    int64_t Step = 0;
};

// Descrive un semplice accesso in memoria
struct SimpleAccess {
    Instruction *Inst = nullptr;
    Value *Base = nullptr; //Oggetto base dell'accesso

    // Servono a rappresentare un indice del tipo Start + Offset, dove Start è la SCEV della IV e Offset è una costante.
    const SCEV *Start = nullptr;
    int64_t Step = 0;
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

// Serve per capire se un BB è vuoto dal punto di vista del programma, cioè se non contiene istruzione reali,
// ma solo un salto incodizionato verso un altro blocco
// funzione utile per controllare correttamente l'adiacenza
bool isEmptyUnconditionalBlock(BasicBlock *BB) {
    if (!BB)
        return false;

    // Controllo il terminatore del blocco
    // accetto solo -> br label %qualcosa
    BranchInst *BI = dyn_cast<BranchInst>(BB->getTerminator());
    if (!BI || !BI->isUnconditional())
        return false;

    for (Instruction &I : *BB) {
        // l'istruzione è il terminator (br) -> la ignoro
        if (&I == BB->getTerminator())
            continue;

        // è un'istruzione di debug -> la ignoro
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

// True se parto da un BB From ed arrivo a To passando solo da blocchi vuoti.
bool emptyPathToTarget(BasicBlock *From, BasicBlock *To) {
    if (!From || !To)
        return false;

    // Inizio del cammino
    BasicBlock *Cur = From;

    // Faccio al max 8 passi -> limite per evitare loop infiniti nel mio pass
    for (unsigned Steps = 0; Steps < 8; ++Steps) {
        // Controllo se il blocco è vuoto
        if (!isEmptyUnconditionalBlock(Cur))
            return false;

        // istruzioni per muovermi verso il blocco successivo
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

    // Guardi tutti i predecessori del preheader, cioè cerchi tutti i BB che arrivano al preheader.
    for (BasicBlock *Pred : predecessors(Preheader)) {
        // Un guard deve essere esterno al loop.
        if (L->contains(Pred))
            continue;

        // Evita di scambiare un blocco appartenente a un altro loop per guard.
        if (LI.getLoopFor(Pred) != nullptr)
            continue;

        // Prendi il terminator del predecessore, per essere guard deve terminare con un branch condizionale
        BranchInst *BI = dyn_cast<BranchInst>(Pred->getTerminator());
        if (!BI || !BI->isConditional())
            continue;

        // un branch  condizionato ha due successori.
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
/**
 * @brief Serve a trovare una semplice induction variable dentro un loop LLVM.
 * Riconosce una PHI del tipo:  i = phi [StartValue, Preheader], [i + 1, Latch]
 * Cioè una variabile che:
 *  - Prende il valore iniziale del preheader
 *  - Viene aggiornata nel latch
 *  - Cresce di 1 ad ogni iterazione
 * 
 * 
 * @param L  Loop di riferimento
 * @param SE valore della induction variable
 * @return std::optional<SimpleIVInfo> 
 */
std::optional<SimpleIVInfo> findSimpleIV(Loop *L, ScalarEvolution &SE) {
    BasicBlock *Header = L->getHeader();
    BasicBlock *Preheader = L->getLoopPreheader();
    BasicBlock *Latch = L->getLoopLatch();

    // Per riconoscere questa forma semplice ci servono:
    //   - header: dove sta la PHI della IV;
    //   - preheader: da dove arriva il valore iniziale;
    //   - latch: da dove arriva il valore aggiornato.
    if (!Header || !Preheader || !Latch)
        return std::nullopt;

    // Scorro tutte le istruzioni dell'header cercando una PHI candidata.
    for (Instruction &I : *Header) {
        PHINode *Phi = dyn_cast<PHINode>(&I);
        if (!Phi)
            continue;

        // La IV che cerchiamo deve essere intera.
        if (!Phi->getType()->isIntegerTy())
            continue;

        // Valore iniziale della IV, preso dal preheader.
        Value *StartValue = Phi->getIncomingValueForBlock(Preheader);

        // Valore aggiornato della IV, preso dal latch.
        Value *LatchValue = Phi->getIncomingValueForBlock(Latch);

        BinaryOperator *BO = dyn_cast<BinaryOperator>(LatchValue);
        if (!BO)
            continue;

        Value *Op0 = BO->getOperand(0);
        Value *Op1 = BO->getOperand(1);

        ConstantInt *C0 = dyn_cast<ConstantInt>(Op0);
        ConstantInt *C1 = dyn_cast<ConstantInt>(Op1);

        if (BO->getOpcode() == Instruction::Add) {
            // Caso: i + C
            if (Op0 == Phi && C1) {
                int64_t Step = C1->getSExtValue();

                if (Step == 1 || Step == -1)
                    return SimpleIVInfo{Phi, SE.getSCEV(StartValue), Step};
            }

            // Caso: C + i
            if (Op1 == Phi && C0) {
                int64_t Step = C0->getSExtValue();

                if (Step == 1 || Step == -1)
                    return SimpleIVInfo{Phi, SE.getSCEV(StartValue), Step};
            }
        }

        if (BO->getOpcode() == Instruction::Sub) {
            // Caso valido: i - C
            if (Op0 == Phi && C1) {
                int64_t Step = -C1->getSExtValue();

                if (Step == 1 || Step == -1)
                    return SimpleIVInfo{Phi, SE.getSCEV(StartValue), Step};
            }

            // Caso NON valido per una IV semplice:
            // C - i
            //
            // Non lo gestiamo perché non rappresenta un incremento/decremento
            // costante del tipo i = i +/- 1.
        }
    }

    return std::nullopt;
}

/**
 * @brief Riconosce espressioni lineari semplici rispetto alla IV.
 * L'obiettivo è trasformare un Value LLVM in una forma: Coeff * IV + Const
 * 
 * @param V 
 * @param IV 
 * @return std::optional<LinearExpr> 
 */
std::optional<LinearExpr> getLinearExprFromIV(Value *V, PHINode *IV) {
    // Se manca il valore o manca la IV, non posso analizzare nulla
    if (!V || !IV)
        return std::nullopt;

    // caso base -> il valore è proprio la induction variable
    // ex: i
    // Forma lineare: 1*i+0
    if (V == IV)
        return LinearExpr{1, 0};

    //Caso Base: il valore è una costante intera
    // ex: 5
    // Forma lineare: 0*i+5
    if (ConstantInt *C = dyn_cast<ConstantInt>(V))
        return LinearExpr{0, C->getSExtValue()};

    // Caso frequente in LLVM -> L'indice viene castato, per ex: %idx64 = sext i32 %i to i64
    // il cast non cambia il fatto che l'espressione dipende da i
    if (CastInst *Cast = dyn_cast<CastInst>(V))
        return getLinearExprFromIV(Cast->getOperand(0), IV);

    // Caso ricorsivo -> se V è un'operazione binaria, provo ad analizzare entrambi gli operandi.
    //ex: i+1, i-1, 2*i
    if (BinaryOperator *BO = dyn_cast<BinaryOperator>(V)) {
        Value *LHS = BO->getOperand(0);
        Value *RHS = BO->getOperand(1);

        // Analizzo ricorsivamente il lato sinistro e il lato destro dell'operazione binaria.
        auto L = getLinearExprFromIV(LHS, IV);
        auto R = getLinearExprFromIV(RHS, IV);

        // Se uno dei due lati non è lineare, allora l'intera espressione
        // non è classificabile da questa funzione.
        if (!L || !R)
            return std::nullopt;

        // Caso:
        //   (a*i + b) + (c*i + d)
        //
        // Risultato:
        //   (a+c)*i + (b+d)
        if (BO->getOpcode() == Instruction::Add) {
            return LinearExpr{
                L->Coeff + R->Coeff,
                L->Const + R->Const
            };
        }

        // Caso:
        //   (a*i + b) - (c*i + d)
        //
        // Risultato:
        //   (a-c)*i + (b-d)
        if (BO->getOpcode() == Instruction::Sub) {
            return LinearExpr{
                L->Coeff - R->Coeff,
                L->Const - R->Const
            };
        }

        // Caso moltiplicazione.
        //
        // Gestiamo solo moltiplicazioni in cui uno dei due lati
        // è una costante.
        //
        // Esempio valido:
        //   2 * i
        //
        // Esempio non gestito:
        //   i * i
        if (BO->getOpcode() == Instruction::Mul) {
            // Se il lato sinistro è costante, moltiplico tutta
            // l'espressione destra per quella costante.
            //
            // Esempio:
            //   2 * i
            //
            // L = 0*i + 2
            // R = 1*i + 0
            //
            // Risultato:
            //   2*i + 0
            if (L->Coeff == 0) {
                return LinearExpr{
                    R->Coeff * L->Const,
                    R->Const * L->Const
                };
            }
            // Caso simmetrico:
            //   i * 2
            if (R->Coeff == 0) {
                return LinearExpr{
                    L->Coeff * R->Const,
                    L->Const * R->Const
                };
            }
        }
    }

    // Se arrivo qui, l'espressione non è abbastanza semplice.
    return std::nullopt;
}
/**
 * @brief Estrae informazioni da una load/store su array.
 * L'obiettivo è riconoscere accessi semplici del tipo:
 * A[i]
 * A[i+1]
 * A[i-1]
 * A[j+3]
 * 
 * e trasformarli in una struttura SimpleAccess contenente:
 * - Inst: istruzione originale;
 * - Base: oggetto base dell'accesso, ad esempio A;
 * - Start: valore iniziale della induction variable;
 * - Step: passo della induction variable, ad esempio +1 o -1;
 * - Offset: costante rispetto alla IV, ad esempio 0, +1, -1;
 * - IsLoad / IsStore.
 * Se l'accesso non è abbastanza semplice, ritorna std::nullopt.
 * 
 * @param I 
 * @param L 
 * @param SE 
 * @return std::optional<SimpleAccess> 
 */
std::optional<SimpleAccess> getSimpleAccessInfo(Instruction *I,
                                                Loop *L,
                                                ScalarEvolution &SE) {
    
    // Prima troviamo la induction variable semplice del loop.
    std::optional<SimpleIVInfo> IVInfo = findSimpleIV(L, SE);
    if (!IVInfo)
        return std::nullopt;

    PHINode *IV = IVInfo->IV;

    Value *Ptr = nullptr;
    bool IsLoad = false;
    bool IsStore = false;

    // Controlliamo se l'istruzione è una load -> x = A[i]
    // In llvm sarà una load da un puntatore calcolato
    if (LoadInst *LI = dyn_cast<LoadInst>(I)) {
        Ptr = LI->getPointerOperand();
        IsLoad = true;
    //Altrimenti controlliamo se l'istruzione è una store -> A[i] = x
    // In llvm sarà una store verso un puntatore calcolato
    } else if (StoreInst *SI = dyn_cast<StoreInst>(I)) {
        Ptr = SI->getPointerOperand();
        IsStore = true;
    // se non è una load o una store, non sappiamo gestirla
    } else {
        return std::nullopt;
    }

    // Gli accessi ad array in LLVM vengono normalmente rappresentati tramite GEP (GetElementPtr)
    GEPOperator *GEP = dyn_cast<GEPOperator>(Ptr);
    if (!GEP)
        return std::nullopt;

    // Recuperiamo l'oggetto base dell'accesso -> A[i+1] -> Base = A
    // stripPointerCasts() -> rimuove eventuali cast sul puntatore.
    Value *Base = getUnderlyingObject(GEP->getPointerOperand())->stripPointerCasts();

    // Prendiamo l'ultimo indice del GEP.
    // Nei casi semplici è quello che rappresenta:
    //
    //   i
    //   i + 1
    //   j - 1
    Value *Index = nullptr;
    for (auto Idx = GEP->idx_begin(); Idx != GEP->idx_end(); ++Idx)
        Index = Idx->get();

    if (!Index)
        return std::nullopt;

    // Proviamo a scrivere l'indice nella forma:
    //
    //   Coeff * IV + Const
    //
    // Esempi:
    //
    //   i      -> Coeff = 1, Const = 0
    //   i + 1  -> Coeff = 1, Const = 1
    //   i - 1  -> Coeff = 1, Const = -1
    auto Expr = getLinearExprFromIV(Index, IV);
    if (!Expr)
        return std::nullopt;

     // Per ora gestiamo solo accessi del tipo:
    //
    //   A[i + c]
    //
    // cioè con coefficiente della IV uguale a 1.
    //
    // Non gestiamo:
    //
    //   A[2*i + c]
    //   A[i*i]
    //   A[f(i)]
    if (Expr->Coeff != 1)
        return std::nullopt;

    // Costruiamo la struttura con le informazioni estratte.
    SimpleAccess A;
    A.Inst = I;
    A.Base = Base;
    A.Start = IVInfo->Start;
    A.Step = IVInfo->Step;
    A.Offset = Expr->Const;
    A.IsLoad = IsLoad;
    A.IsStore = IsStore;

    return A;
}


/**
 * @brief Classificazione semplice per i casi Levels == 0.
 * Serve quando LLVM DependenceAnalysis trova una dipendenza, ma non riesce a darci direzione/distanza
 * 
 * @param I0 Istruzione del loop L0 coinvolta nella dipendenza
 * @param I1 Istruzione del loop L1 coinvolta nella dipendenza
 * @param L0 Loop L0
 * @param L1 Loop L1
 * @param SE ScalarEvolution
 * @return SimpleDepResult
 */
SimpleDepResult classifySimpleDependence(Instruction *I0,
                                          Instruction *I1,
                                          Loop *L0,
                                          Loop *L1,
                                          ScalarEvolution &SE) {
    auto A0 = getSimpleAccessInfo(I0, L0, SE);
    auto A1 = getSimpleAccessInfo(I1, L1, SE);

    if (!A0 || !A1)
        return SimpleDepResult::Unknown;

    /*
     * Se gli accessi sono su basi diverse:
     *
     * - se sono due alloca diverse, tipo int A[10] e int B[10],
     *   li considero indipendenti;
     *
     * - altrimenti, ad esempio due puntatori parametro, non posso
     *   dimostrare che non aliasino, quindi ritorno Unknown.
     */
    if (A0->Base != A1->Base) {
        if (isa<AllocaInst>(A0->Base) && isa<AllocaInst>(A1->Base))
            return SimpleDepResult::Safe;

        return SimpleDepResult::Unknown;
    }

    errs() << "Analisi semplice sugli indici:\n";
    errs() << "  I0: " << *I0 << "\n";
    errs() << "  I1: " << *I1 << "\n";
    errs() << "  Start L0: " << *A0->Start << "\n";
    errs() << "  Start L1: " << *A1->Start << "\n";
    errs() << "  Step L0: " << A0->Step << "\n";
    errs() << "  Step L1: " << A1->Step << "\n";
    errs() << "  Offset L0: " << A0->Offset << "\n";
    errs() << "  Offset L1: " << A1->Offset << "\n";

    // Gli Start devono essere uguali
    bool SameStart =
        (A0->Start == A1->Start) ||
        SE.isKnownPredicate(ICmpInst::ICMP_EQ, A0->Start, A1->Start);

    if (!SameStart) {
        errs() << "Start diversi o non dimostrabili uguali: dipendenza non classificabile\n";
        return SimpleDepResult::Unknown;
    }

    /*
     * Anche lo step deve essere uguale.
     *
     * Per esempio:
     *
     *   L0: i++
     *   L1: j++
     *
     * oppure:
     *
     *   L0: i--
     *   L1: j--
     *
     * Se uno cresce e l'altro decresce, questa analisi semplice
     * non è sufficiente.
     */
    if (A0->Step != A1->Step) {
        errs() << "Step diversi: dipendenza non classificabile\n";
        return SimpleDepResult::Unknown;
    }

    // Caso Loop Crescenti
    if (A0->Step == 1) {
        if (A1->Offset > A0->Offset) {
            errs() << "Dipendenza negativa riconosciuta dagli offset, step +1\n";
            return SimpleDepResult::Negative;
        }

        return SimpleDepResult::Safe;
    }

    // Caso Loop Decrescenti
    if (A0->Step == -1) {
        if (A1->Offset < A0->Offset) {
            errs() << "Dipendenza negativa riconosciuta dagli offset, step -1\n";
            return SimpleDepResult::Negative;
        }

        return SimpleDepResult::Safe;
    }

    return SimpleDepResult::Safe;
}

// Controlla le dipendenze tra L0 e L1.
// Usa prima LLVM DependenceAnalysis.
// Se LLVM trova una dipendenza ma non riesce a classificarla,
// prova il fallback sugli indici A[i+c].
DependenceResult checkDependences(Loop *L0,
                                  Loop *L1,
                                  DependenceInfo &DI,
                                  ScalarEvolution &SE) {
    SmallVector<Instruction *, 16> MemInsts0;
    SmallVector<Instruction *, 16> MemInsts1;

    // Raccolgo solo load/store dei due loop.
    collectMemoryInstructions(L0, MemInsts0);
    collectMemoryInstructions(L1, MemInsts1);

    errs() << "MemInsts in L0: " << MemInsts0.size() << "\n";
    errs() << "MemInsts in L1: " << MemInsts1.size() << "\n";

    // Mi ricordo se ho trovato dipendenze non classificabili.
    bool FoundUnknownUnsafe = false;

    // Confronto ogni istruzione memoria di L0 con ogni istruzione memoria di L1.
    for (Instruction *I0 : MemInsts0) {
        for (Instruction *I1 : MemInsts1) {
            std::unique_ptr<Dependence> Dep = DI.depends(I0, I1, true);

            // Nessuna dipendenza tra questa coppia.
            if (!Dep)
                continue;

            errs() << "Dipendenza trovata tra:\n";
            errs() << "I0: " << *I0 << "\n";
            errs() << "I1: " << *I1 << "\n";

            unsigned Levels = Dep->getLevels();

            // LLVM ha trovato una dipendenza, ma non sa dare LT/EQ/GT.
            if (Levels == 0) {
                errs() << "Dipendenza non classificabile da LLVM, provo analisi semplice\n";

                // Provo a riconoscerla tramite base array e offset degli indici.
                SimpleDepResult Simple = classifySimpleDependence(I0, I1, L0, L1, SE);

                if (Simple == SimpleDepResult::Negative) {
                    errs() << "Dipendenza negativa/backward trovata\n";
                    return DependenceResult::Negative;
                }

                if (Simple == SimpleDepResult::Safe) {
                    errs() << "Dipendenza semplice classificata come sicura\n";
                    continue;
                }

                // Non riesco a dimostrare che sia sicura.
                errs() << "Dipendenza ancora non classificabile\n";
                FoundUnknownUnsafe = true;
                continue;
            }

            // LLVM ha dato livelli di dipendenza: controllo le direzioni.
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

                // GT indica una dipendenza backward/negativa.
                if (Dir & Dependence::DVEntry::GT) {
                    errs() << "Dipendenza negativa/backward trovata\n";
                    return DependenceResult::Negative;
                }

                // Direzione assente/non interpretabile: blocco conservativamente.
                if (!HasKnownDirectionAtThisLevel) {
                    errs() << "Direzione non chiara: salvo UnknownUnsafe\n";
                    FoundUnknownUnsafe = true;
                    continue;
                }
            }
        }
    }

    // Ho trovato dipendenze, ma non sono riuscito a classificarle.
    if (FoundUnknownUnsafe) {
        errs() << "Trovate dipendenze non classificabili, ma nessuna negativa\n";
        return DependenceResult::UnknownUnsafe;
    }

    // Nessuna dipendenza pericolosa trovata.
    errs() << "Nessuna dipendenza pericolosa trovata\n";
    return DependenceResult::Safe;
}

// Ritorna il body principale del loop.
// Gestisce loop semplici del tipo:
//
//   header:
//     br cond, body, exit
//
// dove body è dentro il loop ed exit è fuori.
BasicBlock *getSimpleLoopBody(Loop *L) {
    BasicBlock *Header = L->getHeader();

    // Il loop deve avere un header.
    if (!Header)
        return nullptr;

    // Il terminator del header deve essere un branch condizionale.
    BranchInst *BI = dyn_cast<BranchInst>(Header->getTerminator());
    if (!BI || !BI->isConditional())
        return nullptr;

    BasicBlock *Succ0 = BI->getSuccessor(0);
    BasicBlock *Succ1 = BI->getSuccessor(1);

    bool Succ0InLoop = L->contains(Succ0);
    bool Succ1InLoop = L->contains(Succ1);

    // Uno dei due successori deve essere dentro il loop, l'altro fuori.
    if (Succ0InLoop && !Succ1InLoop)
        return Succ0;

    if (!Succ0InLoop && Succ1InLoop)
        return Succ1;

    return nullptr;
}

// Cambia il blocco predecessore dentro eventuali PHI.
// Serve quando riscriviamo il CFG.
void replacePhiIncomingBlock(BasicBlock *BB, BasicBlock *OldPred, BasicBlock *NewPred) {
    
    if(!BB || !OldPred || !NewPred)
        return;

    for (Instruction &I : *BB) {
        PHINode *PHI = dyn_cast<PHINode>(&I);
        if (!PHI)
            break;

        for(unsigned i = 0; i < PHI->getNumIncomingValues(); ++i) {
            if (PHI->getIncomingBlock(i) == OldPred) {
                PHI->setIncomingBlock(i, NewPred);
            }
        }
    }

}

// Sostituisce gli usi della IV di L1 nel body di L1 con la IV di L0.
// Non tocca header/latch di L1 perché dopo la fusione diventano inutili.
void replaceIVUsesInSecondLoopBody(Loop *L1,
                                   PHINode *IV1,
                                   PHINode *IV0) {
    BasicBlock *Header1 = L1->getHeader();
    BasicBlock *Latch1 = L1->getLoopLatch();

    for (BasicBlock *BB : L1->blocks()) {
        if (BB == Header1 || BB == Latch1)
            continue;

        for (Instruction &I : *BB) {
            for (Use &U : I.operands()) {
                if (U.get() == IV1) {
                    U.set(IV0);
                }
            }
        }
    }
}

bool fuseLoops(Loop *L0,
               Loop *L1,
               LoopInfo &LI,
               ScalarEvolution &SE) {
    errs() << "Provo a fondere i loop...\n";

    BasicBlock *Header0 = L0->getHeader();
    BasicBlock *Header1 = L1->getHeader();

    BasicBlock *Body0 = getSimpleLoopBody(L0);
    BasicBlock *Body1 = getSimpleLoopBody(L1);

    BasicBlock *Latch0 = L0->getLoopLatch();
    BasicBlock *Latch1 = L1->getLoopLatch();

    BasicBlock *Exit0 = L0->getExitBlock();
    BasicBlock *Exit1 = L1->getExitBlock();

    if (!Header0 || !Header1 || !Body0 || !Body1 ||
        !Latch0 || !Latch1 || !Exit0 || !Exit1) {
        errs() << "Fusion fallita: struttura del loop non semplice\n";
        return false;
    }

    /*
     * Per fare una sostituzione diretta IV1 -> IV0,
     * le IV devono avere stesso start e stesso step.
     *
     * Esempio supportato:
     *
     *   for (int i = N; i < M; i++)
     *   for (int j = N; j < M; j++)
     *
     * Esempio NON supportato direttamente:
     *
     *   for (int i = 0; i < 10; i++)
     *   for (int j = 5; j < 15; j++)
     *
     * In quel caso servirebbe sostituire j con i + 5, non solo con i.
     */
    auto IVInfo0 = findSimpleIV(L0, SE);
    auto IVInfo1 = findSimpleIV(L1, SE);

    if (!IVInfo0 || !IVInfo1) {
        errs() << "Fusion fallita: IV semplice non trovata\n";
        return false;
    }

    if (IVInfo0->Step != IVInfo1->Step) {
        errs() << "Fusion fallita: step delle IV diverso\n";
        return false;
    }

    bool SameStart =
        (IVInfo0->Start == IVInfo1->Start) ||
        SE.isKnownPredicate(ICmpInst::ICMP_EQ,
                            IVInfo0->Start,
                            IVInfo1->Start);

    if (!SameStart) {
        errs() << "Fusion fallita: start delle IV diverso o non dimostrabile uguale\n";
        return false;
    }

    PHINode *IV0 = IVInfo0->IV;
    PHINode *IV1 = IVInfo1->IV;

    /*
     * 1. Sostituisco gli usi della IV del secondo loop
     *    nel body del secondo loop.
     */
    replaceIVUsesInSecondLoopBody(L1, IV1, IV0);

    /*
     * 2. Modifico il CFG.
     *
     * Prima:
     *
     *   L0 header --false--> Exit0 / preheader L1
     *   L0 body   ---------> L0 latch
     *
     *   L1 header --true---> L1 body
     *   L1 body   ---------> L1 latch
     *   L1 header --false--> Exit1
     *
     * Dopo:
     *
     *   L0 header --false--> Exit1
     *   L0 body   ---------> L1 body
     *   L1 body   ---------> L0 latch
     *
     * In questo modo, a ogni iterazione di L0, eseguo:
     *
     *   body L0
     *   body L1
     *   latch L0
     */

    BranchInst *Header0BI = dyn_cast<BranchInst>(Header0->getTerminator());
    BranchInst *Body0BI = dyn_cast<BranchInst>(Body0->getTerminator());
    BranchInst *Body1BI = dyn_cast<BranchInst>(Body1->getTerminator());

    if (!Header0BI || !Header0BI->isConditional()) {
        errs() << "Fusion fallita: header L0 non termina con branch condizionale\n";
        return false;
    }

    if (!Body0BI || !Body0BI->isUnconditional()) {
        errs() << "Fusion fallita: body L0 non termina con branch incondizionale\n";
        return false;
    }

    if (!Body1BI || !Body1BI->isUnconditional()) {
        errs() << "Fusion fallita: body L1 non termina con branch incondizionale\n";
        return false;
    }

    /*
     * Header0: il ramo di uscita di L0 deve andare all'uscita finale di L1.
     */
    bool ReplacedExit = false;

    for (unsigned S = 0; S < Header0BI->getNumSuccessors(); ++S) {
        if (Header0BI->getSuccessor(S) == Exit0) {
            Header0BI->setSuccessor(S, Exit1);
            ReplacedExit = true;
            break;
        }
    }

    if (!ReplacedExit) {
        errs() << "Fusion fallita: non trovo Exit0 come successore di Header0\n";
        return false;
    }

    /*
     * Body0: invece di andare al latch di L0, va al body di L1.
     */
    if (Body0BI->getSuccessor(0) != Latch0) {
        errs() << "Fusion fallita: body L0 non va direttamente al latch L0\n";
        return false;
    }

    Body0BI->setSuccessor(0, Body1);

    /*
     * Body1: invece di andare al latch di L1, va al latch di L0.
     */
    if (Body1BI->getSuccessor(0) != Latch1) {
        errs() << "Fusion fallita: body L1 non va direttamente al latch L1\n";
        return false;
    }

    Body1BI->setSuccessor(0, Latch0);

    /*
     * Aggiorno eventuali PHI nei blocchi che hanno cambiato predecessore.
     * Nei tuoi test semplici probabilmente non servono, ma tenerle evita
     * alcuni IR invalidi in casi leggermente più complessi.
     */
    replacePhiIncomingBlock(Body1, Header1, Body0);
    replacePhiIncomingBlock(Latch0, Body0, Body1);
    replacePhiIncomingBlock(Exit1, Header1, Header0);

    errs() << "Fusion eseguita\n";
    return true;
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
            DependenceResult DepRes = checkDependences(L0, L1, DI, SE);

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

            errs() << "Loop fondibili: procedo con la trasformazione\n";

            if (fuseLoops(L0, L1, LI, SE)) {
                errs() << "Trasformazione completata\n";

                bool Removed = EliminateUnreachableBlocks(F);

                if (Removed)
                    errs() << "Cleanup: blocchi non raggiungibili eliminati\n";
                else
                    errs() << "Cleanup: nessun blocco non raggiungibile da eliminare\n";

                return PreservedAnalyses::none();
            }

            errs() << "Trasformazione non eseguita\n";
continue;
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