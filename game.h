#pragma once
#include "raylib.h"
#include "config.h"
#include "player.h"
#include "renderer.h"
#include "map.h"
#include "dialogue.h"
#include "Battle.h"
#include "Items.h"
#include "audio.h"

class Game {
public:
    Game();
    ~Game();
    
    void Run();

private:
    void ProcessInput(); // Button presses
    void Update();       // Game logic (moving, opening chests)
    void Draw();         // Rendering

    void EnterBattle();
    void ExitBattle();

    // Values
    int fileScore;
    float playTimer;

    // Game State and Objects
    Renderer gameRenderer;
    GameMap worldMap;
    AudioManager audio;
    Player myPlayer;
    DialogueBox dialogueBox;
    BattleSystem battle;
    GameState currentState;
    Vector2 playerInput;
    bool wasTouchingPortal;
    Enemy* currentEnemy;
    bool gameBeat;

    // Main Menu & Leaderboard
    int mainMenuSelection;
    std::string playerNameInput;
    
    ScoreEntry leaderboard[MAX_LEADERBOARD];
    int leaderboardCount;

    // Leaderboard Functions
    void SaveToLeaderboard();
    void LoadLeaderboard();
    void SortLeaderboard();

};
