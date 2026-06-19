// config.h
#ifndef CONFIG_H
#define CONFIG_H
#include "raylib.h"
#include <string>

#define TILE_SIZE 32
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 576

struct Item {
    int id;           // 0 = Empty Slot. Anything else is a real item.
    std::string name; // e.g., "Health Potion", "Iron Key"
    std::string description;  // Item description
    int quantity;     // How many of this item
};

#define MAX_LEADERBOARD 10
struct ScoreEntry {
    std::string name;
    int score;
    int timeSeconds;
};

// Item ID Constants
#define ITEM_HEALTH_POTION 1
#define ITEM_STRENGTH_POTION 2
#define ITEM_DEFENSE_POTION 3
#define ITEM_PRISON_KEY 5
#define ITEM_OFFICE_KEY 6
#define ITEM_IRON_KEY 7
#define ITEM_MASTER_KEY 8
#define ITEM_DEBUG_KEY 505

// Loot table for enemies and chests
#define STRENGTH_POTION_DAMAGE_BONUS 10
#define STRENGTH_POTION_DURATION 2  // Turns
#define DEFENSE_POTION_HP_BONUS 50

enum GameState {
    STATE_MAIN_MENU,
    STATE_NAME_INPUT,
    STATE_LEADERBOARD,
    STATE_OVERWORLD,
    STATE_BATTLE,
    STATE_MENU,
    STATE_DIALOGUE,
    STATE_VICTORY
};

static const Color BgColor = {30, 30, 46, 255};     
static const Color PlayerColor = {203, 166, 247, 255}; 
static const Color EnemyColor = {243, 139, 168, 255};  
static const Color MenuPanelColor = {24, 24, 37, 220}; // NEW: 220 makes it slightly see-through!

#endif
