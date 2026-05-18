#include "raylib.h"
#include "config.h"
#include "player.h"
#include "renderer.h"
#include "map.h"

int main() {
    Renderer gameRenderer;   
    
    // Setup the starting world
    GameMap worldMap;        
    worldMap.LoadMap("src/levels/level1.txt"); 
    
    // Add a portal to level.txt at pixel (400, 500) that leads to the house
    // It will teleport the player to (400, 100) on the new map
    //worldMap.AddPortal({400, 500, 32, 32}, "src/levels/level2.txt", 400, 100);
    
    Player myPlayer(400.0f, 300.0f); 
    GameState currentState = STATE_OVERWORLD;

    while (!WindowShouldClose()) {
        switch (currentState) {
            case STATE_OVERWORLD: {
                myPlayer.Update(worldMap); 
                
                // --- NEW: PORTAL CHECKING LOGIC ---
                Portal hitPortal;
                if (worldMap.CheckPortals(myPlayer.GetBounds(), hitPortal)) {
                    
                    // 1. Load the new map (The file itself will automatically spawn the new doors!)
                    worldMap.LoadMap(hitPortal.targetMap);
                    
                    // 2. Teleport the player
                    myPlayer.Teleport(hitPortal.spawnX, hitPortal.spawnY);
                }
                
                if (IsKeyPressed(KEY_B)) currentState = STATE_BATTLE;
                if (IsKeyPressed(KEY_M)) currentState = STATE_MENU; 
                break;
            }

            case STATE_MENU: // No update function called so the game freezes
                if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE)) currentState = STATE_OVERWORLD;
                break;
                
            case STATE_BATTLE:
                if (IsKeyPressed(KEY_ESCAPE)) currentState = STATE_OVERWORLD;
                break;
            
        }

        gameRenderer.DrawFrame(currentState, myPlayer, worldMap);
    }
    return 0;
}