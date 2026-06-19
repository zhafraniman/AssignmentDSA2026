#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"
#include "map.h"
#include "Inventory.h"
#include <string>

class Player {
private:
    Vector2 position;
    float speed;
    Vector2 size;     
    Texture2D sprite; 

    // PLAYERS DATA
    std::string name;
    int level = 1;
    int currentExp = 0;
    int expToNextLevel = 100;
    int score = 0;
    int hp;
    int maxHp;
    int attack = 10;
    Inventory inventory;

public:
    Player(float startX, float startY);
    ~Player();
    
    void Update(GameMap& map, Vector2 inputDirection);
    void Draw();
    Rectangle GetBounds() const;
    void Teleport(float newX, float newY);
    void Reset(float startX, float startY);

    // Leveling System
    void GainExperience(int amount);
    int GetCurrentExp() const { return currentExp; }
    int GetExpToNextLevel() const { return expToNextLevel; }

    // Getters
    std::string GetName() const { return name; }
    int GetLevel() const { return level; }
    int GetHP() const { return hp; }
    int GetMaxHP() const { return maxHp; }
    int GetScore() const { return score; }

    // Setters
    void SetName(const std::string& newName);
    void AddScore(int amount);

    // Inventory methods
    bool AddItem(Item newItem);
    Item GetInventoryItem(int index) const;
    int GetInventoryCount() const;
    bool RemoveItem(int itemID);
    bool UseItem(int itemID);
    
    // Item checks
    bool HasKey() const;
    int GetItemQuantity(int itemID) const;
};

#endif
