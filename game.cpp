#include "game.h"

// --- INITIALIZATION ---
Game::Game() : myPlayer(388.0f, 256.0f) {
    worldMap.LoadMap("src/levels/level1.txt"); 
    currentState = STATE_OVERWORLD;
}

// --- CLEANUP ---
Game::~Game() {
    // CloseWindow();
}

// --- MAIN ENGINE LOOP ---
void Game::Run() {
    while (!WindowShouldClose()) {
        ProcessInput();
        Update();
        Draw();
    }
}

// --- INPUT HANDLING ---
void Game::ProcessInput() {
    playerInput = { 0.0f, 0.0f }; // A Vector2 variable stored in your Game class
    
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) playerInput.y -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) playerInput.y += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) playerInput.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) playerInput.x += 1.0f;
}

// --- GAME LOGIC ---
void Game::Update() {
    switch (currentState) {
        case STATE_OVERWORLD: {
            myPlayer.Update(worldMap, playerInput);
            worldMap.UpdateEnemies(myPlayer.GetBounds());

            Enemy* touchedEnemy = worldMap.CheckEnemyCollision(myPlayer.GetBounds());
            if (touchedEnemy != nullptr) {
                currentEnemy = touchedEnemy; // Remember who we are fighting
                currentState = STATE_BATTLE;
            }

            // CHEST LOGIC
            if (IsKeyPressed(KEY_E)) {
                Chest* nearbyChest = worldMap.CheckChestInteraction(myPlayer.GetBounds());
                if (nearbyChest != nullptr && !nearbyChest->isOpen) {
                    if (myPlayer.AddItem(nearbyChest->content)) {
                        worldMap.MarkChestOpened(nearbyChest); 
                        nearbyChest->isOpen = true; 
                    }
                }
            }

            // SIGNPOST LOGIC
            if (IsKeyPressed(KEY_R)) {
                Signpost* nearbySign = worldMap.CheckSignpostInteraction(myPlayer.GetBounds());
                if (nearbySign != nullptr) {
                    dialogueBox.Start(); 
                    for (int i = 0; i < nearbySign->lineCount; i++) {
                        dialogueBox.Enqueue(nearbySign->dialogue[i]);
                    }
                    currentState = STATE_DIALOGUE;
                }
            }

            // PORTAL LOGIC
            Portal hitPortal;
            if (worldMap.CheckPortals(myPlayer.GetBounds(), hitPortal)) {
                worldMap.LoadMap(hitPortal.targetMap);
                myPlayer.Teleport(hitPortal.spawnX, hitPortal.spawnY);
            }
            
            if (IsKeyPressed(KEY_B)) currentState = STATE_BATTLE;
            if (IsKeyPressed(KEY_M)) currentState = STATE_MENU; 
            break;
        }
        case STATE_DIALOGUE: {
            dialogueBox.Update(); 
            if (IsKeyPressed(KEY_SPACE)) {
                if (dialogueBox.IsTextFinished()) {
                    dialogueBox.Dequeue(); 
                    if (!dialogueBox.IsActive()) currentState = STATE_OVERWORLD; 
                } else {
                    dialogueBox.SkipTyping(); 
                }
            }
            break;
        }
        case STATE_MENU:
            if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE)) currentState = STATE_OVERWORLD;
            break;
            
        case STATE_BATTLE:
            if (IsKeyPressed(KEY_K)) {
                worldMap.MarkEnemyDefeated(currentEnemy);
                currentEnemy = nullptr; // Clear the memory
                currentState = STATE_OVERWORLD;
            }
    
            // If you FLEE from the enemy (Press ESC)
            if (IsKeyPressed(KEY_ESCAPE)) {
                // We DON'T mark them defeated. 
                currentEnemy->bounds.x = currentEnemy->spawnX;
                currentEnemy->bounds.y = currentEnemy->spawnY;
                myPlayer.Teleport(myPlayer.GetBounds().x, myPlayer.GetBounds().y + 50); 
                currentEnemy = nullptr;
                currentState = STATE_OVERWORLD;
            }
            break;
    }
}

// --- RENDERING ---
void Game::Draw() {
    BeginDrawing();
    
    gameRenderer.DrawFrame(currentState, myPlayer, worldMap);
    dialogueBox.Draw(); 

    EndDrawing();
}