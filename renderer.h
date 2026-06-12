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
    void DrawFrame(GameState state, Player& player, GameMap& map, DialogueBox& dialogueBox, int score);

private:
    void DrawOverworld(Player& player, GameMap& map);
    void DrawBattle();
    void DrawGrid();
    void DrawMenu(const Player& player, int score);

    Texture2D bgTexture; 
};

#endif
