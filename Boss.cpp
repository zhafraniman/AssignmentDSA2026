#include "Boss.h"
#include <cstdlib>


//  QUEUE  (circular array) — FIFO attack plan

MoveQueue::MoveQueue() { Clear(); }

void MoveQueue::Clear() {
    frontIdx = 0;
    backIdx  = 0;
    count    = 0;
}

bool MoveQueue::IsEmpty() const { return count == 0; }
bool MoveQueue::IsFull()  const { return count == BOSS_QUEUE_CAP; }
int  MoveQueue::Size()    const { return count; }

void MoveQueue::Enqueue(const BossMove& m) {
    if (IsFull()) return;                 // drop if full (plans never get this long)
    data[backIdx] = m;
    backIdx = (backIdx + 1) % BOSS_QUEUE_CAP;  // wrap around — circular buffer
    count++;
}

BossMove MoveQueue::Dequeue() {
    BossMove out = data[frontIdx];        // caller checks IsEmpty() first
    if (count > 0) {
        frontIdx = (frontIdx + 1) % BOSS_QUEUE_CAP;
        count--;
    }
    return out;
}

// Look ahead at the i-th move still waiting in line (0 = the next one out).
BossMove MoveQueue::PeekAt(int i) const {
    if (i < 0 || i >= count) {
        BossMove empty = { MOVE_SLASH, "", 0 };
        return empty;
    }
    int idx = (frontIdx + i) % BOSS_QUEUE_CAP;
    return data[idx];
}


//  STACK  (array) — LIFO rage charges

RageStack::RageStack() { Clear(); }

void RageStack::Clear()      { topIdx = -1; }
bool RageStack::IsEmpty() const { return topIdx < 0; }
bool RageStack::IsFull()  const { return topIdx >= BOSS_STACK_CAP - 1; }
int  RageStack::Size()    const { return topIdx + 1; }

void RageStack::Push(int chargeDamage) {
    if (IsFull()) return;
    data[++topIdx] = chargeDamage;
}

int RageStack::Pop() {
    if (IsEmpty()) return 0;
    return data[topIdx--];
}

int RageStack::Peek() const {
    if (IsEmpty()) return 0;
    return data[topIdx];
}


//  TREE  (binary decision tree) — the boss brain

DecisionNode* Boss::MakeLeaf(BossMoveType action) {
    DecisionNode* n   = new DecisionNode();
    n->isLeaf         = true;
    n->action         = action;
    n->yes            = nullptr;
    n->no             = nullptr;
    n->conditionType  = -1;
    return n;
}

DecisionNode* Boss::MakeNode(int condition, DecisionNode* yes, DecisionNode* no) {
    DecisionNode* n   = new DecisionNode();
    n->isLeaf         = false;
    n->conditionType  = condition;
    n->yes            = yes;
    n->no             = no;
    n->action         = MOVE_SLASH;
    return n;
}

void Boss::FreeTree(DecisionNode* node) {
    if (node == nullptr) return;
    FreeTree(node->yes);     // post-order traversal so children die before parent
    FreeTree(node->no);
    delete node;
}

// Build the decision tree once. Shape (Y = yes-branch, N = no-branch):
//
//   [boss HP < 40% ?]
//     Y: [3+ charges ?]
//          Y: UNLEASH
//          N: CHARGE
//     N: [player defending ?]
//          Y: GUARD_BREAK
//          N: [player HP < 30% ?]
//               Y: HEAVY
//               N: SLASH
//
// Behaviour that emerges from this tree:
//   - When healthy, it punishes guarding with GUARD_BREAK, finishes low
//     players with HEAVY, and otherwise pokes with SLASH.
//   - When hurt, it gambles: it CHARGEs (taking no swing) to stack up rage,
//     then UNLEASHes a huge combo once it has banked 3 charges.

void Boss::BuildBrain() {
    DecisionNode* leafUnleash = MakeLeaf(MOVE_UNLEASH);
    DecisionNode* leafCharge  = MakeLeaf(MOVE_CHARGE);
    DecisionNode* leafGuard   = MakeLeaf(MOVE_GUARD_BREAK);
    DecisionNode* leafHeavy   = MakeLeaf(MOVE_HEAVY);
    DecisionNode* leafSlash   = MakeLeaf(MOVE_SLASH);

    DecisionNode* hurtBranch  = MakeNode(COND_HAS_3_CHARGES, leafUnleash, leafCharge);

    DecisionNode* lowPlayer   = MakeNode(COND_PLAYER_LOW, leafHeavy, leafSlash);
    DecisionNode* okBranch    = MakeNode(COND_PLAYER_DEFENDING, leafGuard, lowPlayer);

    root = MakeNode(COND_BOSS_HURT, hurtBranch, okBranch);
}

bool Boss::CheckCondition(int condition, int bossHpPct,
                          int playerHpPct, bool defending) const {
    switch (condition) {
        case COND_BOSS_HURT:        return bossHpPct < 40;
        case COND_HAS_3_CHARGES:    return rage.Size() >= 3;
        case COND_PLAYER_DEFENDING: return defending;
        case COND_PLAYER_LOW:       return playerHpPct < 30;
        default:                    return false;
    }
}

// Walk the tree from the root, answering each question, until a leaf
// (an action) is reached.
BossMoveType Boss::Decide(int bossHpPct, int playerHpPct, bool defending) const {
    DecisionNode* cur = root;
    while (cur != nullptr && !cur->isLeaf) {
        if (CheckCondition(cur->conditionType, bossHpPct, playerHpPct, defending))
            cur = cur->yes;
        else
            cur = cur->no;
    }
    return (cur != nullptr) ? cur->action : MOVE_SLASH;
}


//  MOVE / COMBO building

BossMove Boss::MakeMove(BossMoveType t) const {
    switch (t) {
        case MOVE_SLASH:       return { MOVE_SLASH,       "SLASH",  12 };
        case MOVE_HEAVY:       return { MOVE_HEAVY,       "HEAVY",  26 };
        case MOVE_GUARD_BREAK: return { MOVE_GUARD_BREAK, "BREAK",  18 };
        case MOVE_CHARGE:      return { MOVE_CHARGE,      "CHARGE",  0 };
        case MOVE_UNLEASH:     return { MOVE_UNLEASH,     "UNLEASH", 0 };
    }
    return { MOVE_SLASH, "SLASH", 12 };
}

// Turn a single decision into a short combo that gets ENQUEUED.
// Multi-move combos are what make the queue visible: after the first
// move fires, the rest sit in the queue as a telegraph on the player's turn.
void Boss::PlanCombo(BossMoveType decided) {
    switch (decided) {
        case MOVE_SLASH:                       // quick flurry
            plan.Enqueue(MakeMove(MOVE_SLASH));
            plan.Enqueue(MakeMove(MOVE_SLASH));
            break;
        case MOVE_HEAVY:                       // wind up, then the big swing
            plan.Enqueue(MakeMove(MOVE_SLASH));
            plan.Enqueue(MakeMove(MOVE_HEAVY));
            break;
        case MOVE_GUARD_BREAK:                 // immediate punish for guarding
            plan.Enqueue(MakeMove(MOVE_GUARD_BREAK));
            break;
        case MOVE_CHARGE:                      // bank two charges back to back
            plan.Enqueue(MakeMove(MOVE_CHARGE));
            plan.Enqueue(MakeMove(MOVE_CHARGE));
            break;
        case MOVE_UNLEASH:                     // release everything
            plan.Enqueue(MakeMove(MOVE_UNLEASH));
            break;
    }
}


//  BOSS lifecycle

Boss::Boss() {
    name = "The Compiler";
    root = nullptr;
    BuildBrain();
}

Boss::~Boss() {
    FreeTree(root);
    root = nullptr;
}

void Boss::Reset() {
    plan.Clear();
    rage.Clear();
}

// One full enemy turn.
BossDecision Boss::TakeTurn(int bossHpPct, int playerHpPct, bool playerDefending) {
    // 1. THINK: if the plan queue ran dry, walk the decision TREE and
    //    enqueue a fresh combo.
    if (plan.IsEmpty()) {
        BossMoveType decided = Decide(bossHpPct, playerHpPct, playerDefending);
        PlanCombo(decided);
    }

    // 2. ACT: dequeue exactly one move and perform it.
    BossMove move = plan.Dequeue();

    BossDecision result;
    result.type          = move.type;
    result.damage        = 0;
    result.ignoresDefend = false;

    switch (move.type) {
        case MOVE_CHARGE: {
            // Push a charge onto the rage STACK. No damage this turn.
            int chargeDmg = 8 + (std::rand() % 6);   // 8..13
            rage.Push(chargeDmg);
            result.text = name + " channels rage... (charges: "
                          + std::to_string(rage.Size()) + ")";
            break;
        }

        case MOVE_UNLEASH: {
            // Pop EVERY charge off the STACK and add it up. Because a stack
            // is LIFO, the last charge banked comes out first — we build the
            // message in that pop order to show it.
            if (rage.IsEmpty()) {
                // Nothing banked — fall back to a normal slash.
                result.type   = MOVE_SLASH;
                result.damage = MakeMove(MOVE_SLASH).power;
                result.text   = name + " lashes out for "
                                + std::to_string(result.damage) + "!";
                break;
            }
            int total = 0;
            std::string breakdown = "";
            bool first = true;
            while (!rage.IsEmpty()) {
                int c = rage.Pop();          // LIFO: newest charge first
                total += c;
                if (!first) breakdown += " + ";
                breakdown += std::to_string(c);
                first = false;
            }
            total += total / 4;              // +25% combo bonus for unleashing
            result.damage = total;
            result.text   = name + " UNLEASHES the combo! ("
                            + breakdown + ") = "
                            + std::to_string(total) + " damage!";
            break;
        }

        case MOVE_GUARD_BREAK: {
            int dmg = move.power + (std::rand() % 5);
            result.damage        = dmg;
            result.ignoresDefend = true;     // pierces DEFEND
            result.text          = name + " shatters your guard for "
                                   + std::to_string(dmg) + "!";
            break;
        }

        case MOVE_HEAVY: {
            int dmg = move.power + (std::rand() % 6);
            result.damage = dmg;
            result.text   = name + " lands a HEAVY blow for "
                            + std::to_string(dmg) + "!";
            break;
        }

        case MOVE_SLASH:
        default: {
            int dmg = move.power + (std::rand() % 4);
            result.damage = dmg;
            result.text   = name + " slashes for "
                            + std::to_string(dmg) + "!";
            break;
        }
    }

    return result;
}
