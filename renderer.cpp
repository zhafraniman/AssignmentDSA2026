#include "renderer.h"
 
Renderer::Renderer() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Adding a Background");
    SetTargetFPS(60);
    SetExitKey(0); // Disable ESC closing the window (handled in-game)
    bgTexture = LoadTexture("src/sprite/background.png");
    titleSprite = LoadTexture("src/sprite/title.png");
}
 
Renderer::~Renderer() {
    UnloadTexture(bgTexture);
    UnloadTexture(titleSprite);
    CloseWindow();
}
 
void Renderer::DrawFrame(GameState state, Player& player, GameMap& map, DialogueBox& dialogueBox, int score, int menuSelection, const std::string& nameInput, ScoreEntry* leaderboard, int lbCount) {
    switch (state) {
        case STATE_MAIN_MENU:
            DrawMainMenu(menuSelection);
            break;
        case STATE_NAME_INPUT:
            DrawNameInput(nameInput);
            break;
        case STATE_LEADERBOARD:
            DrawLeaderboard(leaderboard, lbCount);
            break;
        case STATE_OVERWORLD:
            DrawOverworld(player, map);
            break;
        case STATE_DIALOGUE:
            DrawOverworld(player, map);
            dialogueBox.Draw();
            break;
        case STATE_MENU:
            DrawOverworld(player, map);
            DrawMenu(player, score);
            break;
        case STATE_BATTLE:
            DrawBattle();
            break;
    }
}
 
void Renderer::DrawOverworld(Player& player, GameMap& map) {
    // Tile the background texture
    for (int y = 0; y < SCREEN_HEIGHT; y += bgTexture.height)
        for (int x = 0; x < SCREEN_WIDTH; x += bgTexture.width)
            DrawTexture(bgTexture, x, y, WHITE);
 
    map.Draw();
    player.Draw();
 
    DrawText("WASD: move  |  E: Interact  |  R: Sign  |  M: Menu  |  B: Debug battle",
             10, 10, 13, RAYWHITE);
}
 
void Renderer::DrawBattle() {
    // Placeholder — real battle rendering is done by BattleSystem::Draw()
    DrawRectangle(600, 100, 64, 64, EnemyColor);
    DrawText("BATTLE ENGAGED!", 200, 250, 20, RAYWHITE);
}
 
void Renderer::DrawGrid() {
    for (int i = 0; i < SCREEN_WIDTH / TILE_SIZE; i++)
        DrawLine(i * TILE_SIZE, 0, i * TILE_SIZE, SCREEN_HEIGHT, LIGHTGRAY);
    for (int i = 0; i < SCREEN_HEIGHT / TILE_SIZE; i++)
        DrawLine(0, i * TILE_SIZE, SCREEN_WIDTH, i * TILE_SIZE, LIGHTGRAY);
}
 
void Renderer::DrawMenu(const Player& player, int score) {
    const int panelX = SCREEN_WIDTH - 260;
    const int panelY = 50;
    const int panelW = 210;
    const int panelH = 340;
 
    DrawRectangle(panelX, panelY, panelW, panelH, MenuPanelColor);
    DrawRectangleLines(panelX, panelY, panelW, panelH, RAYWHITE);
 
    // Stats
    std::string nameText  = "NAME: " + player.GetName();
    std::string levelText = "LVL:  " + std::to_string(player.GetLevel());
    std::string hpText    = "HP:   " + std::to_string(player.GetHP()) +
                            " / "    + std::to_string(player.GetMaxHP());

    std::string scoreText = "SCORE: " + std::to_string(score);
 
    DrawText(nameText.c_str(),  panelX + 15, panelY + 20,  18, RAYWHITE);
    DrawText(levelText.c_str(), panelX + 15, panelY + 50,  18, RAYWHITE);
    DrawText(hpText.c_str(),    panelX + 15, panelY + 80,  18, GREEN);
    DrawText(scoreText.c_str(), panelX + 15, panelY + 110, 18, GOLD);

    // Inventory section
    DrawText("INVENTORY:", panelX + 15, panelY + 145, 18, GOLD);
    DrawLine(panelX + 15, panelY + 167, panelX + panelW - 15, panelY + 167, GOLD);
 
    int drawY = panelY + 177;
    int count = player.GetInventoryCount();
 
    if (count == 0) {
        DrawText("(empty)", panelX + 15, drawY, 15, DARKGRAY);
        drawY += 22;
    }
 
    for (int i = 0; i < count; i++) {
        Item item = player.GetInventoryItem(i);
        if (item.id == 0) continue; // Skip empty slots
 
        // Show name and quantity on the same line
        std::string line = item.name + "  x" + std::to_string(item.quantity);
        DrawText(line.c_str(), panelX + 15, drawY, 14, RAYWHITE);
 
        // Show item description in a smaller, dimmer font on the next line
        if (!item.description.empty()) {
            DrawText(item.description.c_str(), panelX + 20, drawY + 16, 11, LIGHTGRAY);
            drawY += 38;
        } else {
            drawY += 22;
        }
    }
 
    // Key hint at the bottom
    DrawText("Press 'M' to close", panelX + 15, panelY + panelH - 25, 13, LIGHTGRAY);
}

void Renderer::DrawMainMenu(int selection) {
    ClearBackground(BLACK);
    
    // Calculate perfect centering for the X axis
    int titleX = (SCREEN_WIDTH - titleSprite.width) / 2;
    int titleY = 80; // Distance from the top of the screen
    DrawTexture(titleSprite, titleX, titleY, WHITE);
    
    std::string startText = (selection == 0) ? "> START <" : "START";
    std::string leadText  = (selection == 1) ? "> LEADERBOARDS <" : "LEADERBOARDS";
    
    Color startColor = (selection == 0) ? GREEN : GRAY;
    Color leadColor  = (selection == 1) ? GREEN : GRAY;

    // 3. Dynamically calculate the centered X coordinates based on real-time length
    int startX = (SCREEN_WIDTH - MeasureText(startText.c_str(), 30)) / 2;
    int leadX  = (SCREEN_WIDTH - MeasureText(leadText.c_str(), 30)) / 2;

    // 4. Draw the text options using our perfectly calculated centered positions!
    DrawText(startText.c_str(), startX, 300, 30, startColor);
    DrawText(leadText.c_str(), leadX, 360, 30, leadColor);
}

void Renderer::DrawNameInput(const std::string& currentName) {
    ClearBackground(BLACK);
    DrawText("ENTER YOUR NAME:", 250, 200, 30, RAYWHITE);
    
    // Draw a box for the text
    DrawRectangleLines(250, 260, 300, 50, GREEN);
    DrawText(currentName.c_str(), 260, 275, 20, RAYWHITE);
    
    DrawText("Press ENTER to confirm", 280, 350, 20, GRAY);
    DrawText("Press ESC to go back", 290, 390, 20, GRAY);
}

void Renderer::DrawLeaderboard(ScoreEntry* leaderboard, int count) {
    ClearBackground(BLACK);
    
    // Center the main screen header title dynamically
    std::string titleText = "LEADERBOARD";
    int titleX = (SCREEN_WIDTH - MeasureText(titleText.c_str(), 30)) / 2;
    DrawText(titleText.c_str(), titleX, 50, 30, GREEN);

    if (count == 0) {
        // Center the fallback text if there are no high scores yet
        std::string emptyText = "No scores yet!";
        int emptyX = (SCREEN_WIDTH - MeasureText(emptyText.c_str(), 20)) / 2;
        DrawText(emptyText.c_str(), emptyX, 200, 20, RAYWHITE);
    } else {
        // Define our spreadsheet column blueprint
        int tableWidth = 550; // Total horizontal footprint of our columns combined
        int startX = (SCREEN_WIDTH - tableWidth) / 2; // Centered anchor point
        
        // Calculate the exact horizontal start pixel for each individual column
        int colNameX  = startX + 0;   // Far left column
        int colScoreX = startX + 280; // Middle column
        int colTimeX  = startX + 430; // Far right column
        
        int drawY = 120; // Vertical starting height for our table
        
        // Draw Table Column Labels
        DrawText("NAME",  colNameX,  drawY, 20, GOLD);
        DrawText("SCORE", colScoreX, drawY, 20, GOLD);
        DrawText("TIME",  colTimeX,  drawY, 20, GOLD);
        
        // Separator bar directly underneath our headers
        DrawLine(startX, drawY + 25, startX + tableWidth, drawY + 25, GOLD);
        
        drawY += 45; // Shift our vertical position down to begin printing player data rows
        
        // Print each score entry row-by-row inside their aligned zones
        for (int i = 0; i < count; i++) {
            // Split the values up into distinct strings instead of jamming them together
            std::string nameStr  = std::to_string(i + 1) + ". " + leaderboard[i].name;
            std::string scoreStr = std::to_string(leaderboard[i].score);
            std::string timeStr  = std::to_string(leaderboard[i].timeSeconds) + "s";
            
            // Render each piece of data safely aligned to its respective column X coordinate
            DrawText(nameStr.c_str(),  colNameX,  drawY, 20, RAYWHITE);
            DrawText(scoreStr.c_str(), colScoreX, drawY, 20, RAYWHITE);
            DrawText(timeStr.c_str(),  colTimeX,  drawY, 20, RAYWHITE);
            
            drawY += 35; // Drop down to the next row row level
        }
    }
    
    // 5. Center the navigation tip text at the bottom dynamically
    std::string footerText = "Press ESC to return";
    int footerX = (SCREEN_WIDTH - MeasureText(footerText.c_str(), 20)) / 2;
    DrawText(footerText.c_str(), footerX, 520, 20, GRAY);
}
