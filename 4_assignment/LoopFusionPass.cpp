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
#include "llvm/Analysis/AliasAnalysis.h"

#include <functional>

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

// Indice associato a una specifica profondità del loop nest.
struct MultiIndexExpr {
    bool Valid = false;

    const SCEV *Start = nullptr;
    int64_t Step = 0;
    int64_t Offset = 0;
};

// Accesso memoria con offset distinti per ogni livello del nest.
struct MultiAccess {
    Instruction *Inst = nullptr;
    Value *Base = nullptr;

    SmallVector<MultiIndexExpr, 4> ByDepth;

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

// Verifica se un loop ha la forma minima richiesta per la fusion.
bool isEligibleLoop(Loop *L) {
    if (!L)
        return false;

    if (!L->getHeader())
        return false;

    if (!L->getLoopPreheader())
        return false;

    if (!L->getLoopLatch())
        return false;

    if (!L->getExitingBlock())
        return false;

    if (!L->getExitBlock())
        return false;

    for (BasicBlock *BB : L->blocks()) {
        for (Instruction &I : *BB) {
            // controllo se l'istruzione può lanciare delle istruzioni
            if (I.mayThrow())
                return false;

            if (LoadInst *LI = dyn_cast<LoadInst>(&I)) {
                // se trova una load volatile, il loop non è sicuro
                // load volatile -> lettura che il compilatore non può eliminare, spostare o riordinare liberamente, 
                // perchè ha un effetto osservabile
                if (LI->isVolatile())
                    return false;
            }

            if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
                // se trova una store volatile il loop non è sicuro
                // store volatile -> non può essere riordinata liberamente.
                if (SI->isVolatile())
                    return false;
            }
        }
    }

    return true;
}

// Hook di profittabilità: per ora abilita la maximal fusion.
bool isProfitableToFuse(Loop *L0, Loop *L1) {
    return true;
}

// Due loop sono candidati insieme solo se sono fratelli.
bool haveSameParentLoop(Loop *L0, Loop *L1) {
    return L0->getParentLoop() == L1->getParentLoop();
}

// Raccoglie gruppi di loop fratelli:
// - prima i loop top-level della funzione;
// - poi, ricorsivamente, i sotto-loop che hanno lo stesso parent.
void collectSiblingLoopGroups(LoopInfo &LI,
                              SmallVectorImpl<SmallVector<Loop *, 8>> &Groups) {
    SmallVector<Loop *, 8> TopLevel;

    // Raccoglie tutti i loop esterni, cioè quelli non contenuti in altri loop.
    for (Loop *L : LI)
        TopLevel.push_back(L);

    // Se ci sono almeno due loop top-level, possono essere confrontati tra loro.
    if (TopLevel.size() >= 2)
        Groups.push_back(TopLevel);

    // Visita ricorsivamente la gerarchia dei loop.
    std::function<void(Loop *)> Visit = [&](Loop *Parent) {
        SmallVector<Loop *, 8> Children;

        // Raccoglie i sotto-loop diretti del loop corrente.
        for (Loop *SubL : Parent->getSubLoops())
            Children.push_back(SubL);

        // Se il parent ha almeno due figli, quei figli formano un gruppo di fratelli.
        if (Children.size() >= 2)
            Groups.push_back(Children);

        // Continua la visita anche dentro ogni sotto-loop.
        for (Loop *SubL : Parent->getSubLoops())
            Visit(SubL);
    };

    // Avvia la visita ricorsiva da ogni loop top-level.
    for (Loop *L : LI)
        Visit(L);
}

// Versione senza stampe della control-flow equivalence.
bool areControlFlowEquivalentQuiet(Loop *L0,
                                   Loop *L1,
                                   DominatorTree &DT,
                                   PostDominatorTree &PDT) {
    BasicBlock *H0 = L0->getHeader();
    BasicBlock *H1 = L1->getHeader();

    bool Forward = DT.dominates(H0, H1) && PDT.dominates(H1, H0);
    bool Backward = DT.dominates(H1, H0) && PDT.dominates(H0, H1);

    return Forward || Backward;
}

// Costruisce gruppi di loop control-flow equivalent.
// Ogni gruppo contiene loop che vengono eseguiti nello stesso contesto di controllo.
void buildCFESets(SmallVectorImpl<Loop *> &Candidates,
                  SmallVectorImpl<SmallVector<Loop *, 8>> &CFESets,
                  DominatorTree &DT,
                  PostDominatorTree &PDT,
                  DenseMap<BasicBlock *, unsigned> &BlockOrder) {
    
    // Analizza ogni loop candidato.
    for (Loop *L : Candidates) {
        bool Inserted = false;

        // Prova a inserire L in uno dei gruppi già esistenti.
        for (auto &Set : CFESets) {
            if (Set.empty())
                continue;

            // Basta confrontare L con il primo loop del gruppo:
            // se sono control-flow equivalent, L appartiene a quel gruppo.
            if (areControlFlowEquivalentQuiet(Set[0], L, DT, PDT)) {
                Set.push_back(L);
                Inserted = true;
                break;
            }
        }

        // Se L non appartiene a nessun gruppo esistente,
        // crea un nuovo gruppo contenente solo L.
        if (!Inserted) {
            SmallVector<Loop *, 8> NewSet;
            NewSet.push_back(L);
            CFESets.push_back(NewSet);
        }
    }

    // Ordina i loop dentro ogni gruppo secondo l'ordine del CFG.
    for (auto &Set : CFESets) {
        std::sort(Set.begin(), Set.end(),
                  [&](Loop *A, Loop *B) {
                      // Se A domina B, A viene prima.
                      if (DT.dominates(A->getHeader(), B->getHeader()))
                          return true;

                      // Se B domina A, B viene prima.
                      if (DT.dominates(B->getHeader(), A->getHeader()))
                          return false;

                      // Se non c'è dominanza diretta, usa l'ordine dei blocchi.
                      return BlockOrder[A->getHeader()] <
                             BlockOrder[B->getHeader()];
                  });
    }
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



// Costruisce la catena di loop da Root fino al loop che contiene I.
// Esempio: Root=L0, I dentro L2  => Chain = {L0, L1, L2}
bool getLoopChainForInstruction(Loop *Root,
                                Instruction *I,
                                LoopInfo &LI,
                                SmallVectorImpl<Loop *> &Chain) {
    // Trova il loop più interno che contiene il basic block dell'istruzione.
    Loop *Cur = LI.getLoopFor(I->getParent());

    SmallVector<Loop *, 4> ReverseChain;

    // Risale dai loop più interni verso quelli più esterni.
    while (Cur) {
        ReverseChain.push_back(Cur);

        // Se abbiamo raggiunto Root, la catena è valida.
        if (Cur == Root)
            break;

        Cur = Cur->getParentLoop();
    }

    // Se Root non è stato trovato, I non appartiene alla gerarchia di Root.
    if (ReverseChain.empty() || ReverseChain.back() != Root)
        return false;

    // Inverte l'ordine: da Root -> ... -> loop più interno.
    for (auto It = ReverseChain.rbegin(); It != ReverseChain.rend(); ++It)
        Chain.push_back(*It);

    return true;
}

// Raccoglie gli indici da tutta la catena di GEP.
// Serve per casi in cui un accesso A[i][j] viene spezzato in più GEP.
bool collectAllGEPIndices(Value *Ptr,
                          SmallVectorImpl<Value *> &Indices) {
    if (!Ptr)
        return false;

    // Serve normalizzare un puntatore
    Ptr = Ptr->stripPointerCasts();

    GEPOperator *GEP = dyn_cast<GEPOperator>(Ptr);
    if (!GEP)
        return false;

    /*
     * Se il puntatore base del GEP è a sua volta un GEP,
     * prima raccolgo gli indici più esterni.
     *
     * Esempio:
     *   %row = gep A, 0, i + 1
     *   %elt = gep %row, 0, j
     *
     * Risultato:
     *   0, i + 1, 0, j
     */
    collectAllGEPIndices(GEP->getPointerOperand(), Indices);

    for (auto Idx = GEP->idx_begin(); Idx != GEP->idx_end(); ++Idx)
        Indices.push_back(Idx->get());

    return true;
}

// Prova a riconoscere espressioni del tipo:
//   IV
//   IV + c
//   IV - c
//   c + IV
// Restituisce la costante c, oppure nullopt se il pattern non è riconosciuto.
std::optional<int64_t> getConstantOffsetByPattern(Value *V,
                                                  PHINode *IV) {
    // Controllo di sicurezza sugli argomenti.
    if (!V || !IV)
        return std::nullopt;

    // Elimina eventuali cast sul valore, se presenti.
    V = V->stripPointerCasts();

    // Caso base: il valore è proprio la induction variable.
    if (V == IV)
        return 0;

    /*
     * Una costante da sola non è della forma IV + c.
     * Esempio: 5 non dipende da IV.
     */
    if (isa<ConstantInt>(V))
        return std::nullopt;

    /*
     * Caso frequente:
     *   %idx64 = sext i32 %i to i64
     *
     * Ignora il cast e continua l'analisi sull'operando originale.
     */
    if (CastInst *Cast = dyn_cast<CastInst>(V))
        return getConstantOffsetByPattern(Cast->getOperand(0), IV);

    // Da qui in poi accettiamo solo operazioni binarie.
    BinaryOperator *BO = dyn_cast<BinaryOperator>(V);
    if (!BO)
        return std::nullopt;

    Value *LHS = BO->getOperand(0);
    Value *RHS = BO->getOperand(1);

    // Prova a riconoscere ricorsivamente IV + c nei due operandi.
    auto LOff = getConstantOffsetByPattern(LHS, IV);
    auto ROff = getConstantOffsetByPattern(RHS, IV);

    // Controlla se uno dei due operandi è una costante intera.
    ConstantInt *LC = dyn_cast<ConstantInt>(LHS);
    ConstantInt *RC = dyn_cast<ConstantInt>(RHS);

    // Caso: (IV + c1) + c2 oppure c2 + (IV + c1)
    if (BO->getOpcode() == Instruction::Add) {
        if (LOff && RC)
            return *LOff + RC->getSExtValue();

        if (ROff && LC)
            return *ROff + LC->getSExtValue();
    }

    // Caso: (IV + c1) - c2
    if (BO->getOpcode() == Instruction::Sub) {
        if (LOff && RC)
            return *LOff - RC->getSExtValue();
    }

    // Pattern non riconosciuto.
    return std::nullopt;
}

// Prova a riconoscere IndexValue = IV + c usando ScalarEvolution.
// Restituisce c, oppure nullopt se la differenza non è costante.
std::optional<int64_t> getConstantOffsetBySCEV(Value *IndexValue,
                                               PHINode *IV,
                                               ScalarEvolution &SE) {
    // Controllo di sicurezza sugli argomenti.
    if (!IndexValue || !IV)
        return std::nullopt;

    // Converte i valori LLVM in espressioni SCEV.
    const SCEV *IndexS = SE.getSCEV(IndexValue);
    const SCEV *IVS = SE.getSCEV(IV);

    Type *IndexTy = IndexS->getType();
    Type *IVTy = IVS->getType();

    // Se i tipi sono diversi, prova a portarli alla stessa larghezza.
    if (IndexTy != IVTy) {
        IntegerType *IndexIntTy = dyn_cast<IntegerType>(IndexTy);
        IntegerType *IVIntTy = dyn_cast<IntegerType>(IVTy);

        // Se non sono interi, non possiamo confrontarli in modo semplice.
        if (!IndexIntTy || !IVIntTy)
            return std::nullopt;

        // Sceglie il tipo intero più largo tra i due.
        unsigned Width = std::max(IndexIntTy->getBitWidth(),
                                  IVIntTy->getBitWidth());

        Type *WideTy = IntegerType::get(IndexValue->getContext(), Width);

        // Estende le due espressioni allo stesso tipo.
        IndexS = SE.getNoopOrSignExtend(IndexS, WideTy);
        IVS = SE.getNoopOrSignExtend(IVS, WideTy);
    }

    // Calcola: IndexValue - IV.
    const SCEV *Diff = SE.getMinusSCEV(IndexS, IVS);

    // Se la differenza è una costante, allora IndexValue = IV + costante.
    if (const SCEVConstant *C = dyn_cast<SCEVConstant>(Diff))
        return C->getAPInt().getSExtValue();

    // Altrimenti non è un offset costante riconoscibile.
    return std::nullopt;
}

// Prova prima ScalarEvolution, poi pattern matching manuale.
std::optional<int64_t> getConstantOffsetFromIV(Value *IndexValue,
                                               PHINode *IV,
                                               ScalarEvolution &SE) {
    if (auto Off = getConstantOffsetBySCEV(IndexValue, IV, SE))
        return Off;

    return getConstantOffsetByPattern(IndexValue, IV);
}

// Estrae informazioni da una load/store multidimensionale.
// Riconosce base dell'array, tipo di accesso e offset per ogni livello del nest.
std::optional<MultiAccess> getMultiAccessInfo(Instruction *I,
                                              Loop *Root,
                                              LoopInfo &LI,
                                              ScalarEvolution &SE) {
    Value *Ptr = nullptr;
    bool IsLoad = false;
    bool IsStore = false;

    // Accetta solo istruzioni load o store.
    if (LoadInst *Load = dyn_cast<LoadInst>(I)) {
        Ptr = Load->getPointerOperand();
        IsLoad = true;
    } else if (StoreInst *Store = dyn_cast<StoreInst>(I)) {
        Ptr = Store->getPointerOperand();
        IsStore = true;
    } else {
        return std::nullopt;
    }

    SmallVector<Value *, 8> GEPIndices;

    // Raccoglie tutti gli indici dei GEP, anche se ci sono GEP concatenati.
    if (!collectAllGEPIndices(Ptr, GEPIndices))
        return std::nullopt;

    SmallVector<Loop *, 4> Chain;

    // Costruisce la catena Root -> ... -> loop che contiene l'istruzione.
    if (!getLoopChainForInstruction(Root, I, LI, Chain))
        return std::nullopt;

    SmallVector<SimpleIVInfo, 4> IVInfos;

    // Per ogni loop della catena cerca una induction variable semplice.
    for (Loop *L : Chain) {
        auto IV = findSimpleIV(L, SE);

        if (!IV)
            return std::nullopt;

        IVInfos.push_back(*IV);
    }

    MultiAccess A;
    A.Inst = I;

    // Trova l'oggetto base dell'accesso, ad esempio A in A[i][j].
    A.Base = getUnderlyingObject(Ptr)->stripPointerCasts();

    A.IsLoad = IsLoad;
    A.IsStore = IsStore;

    // Prepara un'informazione per ogni livello di annidamento.
    A.ByDepth.resize(IVInfos.size());

    /*
     * Per ogni livello del loop nest cerca un indice GEP della forma:
     *
     *   IV_depth + costante
     *
     * Esempi:
     *   i       -> offset 0
     *   i + 1   -> offset 1
     *   j - 1   -> offset -1
     */
    for (unsigned Depth = 0; Depth < IVInfos.size(); ++Depth) {
        PHINode *IV = IVInfos[Depth].IV;
        bool FoundIndexForDepth = false;

        // Cerca tra tutti gli indici GEP quello legato alla IV corrente.
        for (Value *IndexValue : GEPIndices) {
            std::optional<int64_t> Offset =
                getConstantOffsetFromIV(IndexValue, IV, SE);

            if (!Offset)
                continue;

            // Salva start, step e offset per questo livello del nest.
            A.ByDepth[Depth].Valid = true;
            A.ByDepth[Depth].Start = IVInfos[Depth].Start;
            A.ByDepth[Depth].Step = IVInfos[Depth].Step;
            A.ByDepth[Depth].Offset = *Offset;

            FoundIndexForDepth = true;
            break;
        }

        // Se manca l'indice per un livello, l'accesso non è analizzabile.
        if (!FoundIndexForDepth)
            return std::nullopt;
    }

    return A;
}

// Confronta due indici invarianti tramite valore o SCEV.
bool sameInvariantIndex(Value *V0,
                        Value *V1,
                        ScalarEvolution &SE) {
    if (V0 == V1)
        return true;

    ConstantInt *C0 = dyn_cast<ConstantInt>(V0);
    ConstantInt *C1 = dyn_cast<ConstantInt>(V1);

    if (C0 && C1)
        return C0->getSExtValue() == C1->getSExtValue();

    const SCEV *S0 = SE.getSCEV(V0);
    const SCEV *S1 = SE.getSCEV(V1);

    if (S0 == S1)
        return true;

    return SE.isKnownPredicate(ICmpInst::ICMP_EQ, S0, S1);
}

// Classifica dipendenze tra accessi multidimensionali tipo A[i][j+c].
// Usa l'ordine delle depth del loop nest: prima loop esterno, poi interno.
SimpleDepResult classifyMultiDimDependence(Instruction *I0,
                                           Instruction *I1,
                                           Loop *L0,
                                           Loop *L1,
                                           LoopInfo &LI,
                                           ScalarEvolution &SE) {
    // Estrae base, tipo accesso e offset per ogni depth del loop nest.
    auto A0 = getMultiAccessInfo(I0, L0, LI, SE);
    auto A1 = getMultiAccessInfo(I1, L1, LI, SE);

    // Se uno dei due accessi non è analizzabile, non classifico.
    if (!A0 || !A1)
        return SimpleDepResult::Unknown;

    /*
     * Se le basi sono diverse:
     * - due alloca diverse sono indipendenti;
     * - puntatori diversi, invece, potrebbero aliasare.
     */
    if (A0->Base != A1->Base) {
        if (isa<AllocaInst>(A0->Base) && isa<AllocaInst>(A1->Base))
            return SimpleDepResult::Safe;

        return SimpleDepResult::Unknown;
    }

    // Gli accessi devono avere lo stesso numero di depth.
    if (A0->ByDepth.size() != A1->ByDepth.size())
        return SimpleDepResult::Unknown;

    errs() << "Analisi multidimensionale per depth del nest:\n";
    errs() << "  I0: " << *I0 << "\n";
    errs() << "  I1: " << *I1 << "\n";

    /*
     * Confronta gli indici in ordine lessicografico.
     * La prima depth con offset diverso decide la direzione.
     */
    for (unsigned Depth = 0; Depth < A0->ByDepth.size(); ++Depth) {
        const MultiIndexExpr &X = A0->ByDepth[Depth];
        const MultiIndexExpr &Y = A1->ByDepth[Depth];

        // Se manca qualche informazione, non classifico.
        if (!X.Valid || !Y.Valid)
            return SimpleDepResult::Unknown;

        // I due loop devono partire dallo stesso valore.
        bool SameStart =
            (X.Start == Y.Start) ||
            SE.isKnownPredicate(ICmpInst::ICMP_EQ, X.Start, Y.Start);

        if (!SameStart)
            return SimpleDepResult::Unknown;

        // Per ora gestisce solo step uguali.
        if (X.Step != Y.Step)
            return SimpleDepResult::Unknown;

        // Differenza tra offset del secondo accesso e del primo.
        int64_t Diff = Y.Offset - X.Offset;

        errs() << "  Depth " << Depth
               << " step=" << X.Step
               << " offset L0=" << X.Offset
               << " offset L1=" << Y.Offset
               << "\n";

        // Se gli offset sono uguali, passo alla depth successiva.
        if (Diff == 0)
            continue;

        /*
         * Loop crescente:
         * se L1 accede a un offset maggiore, dipende da una futura iterazione di L0.
         */
        if (X.Step == 1) {
            if (Diff > 0) {
                errs() << "Dipendenza negativa su depth "
                       << Depth << ", step +1\n";
                return SimpleDepResult::Negative;
            }

            return SimpleDepResult::Safe;
        }

        /*
         * Loop decrescente:
         * se L1 accede a un offset minore, dipende da una futura iterazione di L0.
         */
        if (X.Step == -1) {
            if (Diff < 0) {
                errs() << "Dipendenza negativa su depth "
                       << Depth << ", step -1\n";
                return SimpleDepResult::Negative;
            }

            return SimpleDepResult::Safe;
        }

        // Step diversi da +1 o -1 non vengono gestiti.
        return SimpleDepResult::Unknown;
    }

    // Tutti gli offset sono uguali: dipendenza a distanza zero.
    errs() << "Dipendenza a distanza zero multidimensionale: sicura\n";
    return SimpleDepResult::Safe;
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
                                          LoopInfo &LI,
                                          ScalarEvolution &SE) {
    SimpleDepResult Multi =
        classifyMultiDimDependence(I0, I1, L0, L1, LI, SE);

    if (Multi != SimpleDepResult::Unknown)
        return Multi;

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
                                  LoopInfo &LI,
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
                SimpleDepResult Simple = classifySimpleDependence(I0, I1, L0, L1, LI, SE);

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

                bool HasLT = Dir & Dependence::DVEntry::LT;
                bool HasEQ = Dir & Dependence::DVEntry::EQ;
                bool HasGT = Dir & Dependence::DVEntry::GT;

                errs() << "Direzione livello " << Level << ": ";

                if (HasLT)
                    errs() << "LT ";

                if (HasEQ)
                    errs() << "EQ ";

                if (HasGT)
                    errs() << "GT ";

                if (!HasLT && !HasEQ && !HasGT)
                    errs() << "unknown";

                errs() << "\n";

                const SCEV *Dist = Dep->getDistance(Level);

                if (Dist)
                    errs() << "Distanza: " << *Dist << "\n";
                else
                    errs() << "Distanza non calcolabile\n";

                /*
                * Caso chiaro:
                * solo GT significa dipendenza backward certa.
                */
                if (HasGT && !HasLT && !HasEQ) {
                    errs() << "Dipendenza negativa/backward trovata\n";
                    return DependenceResult::Negative;
                }

                /*
                * Caso ambiguo:
                * LLVM dice LT/EQ/GT, quindi non posso decidere solo dalla direction.
                * Provo il fallback basato sugli indici.
                */
                if (HasGT && (HasLT || HasEQ)) {
                    errs() << "Direzione ambigua con GT: provo fallback sugli indici\n";

                    SimpleDepResult Simple =
                        classifySimpleDependence(I0, I1, L0, L1, LI, SE);

                    if (Simple == SimpleDepResult::Negative) {
                        errs() << "Dipendenza negativa/backward trovata dal fallback\n";
                        return DependenceResult::Negative;
                    }

                    if (Simple == SimpleDepResult::Safe) {
                        errs() << "Dipendenza ambigua classificata come sicura dal fallback\n";
                        continue;
                    }

                    errs() << "Dipendenza ambigua non classificabile\n";
                    FoundUnknownUnsafe = true;
                    continue;
                }

                /*
                * LT o EQ senza GT non sono dipendenze negative per la fusion.
                */
                if (HasLT || HasEQ)
                    continue;

                errs() << "Direzione non chiara: salvo UnknownUnsafe\n";
                FoundUnknownUnsafe = true;
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

// Trova il blocco del body che va direttamente al latch del loop.
// Serve quando il loop ha più blocchi nel body, ad esempio dopo una prima fusion.
//ex: header -> body1 -< body2 -> latch -> header. La funzione vuole trovare body2, cioè il blocco immediatamente prima del latch
BasicBlock *getBlockBeforeLatch(Loop *L) {
    // Controllo di sicurezza.
    if (!L)
        return nullptr;

    BasicBlock *Header = L->getHeader();
    BasicBlock *Latch = L->getLoopLatch();

    // Se il loop non ha header o latch unico, non è gestibile.
    if (!Header || !Latch)
        return nullptr;

    BasicBlock *Candidate = nullptr;

    // Scorre tutti i blocchi appartenenti al loop.
    for (BasicBlock *BB : L->blocks()) {
        // Header e latch non sono blocchi del body da scegliere.
        if (BB == Header || BB == Latch)
            continue;

        // Considera solo branch incondizionati.
        BranchInst *BI = dyn_cast<BranchInst>(BB->getTerminator());
        if (!BI || !BI->isUnconditional())
            continue;

        // Cerca un blocco che salta direttamente al latch.
        if (BI->getSuccessor(0) == Latch) {
            // Se ce n'è più di uno, il caso è ambiguo.
            if (Candidate)
                return nullptr;

            Candidate = BB;
        }
    }

    // Ritorna il blocco trovato, oppure nullptr se non esiste.
    return Candidate;
}


// Fonde due loop semplici L0 e L1 modificando direttamente il CFG.
// Richiede stessa IV iniziale, stesso step e struttura CFG regolare.
bool fuseSimpleLoops(Loop *L0,
                     Loop *L1,
                     LoopInfo &LI,
                     ScalarEvolution &SE) {
    errs() << "Provo a fondere i loop...\n";

    // Recupera header dei due loop.
    BasicBlock *Header0 = L0->getHeader();
    BasicBlock *Header1 = L1->getHeader();

    // Recupera i body principali dei due loop.
    BasicBlock *Body0 = getSimpleLoopBody(L0);
    BasicBlock *Body1 = getSimpleLoopBody(L1);

    // Recupera l'ultimo blocco del body di L0 prima del latch.
    BasicBlock *Tail0 = getBlockBeforeLatch(L0);

    // Recupera i latch dei due loop.
    BasicBlock *Latch0 = L0->getLoopLatch();
    BasicBlock *Latch1 = L1->getLoopLatch();

    // Recupera i blocchi di uscita dei due loop.
    BasicBlock *Exit0 = L0->getExitBlock();
    BasicBlock *Exit1 = L1->getExitBlock();

    // Se manca qualche blocco necessario, il loop non è abbastanza semplice.
    if (!Header0 || !Header1 || !Body0 || !Body1 || !Tail0 ||
        !Latch0 || !Latch1 || !Exit0 || !Exit1) {
        errs() << "Fusion fallita: struttura del loop non semplice\n";
        return false;
    }

    // Cerca le induction variable semplici dei due loop.
    auto IVInfo0 = findSimpleIV(L0, SE);
    auto IVInfo1 = findSimpleIV(L1, SE);

    if (!IVInfo0 || !IVInfo1) {
        errs() << "Fusion fallita: IV semplice non trovata\n";
        return false;
    }

    // Per sostituire IV1 con IV0, gli step devono coincidere.
    if (IVInfo0->Step != IVInfo1->Step) {
        errs() << "Fusion fallita: step delle IV diverso\n";
        return false;
    }

    // Controlla che le due IV partano dallo stesso valore.
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

    // Sostituisce nel secondo loop gli usi di IV1 con IV0.
    replaceIVUsesInSecondLoopBody(L1, IV1, IV0);

    // Recupera i terminator dei blocchi che verranno modificati.
    BranchInst *Header0BI = dyn_cast<BranchInst>(Header0->getTerminator());
    BranchInst *Tail0BI = dyn_cast<BranchInst>(Tail0->getTerminator());
    BranchInst *Body1BI = dyn_cast<BranchInst>(Body1->getTerminator());

    // Header0 deve avere il branch condizionale del loop.
    if (!Header0BI || !Header0BI->isConditional()) {
        errs() << "Fusion fallita: header L0 non termina con branch condizionale\n";
        return false;
    }

    // Tail0 deve saltare direttamente al latch di L0.
    if (!Tail0BI || !Tail0BI->isUnconditional()) {
        errs() << "Fusion fallita: tail L0 non termina con branch incondizionale\n";
        return false;
    }

    // Body1 deve saltare direttamente al latch di L1.
    if (!Body1BI || !Body1BI->isUnconditional()) {
        errs() << "Fusion fallita: body L1 non termina con branch incondizionale\n";
        return false;
    }

    // Cambia l'uscita di L0: quando L0 termina, deve andare all'uscita finale di L1.
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

    // Collega la fine del body di L0 al body di L1.
    if (Tail0BI->getSuccessor(0) != Latch0) {
        errs() << "Fusion fallita: tail L0 non va direttamente al latch L0\n";
        return false;
    }

    Tail0BI->setSuccessor(0, Body1);

    // Collega la fine del body di L1 al latch di L0.
    if (Body1BI->getSuccessor(0) != Latch1) {
        errs() << "Fusion fallita: body L1 non va direttamente al latch L1\n";
        return false;
    }

    Body1BI->setSuccessor(0, Latch0);

    // Aggiorna le PHI nei blocchi che hanno cambiato predecessori.
    replacePhiIncomingBlock(Body1, Header1, Tail0);
    replacePhiIncomingBlock(Latch0, Tail0, Body1);
    replacePhiIncomingBlock(Exit1, Header1, Header0);

    errs() << "Fusion eseguita\n";
    return true;
}


// True per nest a due livelli: un loop esterno con un solo subloop.
bool isSimpleOneSubLoopNest(Loop *L) {
    if (!L)
        return false;

    if (L->getSubLoops().size() != 1)
        return false;

    if (!L->getLoopPreheader())
        return false;

    if (!L->getLoopLatch())
        return false;

    if (!L->getExitBlock())
        return false;

    Loop *Inner = L->getSubLoops()[0];

    if (!Inner->getLoopPreheader())
        return false;

    if (!Inner->getLoopLatch())
        return false;

    if (!Inner->getExitBlock())
        return false;

    return true;
}

// Sostituisce un valore dentro il body del loop, esclusi header/latch.
void replaceValueUsesInsideLoopBody(Loop *L,
                                    Value *OldV,
                                    Value *NewV) {
    BasicBlock *Header = L->getHeader();
    BasicBlock *Latch = L->getLoopLatch();

    for (BasicBlock *BB : L->blocks()) {
        if (BB == Header || BB == Latch)
            continue;

        for (Instruction &I : *BB) {
            for (Use &U : I.operands()) {
                if (U.get() == OldV)
                    U.set(NewV);
            }
        }
    }
}

// Fonde due loop nest semplici:
// ogni loop esterno deve contenere esattamente un loop interno.
bool fuseOneSubLoopNests(Loop *L0,
                         Loop *L1,
                         LoopInfo &LI,
                         ScalarEvolution &SE) {
    errs() << "Provo a fondere due loop nest semplici...\n";

    // Accetta solo nest del tipo: loop esterno con un solo subloop.
    if (!isSimpleOneSubLoopNest(L0) || !isSimpleOneSubLoopNest(L1)) {
        errs() << "Fusion nest fallita: struttura non supportata\n";
        return false;
    }

    // Recupera i loop interni dei due nest.
    Loop *Inner0 = L0->getSubLoops()[0];
    Loop *Inner1 = L1->getSubLoops()[0];

    // Recupera header, latch ed exit dei loop esterni.
    BasicBlock *Header0 = L0->getHeader();
    BasicBlock *Header1 = L1->getHeader();

    BasicBlock *Latch0 = L0->getLoopLatch();
    BasicBlock *Latch1 = L1->getLoopLatch();

    BasicBlock *Exit0 = L0->getExitBlock();
    BasicBlock *Exit1 = L1->getExitBlock();

    // Blocchi importanti dei loop interni.
    BasicBlock *InnerPreheader1 = Inner1->getLoopPreheader();
    BasicBlock *InnerExit0 = Inner0->getExitBlock();
    BasicBlock *InnerExit1 = Inner1->getExitBlock();

    // Se manca qualche blocco necessario, non trasformo.
    if (!Header0 || !Header1 || !Latch0 || !Latch1 ||
        !Exit0 || !Exit1 || !InnerPreheader1 ||
        !InnerExit0 || !InnerExit1) {
        errs() << "Fusion nest fallita: blocchi necessari mancanti\n";
        return false;
    }

    // Cerca le induction variable dei due loop esterni.
    auto IVInfo0 = findSimpleIV(L0, SE);
    auto IVInfo1 = findSimpleIV(L1, SE);

    if (!IVInfo0 || !IVInfo1) {
        errs() << "Fusion nest fallita: IV esterna non trovata\n";
        return false;
    }

    // Gli step delle IV esterne devono coincidere.
    if (IVInfo0->Step != IVInfo1->Step) {
        errs() << "Fusion nest fallita: step IV esterna diverso\n";
        return false;
    }

    // Le IV esterne devono partire dallo stesso valore.
    bool SameStart =
        (IVInfo0->Start == IVInfo1->Start) ||
        SE.isKnownPredicate(ICmpInst::ICMP_EQ,
                            IVInfo0->Start,
                            IVInfo1->Start);

    if (!SameStart) {
        errs() << "Fusion nest fallita: start IV esterna diverso\n";
        return false;
    }

    // Sostituisce gli usi della IV esterna di L1 con quella di L0.
    replaceValueUsesInsideLoopBody(L1, IVInfo1->IV, IVInfo0->IV);

    // Recupera i branch che verranno modificati.
    BranchInst *Header0BI = dyn_cast<BranchInst>(Header0->getTerminator());
    BranchInst *InnerExit0BI = dyn_cast<BranchInst>(InnerExit0->getTerminator());
    BranchInst *InnerExit1BI = dyn_cast<BranchInst>(InnerExit1->getTerminator());

    // Header0 deve essere il branch condizionale del loop esterno.
    if (!Header0BI || !Header0BI->isConditional()) {
        errs() << "Fusion nest fallita: header L0 non condizionale\n";
        return false;
    }

    // L'uscita del primo inner loop deve essere un salto semplice.
    if (!InnerExit0BI || !InnerExit0BI->isUnconditional()) {
        errs() << "Fusion nest fallita: exit inner0 non incondizionale\n";
        return false;
    }

    // Anche l'uscita del secondo inner loop deve essere un salto semplice.
    if (!InnerExit1BI || !InnerExit1BI->isUnconditional()) {
        errs() << "Fusion nest fallita: exit inner1 non incondizionale\n";
        return false;
    }

    // Cambia l'uscita del loop esterno L0 verso l'uscita finale di L1.
    bool ReplacedOuterExit = false;

    for (unsigned S = 0; S < Header0BI->getNumSuccessors(); ++S) {
        if (Header0BI->getSuccessor(S) == Exit0) {
            Header0BI->setSuccessor(S, Exit1);
            ReplacedOuterExit = true;
            break;
        }
    }

    if (!ReplacedOuterExit) {
        errs() << "Fusion nest fallita: non trovo Exit0 in Header0\n";
        return false;
    }

    // Dopo Inner0, invece di andare al latch esterno di L0,
    // si entra nel preheader del secondo inner loop.
    if (InnerExit0BI->getSuccessor(0) != Latch0) {
        errs() << "Fusion nest fallita: InnerExit0 non va al latch esterno L0\n";
        return false;
    }

    InnerExit0BI->setSuccessor(0, InnerPreheader1);

    // Dopo Inner1, si torna al latch esterno di L0.
    if (InnerExit1BI->getSuccessor(0) != Latch1) {
        errs() << "Fusion nest fallita: InnerExit1 non va al latch esterno L1\n";
        return false;
    }

    InnerExit1BI->setSuccessor(0, Latch0);

    // Aggiorna eventuali PHI nei blocchi con predecessori cambiati.
    replacePhiIncomingBlock(InnerPreheader1, Header1, InnerExit0);
    replacePhiIncomingBlock(Latch0, InnerExit0, InnerExit1);
    replacePhiIncomingBlock(Exit1, Header1, Header0);

    errs() << "Fusion nest eseguita\n";
    return true;
}

// Dispatcher: sceglie la trasformazione in base alla struttura dei loop.
bool fuseLoops(Loop *L0,
               Loop *L1,
               LoopInfo &LI,
               ScalarEvolution &SE) {
    if (L0->getSubLoops().empty() && L1->getSubLoops().empty())
        return fuseSimpleLoops(L0, L1, LI, SE);

    if (isSimpleOneSubLoopNest(L0) && isSimpleOneSubLoopNest(L1))
        return fuseOneSubLoopNests(L0, L1, LI, SE);

    errs() << "Fusion fallita: tipo di loop non supportato\n";
    return false;
}


struct LoopFusionPass : public PassInfoMixin<LoopFusionPass> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        LoopInfo &LI = AM.getResult<LoopAnalysis>(F);

        DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
        PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
        ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
        DependenceInfo &DI = AM.getResult<DependenceAnalysis>(F);

        errs() << "\nFunction: " << F.getName() << "\n";

        DenseMap<BasicBlock *, unsigned> BlockOrder;
        unsigned Order = 0;

        for (BasicBlock &BB : F)
            BlockOrder[&BB] = Order++;

        SmallVector<SmallVector<Loop *, 8>, 8> LoopGroups;
        collectSiblingLoopGroups(LI, LoopGroups);

        if (LoopGroups.empty()) {
            errs() << "Nessun gruppo di loop confrontabile\n";
            return PreservedAnalyses::all();
        }

        /*
         * Barton: si procede per nest level.
         * Prima top-level, poi subloop fratelli.
         */
        for (auto &Group : LoopGroups) {
            SmallVector<Loop *, 8> Candidates;

            for (Loop *L : Group) {
                if (!isEligibleLoop(L)) {
                    errs() << "Loop scartato: non eleggibile\n";
                    continue;
                }

                Candidates.push_back(L);
            }

            if (Candidates.size() < 2)
                continue;

            std::sort(Candidates.begin(), Candidates.end(),
                      [&](Loop *A, Loop *B) {
                          return BlockOrder[A->getHeader()] <
                                 BlockOrder[B->getHeader()];
                      });

            SmallVector<SmallVector<Loop *, 8>, 8> CFESets;
            buildCFESets(Candidates, CFESets, DT, PDT, BlockOrder);

            for (auto &Set : CFESets) {
                if (Set.size() < 2)
                    continue;

                for (size_t i = 0; i + 1 < Set.size(); ++i) {
                    Loop *L0 = Set[i];
                    Loop *L1 = Set[i + 1];

                    errs() << "\nAnalizzo coppia di loop " << i
                           << " e " << i + 1 << "\n";

                    if (!haveSameParentLoop(L0, L1)) {
                        errs() << "Loop NON fondibili: parent loop diverso\n";
                        continue;
                    }

                    if (!areControlFlowEquivalent(L0, L1, DT, PDT)) {
                        errs() << "Loop NON fondibili: non CFE\n";
                        continue;
                    }

                    if (!haveSameTripCount(L0, L1, SE)) {
                        errs() << "Loop NON fondibili: trip count diverso\n";
                        continue;
                    }

                    errs() << "Loop hanno lo stesso trip count\n";

                    if (!areAdjacent(L0, L1, LI)) {
                        errs() << "Loop NON fondibili: non adiacenti\n";
                        continue;
                    }

                    DependenceResult DepRes =
                        checkDependences(L0, L1, LI, DI, SE);

                    if (DepRes == DependenceResult::Negative) {
                        errs() << "Loop NON fondibili: dipendenza negativa/backward\n";
                        continue;
                    }

                    if (DepRes == DependenceResult::UnknownUnsafe) {
                        errs() << "Loop NON fondibili: dipendenza non classificabile\n";
                        continue;
                    }

                    if (!isProfitableToFuse(L0, L1)) {
                        errs() << "Loop NON fondibili: non profittevole\n";
                        continue;
                    }

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
                }
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