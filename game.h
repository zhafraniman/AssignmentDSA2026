#pragma once
#include "raylib.h"
#include "config.h"
#include "player.h"
#include "renderer.h"
#include "map.h"
#include "dialogue.h"
#include "Battle.h"
#include "Items.h"

class Game {
public:
    Game();
    ~Game();
    
    void Run();

private:
    void ProcessInput(); // Button presses
    void Update();       // Game logic (moving, opening chests)
    void Draw();         // Rendering

    // Game State and Objects
    Renderer gameRenderer;
    GameMap worldMap;
    Player myPlayer;
    DialogueBox dialogueBox;
    BattleSystem battle;
    GameState currentState;
    Vector2 playerInput;
    bool wasTouchingPortal;
    //values
    int fileScore;

    Enemy* currentEnemy;
};
