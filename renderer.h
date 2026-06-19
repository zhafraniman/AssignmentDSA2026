#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include "config.h"
#include "player.h"
#include "map.h"
#include "dialogue.h"

class Renderer {
public:
    Renderer();
    ~Renderer();
    void DrawFrame(GameState state, Player& player, GameMap& map, DialogueBox& dialogueBox,
                     int score, int menuSelection, const std::string& nameInput, ScoreEntry* leaderboard, int lbCount);
    void DrawVictoryScreen(int score, float playTimer);

private:
    void DrawOverworld(Player& player, GameMap& map);
    void DrawBattle();
    void DrawGrid();
    void DrawMenu(const Player& player, int score);
    void DrawMainMenu(int selection);
    void DrawNameInput(const std::string& currentName);
    void DrawLeaderboard(ScoreEntry* leaderboard, int count);

    Texture2D bgTexture;
    Texture2D titleSprite;
};

#endif
