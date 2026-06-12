#include "renderer.h"
 
Renderer::Renderer() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Adding a Background");
    SetTargetFPS(60);
    SetExitKey(0); // Disable ESC closing the window (handled in-game)
    bgTexture = LoadTexture("src/sprite/background.png");
}
 
Renderer::~Renderer() {
    UnloadTexture(bgTexture);
    CloseWindow();
}
 
void Renderer::DrawFrame(GameState state, Player& player, GameMap& map, DialogueBox& dialogueBox, int score) {
    switch (state) {
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
 
    DrawText("WASD: move  |  E: chest  |  R: sign  |  M: menu  |  B: debug battle",
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
