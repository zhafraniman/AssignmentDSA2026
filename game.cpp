#include "game.h"
#include <fstream>

// --- INITIALIZATION ---
Game::Game() : myPlayer(388.0f, 256.0f) {
    worldMap.LoadMap("src/levels/level1.txt");

    currentState = STATE_OVERWORLD;
    currentEnemy = nullptr;

    // FIX: Prevent portal spam loop
    wasTouchingPortal = false;

    std::ifstream inputFile("usersfile.txt");
    inputFile >> fileScore;
    inputFile.close();
}

// --- CLEANUP ---
Game::~Game() {
    std::ofstream outputFile("usersfile.txt", std::ios::out);
    outputFile << fileScore;
    outputFile.close();
}

// --- MAIN LOOP ---
void Game::Run() {
    while (!WindowShouldClose()) {
        ProcessInput();
        Update();
        Draw();
    }
}

// --- INPUT HANDLING ---
void Game::ProcessInput() {
    playerInput = { 0.0f, 0.0f };

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        playerInput.y -= 1.0f;

    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        playerInput.y += 1.0f;

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        playerInput.x -= 1.0f;

    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        playerInput.x += 1.0f;
}

// --- GAME LOGIC ---
void Game::Update() {

    switch (currentState) {

        // ============================================================
        case STATE_OVERWORLD: {

            myPlayer.Update(worldMap, playerInput);

            worldMap.UpdateEnemies(myPlayer.GetBounds());

            // --------------------------------------------------------
            // ENEMY COLLISION -> START BATTLE
            // --------------------------------------------------------
            Enemy* touchedEnemy =
                worldMap.CheckEnemyCollision(myPlayer.GetBounds());

            if (touchedEnemy != nullptr) {

                currentEnemy = touchedEnemy;

                battle.StartBattle();

                currentState = STATE_BATTLE;

                break;
            }

            // --------------------------------------------------------
            // CHEST INTERACTION (E)
            // --------------------------------------------------------
            if (IsKeyPressed(KEY_E)) {

                Chest* nearbyChest =
                    worldMap.CheckChestInteraction(myPlayer.GetBounds());

                if (nearbyChest != nullptr &&
                    !nearbyChest->isOpen) {

                    if (myPlayer.AddItem(nearbyChest->content)) {

                        worldMap.MarkChestOpened(nearbyChest);

                        nearbyChest->isOpen = true;

                        fileScore += 10;

                        dialogueBox.Start();

                        dialogueBox.Enqueue(
                            "You found: " +
                            nearbyChest->content.name + "!"
                        );

                        currentState = STATE_DIALOGUE;

                    } else {

                        dialogueBox.Start();

                        dialogueBox.Enqueue(
                            "Your inventory is full!"
                        );

                        currentState = STATE_DIALOGUE;
                    }
                }
            }

            // --------------------------------------------------------
            // SIGNPOST INTERACTION (R)
            // --------------------------------------------------------
            if (IsKeyPressed(KEY_R)) {

                Signpost* nearbySign =
                    worldMap.CheckSignpostInteraction(
                        myPlayer.GetBounds()
                    );

                if (nearbySign != nullptr) {

                    dialogueBox.Start();

                    for (int i = 0;
                         i < nearbySign->lineCount;
                         i++) {

                        dialogueBox.Enqueue(
                            nearbySign->dialogue[i]
                        );
                    }

                    currentState = STATE_DIALOGUE;
                }
            }

            // --------------------------------------------------------
            // PORTAL SYSTEM
            // FIXED: Prevent infinite dialogue spam
            // --------------------------------------------------------
            {
                Portal hitPortal;

                bool touchingPortal =
                    worldMap.CheckPortals(
                        myPlayer.GetBounds(),
                        hitPortal
                    );

                // Trigger ONLY when entering portal
                if (touchingPortal &&
                    !wasTouchingPortal) {

                    // ---------------- LOCKED PORTAL ----------------
                    if (hitPortal.requiresKey &&
                        !myPlayer.HasIronKey()) {

                        dialogueBox.Start();

                        dialogueBox.Enqueue(
                            "The door is locked."
                        );

                        dialogueBox.Enqueue(
                            "You need an Iron Key to pass."
                        );

                        currentState = STATE_DIALOGUE;
                    }

                    // ---------------- NORMAL PORTAL ----------------
                    else {

                        // Consume key if needed
                        if (hitPortal.requiresKey) {

                            myPlayer.UseItem(
                                ITEM_IRON_KEY
                            );
                        }

                        // Load next map
                        worldMap.LoadMap(
                            hitPortal.targetMap
                        );

                        // Teleport player
                        myPlayer.Teleport(
                            hitPortal.spawnX,
                            hitPortal.spawnY
                        );

                        // Safety:
                        // Move away if player spawns on enemy
                        const float offX[] = {
                            50.0f,
                           -50.0f,
                             0.0f,
                             0.0f
                        };

                        const float offY[] = {
                             0.0f,
                             0.0f,
                           -50.0f,
                            50.0f
                        };

                        for (int i = 0; i < 4; i++) {

                            if (worldMap.CheckEnemyCollision(
                                    myPlayer.GetBounds()
                                ) == nullptr)
                            {
                                break;
                            }

                            myPlayer.Teleport(
                                hitPortal.spawnX + offX[i],
                                hitPortal.spawnY + offY[i]
                            );
                        }
                    }
                }

                // IMPORTANT:
                // Update AFTER processing
                wasTouchingPortal = touchingPortal;
            }

            // --------------------------------------------------------
            // MENU
            // --------------------------------------------------------
            if (IsKeyPressed(KEY_M)) {

                currentState = STATE_MENU;
            }

            // --------------------------------------------------------
            // DEBUG BATTLE
            // --------------------------------------------------------
            if (IsKeyPressed(KEY_B)) {

                battle.StartBattle();

                currentEnemy = nullptr;

                currentState = STATE_BATTLE;
            }

            break;
        }

        // ============================================================
        case STATE_DIALOGUE: {

            dialogueBox.Update();

            if (IsKeyPressed(KEY_SPACE)) {

                if (dialogueBox.IsTextFinished()) {

                    dialogueBox.Dequeue();

                    if (!dialogueBox.IsActive()) {

                        currentState = STATE_OVERWORLD;
                    }

                } else {

                    dialogueBox.SkipTyping();
                }
            }

            break;
        }

        // ============================================================
        case STATE_MENU: {

            if (IsKeyPressed(KEY_M) ||
                IsKeyPressed(KEY_ESCAPE)) {

                currentState = STATE_OVERWORLD;
            }

            break;
        }

        // ============================================================
        case STATE_BATTLE: {

            if (!battle.IsBattleOver()) {

                battle.Update(myPlayer);
            }

            if (battle.IsBattleOver()) {

                if (IsKeyPressed(KEY_SPACE) ||
                    IsKeyPressed(KEY_ESCAPE)) {

                    // ------------------------------------------------
                    // PLAYER LOST
                    // ------------------------------------------------
                    if (battle.GetState() == PLAYER_LOSE) {

                        worldMap.ResetDefeatedEnemies();

                        worldMap.LoadMap(
                            "src/levels/level1.txt"
                        );

                        myPlayer.Teleport(
                            388.0f,
                            256.0f
                        );

                        battle.get_healing() =
                            battle.max_HP();

                        currentEnemy = nullptr;

                        battle.StartBattle();

                        currentState = STATE_OVERWORLD;
                    }

                    // ------------------------------------------------
                    // PLAYER WON
                    // ------------------------------------------------
                    else {

                        std::string lootMsg = "";

                        if (currentEnemy != nullptr) {

                            Item loot =
                                worldMap.GetEnemyLoot(
                                    currentEnemy
                                );

                            worldMap.MarkEnemyDefeated(
                                currentEnemy
                            );

                            currentEnemy = nullptr;

                            if (loot.id != 0) {

                                if (myPlayer.AddItem(loot)) {

                                    lootMsg =
                                        "You received: " +
                                        loot.name + "!";

                                    fileScore += 20;

                                } else {

                                    lootMsg =
                                        "Inventory full! " +
                                        loot.name +
                                        " was lost.";
                                }
                            }
                        }

                        battle.StartBattle();

                        if (!lootMsg.empty()) {

                            dialogueBox.Start();

                            dialogueBox.Enqueue(
                                lootMsg
                            );

                            currentState = STATE_DIALOGUE;

                        } else {

                            currentState =
                                STATE_OVERWORLD;
                        }
                    }
                }
            }

            break;
        }
    }
}

// --- RENDERING ---
void Game::Draw() {

    BeginDrawing();

    if (currentState == STATE_BATTLE) {

        battle.Draw(myPlayer);

    } else {

        gameRenderer.DrawFrame(
            currentState,
            myPlayer,
            worldMap,
            dialogueBox
        );
    }

    // ------------------------------------------------------------
    // SCORE DISPLAY
    // ------------------------------------------------------------
    if (currentState != STATE_BATTLE) {

        DrawRectangle(
            50,
            50,
            100,
            100,
            YELLOW
        );

        DrawRectangleLines(
            50,
            50,
            100,
            100,
            BLACK
        );

        DrawText(
            std::to_string(fileScore).c_str(),
            55,
            60,
            50,
            BLACK
        );
    }

    EndDrawing();
}
