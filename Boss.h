#ifndef BOSS_H
#define BOSS_H

#include <string>

// ============================================================
// Boss.h  —  "The Compiler", final boss AI
//
// This boss attacks USING three hand-built data structures
// (assignment rule: no STL containers, every DSA made manually):
//
//   QUEUE  -> the attack PLAN. Moves the boss intends to perform
//             are enqueued, then executed one per turn (FIFO).
//             The still-queued moves are shown to the player as a
//             telegraph so they can react (DEFEND, heal, etc).
//
//   STACK  -> the RAGE / charge combo. Each CHARGE move pushes a
//             charge; an UNLEASH move pops them ALL back off and
//             releases the damage. Because a stack is LIFO, the
//             charge added last is released first.
//
//   TREE   -> the DECISION TREE. A small binary tree of yes/no
//             questions about the battle state. Each enemy turn the
//             boss walks root -> leaf to pick what to do next.
// ============================================================

// What a single boss move does.
enum BossMoveType {
    MOVE_SLASH,        // light hit
    MOVE_HEAVY,        // big hit (telegraphed a turn early)
    MOVE_GUARD_BREAK,  // hit that ignores the player's DEFEND
    MOVE_CHARGE,       // deal no damage, push 1 charge onto the rage STACK
    MOVE_UNLEASH       // pop EVERY charge off the STACK, release combined damage
};

// One element stored inside the attack QUEUE.
struct BossMove {
    BossMoveType type;
    std::string  label;  // short text shown in the telegraph e.g. "HEAVY"
    int          power;  // base damage (0 for CHARGE / UNLEASH)
};

// What the boss did on a given turn — handed back to the BattleSystem,
// which is responsible for applying DEFEND / potion mitigation.
struct BossDecision {
    BossMoveType type;
    std::string  text;     // message to show the player
    int          damage;   // raw damage aimed at the player (0 if none)
    bool         ignoresDefend;
};

// ------------------------------------------------------------
// QUEUE (circular array) — the boss attack plan (FIFO)
// ------------------------------------------------------------
#define BOSS_QUEUE_CAP 12
class MoveQueue {
private:
    BossMove data[BOSS_QUEUE_CAP];
    int frontIdx;
    int backIdx;
    int count;
public:
    MoveQueue();
    void     Clear();
    bool     IsEmpty() const;
    bool     IsFull() const;
    int      Size() const;
    void     Enqueue(const BossMove& m);
    BossMove Dequeue();
    BossMove PeekAt(int i) const;  // i-th upcoming move (0 = next)
};

// ------------------------------------------------------------
// STACK (array) — the boss rage charges (LIFO)
// ------------------------------------------------------------
#define BOSS_STACK_CAP 12
class RageStack {
private:
    int data[BOSS_STACK_CAP];  // each slot = damage stored in that charge
    int topIdx;                // -1 when empty
public:
    RageStack();
    void Clear();
    bool IsEmpty() const;
    bool IsFull() const;
    int  Size() const;
    void Push(int chargeDamage);
    int  Pop();   // returns 0 if empty
    int  Peek() const;
};

// ------------------------------------------------------------
// TREE (binary decision tree) — the boss brain
// ------------------------------------------------------------
struct DecisionNode {
    bool isLeaf;

    // Internal node: a yes/no question about the battle state.
    int conditionType;       // see BossCondition below
    DecisionNode* yes;       // followed when the answer is TRUE
    DecisionNode* no;        // followed when the answer is FALSE

    // Leaf node: the action the boss commits to.
    BossMoveType action;
};

// The yes/no questions the decision tree can ask.
enum BossCondition {
    COND_BOSS_HURT,        // boss HP below 40%?
    COND_HAS_3_CHARGES,    // rage stack has >= 3 charges?
    COND_PLAYER_DEFENDING, // is the player guarding this turn?
    COND_PLAYER_LOW        // player HP below 30%?
};

// ------------------------------------------------------------
// BOSS — owns and coordinates the three structures
// ------------------------------------------------------------
class Boss {
private:
    std::string   name;
    MoveQueue     plan;   // QUEUE
    RageStack     rage;   // STACK
    DecisionNode* root;   // TREE root

    // --- tree construction / teardown ---
    DecisionNode* MakeLeaf(BossMoveType action);
    DecisionNode* MakeNode(int condition, DecisionNode* yes, DecisionNode* no);
    void          FreeTree(DecisionNode* node);
    void          BuildBrain();

    // --- helpers ---
    bool         CheckCondition(int condition, int bossHpPct,
                                int playerHpPct, bool defending) const;
    BossMoveType Decide(int bossHpPct, int playerHpPct, bool defending) const;
    BossMove     MakeMove(BossMoveType t) const;
    void         PlanCombo(BossMoveType decided);

public:
    Boss();
    ~Boss();

    void Reset();   // clears queue + stack between battles (keeps the tree)

    // Runs one boss turn: refills the plan via the decision tree when the
    // queue is empty, then dequeues and performs exactly one move.
    BossDecision TakeTurn(int bossHpPct, int playerHpPct, bool playerDefending);

    // --- read-only accessors for the battle UI telegraph ---
    std::string GetName() const { return name; }
    int         QueuedCount() const { return plan.Size(); }
    std::string PeekLabel(int i) const { return plan.PeekAt(i).label; }
    int         ChargeCount() const { return rage.Size(); }
};

#endif
