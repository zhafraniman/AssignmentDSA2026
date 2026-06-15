#ifndef BATTLE_H
#define BATTLE_H
 
#include "raylib.h"
#include "player.h"
#include "Items.h"
#include <string>
 
enum BattleState {
    PLAYER_TURN,
    ENEMY_TURN,
    PLAYER_WIN,
    PLAYER_LOSE
};
 
// BUG FIX: Enum order now matches the on-screen display order.
// Old enum had ATTACK=0, ITEMS=1, DEFEND=2, but Draw() showed
// "ATTACK","DEFEND","POTION" — meaning selectedOption=1 highlighted
// DEFEND visually but triggered ITEMS logic, and vice versa.
enum MenuOption { ATTACK = 0, DEFEND = 1, ITEMS = 2, MENU_COUNT = 3 };
 
class BattleSystem {
private:
    std::string enemyname;
    int enemyHp;
    int enemyMaxHp;
    int enemyAttack;
    int PlayerHP;
    int PlayerMaxHP;
    int PlayerAttack;
 
    // Active timed potion effects
    PotionEffect playerStrengthEffect;
    PotionEffect playerDefenseEffect;
 
    BattleState currentState;
    std::string battleMessage;
 
    bool playerDefending;
    int selectedOption;
 
    // Item sub-menu state (opened when player picks ITEMS)
    bool itemMenuOpen;
    int itemSubMenuOption;
    static const int ITEM_SUBMENU_COUNT = 3;
 
    float turnTimer;
    float turnDelay;
 
    // Internal helpers
    void HandleItemSubMenu(Player& player);
 
public:
    BattleSystem();
    void ResetPlayerStats();
 
    // Accessors used by Game for health reset on PLAYER_LOSE
    int& get_healing() { return PlayerHP; }
    int  max_HP()      { return PlayerMaxHP; }
 
    void StartBattle();
    void Update(Player& player);
    void Enemy_Damage(Player& player);
    void Player_Damage(Player& player);
    void Draw(const Player& player);
    void Menu_Option(Player& player);
 
    // Lets Game inject a loot message onto the win screen
    void SetBattleMessage(const std::string& msg) { battleMessage = msg; }
 
    bool IsBattleOver() const;
    BattleState GetState() const;
 
    // Potion application — each checks player inventory, uses 1 charge, applies effect
    void ApplyHealthPotion(Player& player);
    void ApplyStrengthPotion(Player& player);
    void ApplyDefensePotion(Player& player);
 
    void UpdatePotionEffects();
    int  GetActualPlayerDamage() const;
    int  GetActualPlayerMaxHP() const;
};
 
#endif
 
