#include "map.h"
#include <fstream>
#include <iostream>
#include <math.h>

GameMap::GameMap() {
    for (int y = 0; y < MAP_ROWS; y++) {
        for (int x = 0; x < MAP_COLS; x++) {
            grid[y][x] = 0;
        }
    }

    wallSprite = LoadTexture("src/sprite/wall.png"); 
    portalSprite = LoadTexture("src/sprite/door.png");
    chestClosedSprite = LoadTexture("src/sprite/chest_closed.png");
    chestOpenSprite = LoadTexture("src/sprite/chest_open.png");
    signSprite = LoadTexture("src/sprite/sign.png");
    enemySprite = LoadTexture("src/sprite/enemy.png");
    
    //Initialize our manual counter to 0
    portalCount = 0; 
    chestCount = 0; 
    historyCount = 0;
    signpostCount = 0;
    enemyCount = 0;
    defeatedCount = 0;
}

GameMap::~GameMap() {
    UnloadTexture(wallSprite); 
    UnloadTexture(portalSprite);
    UnloadTexture(chestClosedSprite);
    UnloadTexture(chestOpenSprite);
    UnloadTexture(signSprite);
    UnloadTexture(enemySprite);
}

bool GameMap::LoadMap(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "ERROR: Could not load map file: " << filename << std::endl;
        return false;
    }

    // CLEAR OLD DATA, avoids dupes
    portalCount = 0; 
    chestCount = 0;
    signpostCount = 0;
    enemyCount = 0;

    // 2. Read the Map Grid
    for (int y = 0; y < MAP_ROWS; y++) {
        for (int x = 0; x < MAP_COLS; x++) {
            file >> grid[y][x]; 
        }
    }

    // 3. THE ARCHITECTURE UPGRADE: Read all remaining chunks of data
    std::string marker;
    
    // This loop runs continuously, sucking up one word at a time, until the file ends.
    while (file >> marker) {
        
        if (marker == "PORTALS") {
            int numPortals;
            file >> numPortals; 
            for (int i = 0; i < numPortals; i++) {
                int gridX, gridY, spawnGridX, spawnGridY;
                std::string target;
                file >> gridX >> gridY >> target >> spawnGridX >> spawnGridY;
                
                float px = (float)(gridX * TILE_SIZE);
                float py = (float)(gridY * TILE_SIZE);
                float sx = (float)(spawnGridX * TILE_SIZE);
                float sy = (float)(spawnGridY * TILE_SIZE);
                
                AddPortal({px, py, (float)TILE_SIZE, (float)TILE_SIZE}, target, sx, sy);
            }
        } 

        else if (marker == "CHESTS") { 
            int numChests;
            file >> numChests;
            for (int i = 0; i < numChests; i++) {
                int gridX, gridY, chestID, itemID;
                std::string itemName;
                
                // Read the new ChestID from the file
                file >> gridX >> gridY >> chestID >> itemID >> itemName;

                // Inside LoadMap(), parsing the CHESTS...
                float px = (float)(gridX * TILE_SIZE);
                float py = (float)(gridY * TILE_SIZE);
                
                Chest newChest;
                // Use our new float variables and cast TILE_SIZE!
                
                newChest.bounds = {px, py, (float)TILE_SIZE, (float)TILE_SIZE};
                //newChest.bounds = {gridX * TILE_SIZE, gridY * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                newChest.content = {itemID, itemName};
                newChest.isOpen = false;
                newChest.uniqueID = chestID; // Store the ID!

                std::cout << "[LOADER] Reading blueprint for Chest ID: " << chestID << std::endl;

                // --- THE NEW, ULTRA-FAST MEMORY CHECK ---
                for (int j = 0; j < historyCount; j++) {
                    // Just compare the two numbers!
                    if (openedHistory[j] == chestID) {
                        newChest.isOpen = true; 
                        newChest.content.id = 0; 
                        std::cout << "[LOADER] Match found in history! Forcing ID " << chestID << " open." << std::endl;
                        break; 
                    }
                }
                // ----------------------------------------

                if (chestCount < MAX_CHESTS) {
                    chests[chestCount] = newChest;
                    chestCount++;
                }
            }
        }
        
        else if (marker == "SIGNPOSTS") {
            int numSigns;
            file >> numSigns;
            
            for (int i = 0; i < numSigns; i++) {
                int gridX, gridY, numLines;
                file >> gridX >> gridY >> numLines;

                Signpost newSign;
                newSign.bounds = {(float)(gridX * TILE_SIZE), (float)(gridY * TILE_SIZE), (float)TILE_SIZE, (float)TILE_SIZE};
                newSign.lineCount = 0;

                // THE C++ TRAP FIX: 
                // We must "eat" the invisible Enter key (\n) left behind by file >>
                std::string dummy;
                std::getline(file, dummy); 

                // Now we read the actual sentences
                for (int j = 0; j < numLines; j++) {
                    std::string line;
                    std::getline(file, line); // Grabs the whole line, spaces included!
                    
                    if (newSign.lineCount < MAX_LINES_PER_SIGNPOST) {
                        newSign.dialogue[newSign.lineCount] = line;
                        newSign.lineCount++;
                    }
                }

                if (signpostCount < MAX_SIGNPOSTS) {
                    signposts[signpostCount] = newSign;
                    signpostCount++;
                }
            }
        } else if (marker == "ENEMIES") {
            int numEnemies;
            file >> numEnemies;
    
            for (int i = 0; i < numEnemies; i++) {
            int gridX, gridY, enemyID, aggroRange;
            float speed;
        
            // Read the blueprint data from the text file
            file >> gridX >> gridY >> enemyID >> aggroRange >> speed;

            float px = (float)(gridX * TILE_SIZE);
            float py = (float)(gridY * TILE_SIZE);

            // We are making the hitbox 20 pixels wide and 20 pixels tall.
            float hitboxWidth = 20.0f;
            float hitboxHeight = 20.0f;

            // Calculate how far to push the box so it sits in the middle-bottom of the 32x32 tile
            float offsetX = (TILE_SIZE - hitboxWidth) / 2.0f; 
            float offsetY = TILE_SIZE - hitboxHeight; 

            
            // Build the enemy
            Enemy newEnemy;
            // Apply the offsets to the starting position!
            newEnemy.bounds = {px + offsetX, py + offsetY, hitboxWidth, hitboxHeight};
            newEnemy.uniqueID = enemyID;
            newEnemy.aggroRange = aggroRange;
            newEnemy.speed = speed;
            // IMPORTANT: Keep spawnX and spawnY as the original tile coordinates 
            // so they walk back to the exact right spot if you flee!
            newEnemy.spawnX = px + offsetX; 
            newEnemy.spawnY = py + offsetY;
            newEnemy.isDefeated = false;

            // --- THE MEMORY CHECK ---
            for (int j = 0; j < defeatedCount; j++) {
                if (defeatedHistory[j] == enemyID) {
                    newEnemy.isDefeated = true; 
                    std::cout << "[LOADER] Enemy ID " << enemyID << " is already dead." << std::endl;
                    break; 
                }
            }

            // Save the enemy into the map's array
            if (enemyCount < MAX_ENEMIES) {
                enemies[enemyCount] = newEnemy;
                enemyCount++;
            }
        }
    }
}

    file.close();
    return true;
}
void GameMap::AddPortal(Rectangle bounds, std::string targetMap, float spawnX, float spawnY) {
    // Ensure we don't exceed hardcoded array limit
    if (portalCount < MAX_PORTALS) {
        Portal newPortal;
        newPortal.bounds = bounds;
        newPortal.targetMap = targetMap;
        newPortal.spawnX = spawnX;
        newPortal.spawnY = spawnY;

        // Slot the new portal into the array at the current count, then increment the count
        portals[portalCount] = newPortal;
        portalCount++;
    } else {
        std::cout << "WARNING: Max portals reached. Cannot add more!" << std::endl;
    }
}

bool GameMap::CheckPortals(Rectangle playerBounds, Portal& outPortal) {
    // REPLACED: portals.size() with our manual portalCount
    for (int i = 0; i < portalCount; i++) {
        if (CheckCollisionRecs(playerBounds, portals[i].bounds)) {
            outPortal = portals[i];
            return true; 
        }
    }
    return false; 
}

void GameMap::Draw() {
    // Draw the walls
    for (int y = 0; y < MAP_ROWS; y++) {
        for (int x = 0; x < MAP_COLS; x++) {
            if (grid[y][x] == 1) {
                DrawTexture(wallSprite, x * TILE_SIZE, y * TILE_SIZE, WHITE);
            }
        }
    }

    // Draw the portals
    for (int i = 0; i < portalCount; i++) {
        // Cast the float coordinates back to integers (int) for pixel-perfect drawing
        int drawX = (int)portals[i].bounds.x;
        int drawY = (int)portals[i].bounds.y;
        
        // Stamp the portal sprite onto the screen!
        DrawTexture(portalSprite, drawX, drawY, WHITE);
    }

    // Draw the chest
    for (int i = 0; i < chestCount; i++) {
        int drawX = (int)chests[i].bounds.x;
        int drawY = (int)chests[i].bounds.y;
        if (chests[i].isOpen) {
            // If it's been looted, draw the empty open chest
            DrawTexture(chestOpenSprite, drawX, drawY, WHITE);
        } else {
            // If it hasn't been looted, draw the shiny closed chest
            DrawTexture(chestClosedSprite, drawX, drawY, WHITE);
        }
    }

    // Draw the signpost
    for (int i = 0; i < signpostCount; i++) {
        // Grab the exact X and Y pixel coordinates, casting them to integers for the screen
        int drawX = (int)signposts[i].bounds.x;
        int drawY = (int)signposts[i].bounds.y;

        // Stamp the signpost onto the map
        DrawTexture(signSprite, drawX, drawY, WHITE);
    }

    // Draw the enemy
    for (int i = 0; i < enemyCount; i++) {
    // Only draw the enemy if they haven't been killed yet!
    if (!enemies[i].isDefeated) {
        int drawX = (int)(enemies[i].bounds.x - 6.0f); // 6 is the offsetX we calculated above
        int drawY = (int)(enemies[i].bounds.y - 12.0f); // 12 is the offsetY we calculated above
        
        // Stamp the custom enemy sprite onto the screen
        DrawTexture(enemySprite, drawX, drawY, WHITE);
        if (isAggro) {
            DrawTexture(enemySprite, drawX, drawY, RED);
        }

        // Debugging
        DrawRectangleLinesEx(enemies[i].bounds, 1, RED);
    }
}
}

bool GameMap::IsSolid(int targetX, int targetY) {
    // BOUNDS CHECK: Prevent crashing if checking outside the screen
    if (targetX < 0 || targetX >= MAP_COLS || targetY < 0 || targetY >= MAP_ROWS) {
        return true; 
    }

    // TILE CHECK: Is the destination a wall
    if (grid[targetY][targetX] == 1) {
        return true;
    }

    return false; 
}

bool GameMap::CheckCollision(Rectangle rect) {
    // Convert the pixel boundaries of the bounding box into grid rows and columns
    int startCol = (int)(rect.x / TILE_SIZE);
    int endCol = (int)((rect.x + rect.width) / TILE_SIZE);
    int startRow = (int)(rect.y / TILE_SIZE);
    int endRow = (int)((rect.y + rect.height) / TILE_SIZE);

    // Scan only the specific subset of tiles the bounding box is currently touching
    for (int row = startRow; row <= endRow; row++) {
        for (int col = startCol; col <= endCol; col++) {
            if (IsSolid(col, row)) {
                return true; // Hit a wall!
            }
        }
    }
    return false; // Safe to walk
}

void GameMap::AddChest(Rectangle bounds, Item content) {
    if (chestCount < MAX_CHESTS) {
        Chest newChest;
        newChest.bounds = bounds;
        newChest.content = content;
        newChest.isOpen = false;
        
        chests[chestCount] = newChest;
        chestCount++;
    }
}

// THE INTERACTION CHECK
Chest* GameMap::CheckChestInteraction(Rectangle playerBounds) {
    // We create a "reach" box that is slightly larger than the player
    // so they don't have to be perfectly colliding with the chest to open it.
    Rectangle reach = { 
        playerBounds.x - 10, 
        playerBounds.y - 10, 
        playerBounds.width + 20, 
        playerBounds.height + 20 
    };

    for (int i = 0; i < chestCount; i++) {
        if (CheckCollisionRecs(reach, chests[i].bounds)) {
            // We found a chest! Return its exact memory address using the '&' symbol
            return &chests[i]; 
        }
    }
    
    return nullptr; // nullptr means "We found absolutely nothing."
}

void GameMap::MarkChestOpened(Chest* chest) {
    chest->isOpen = true;
    chest->content.id = 0; 
    
    if (historyCount < MAX_HISTORY) {
        // Just write down the ID number in our journal!
        openedHistory[historyCount] = chest->uniqueID;
        historyCount++; 

        std::cout << "[MEMORY] Saved Chest ID: " << chest->uniqueID 
                  << " | Total opened: " << historyCount << std::endl;
    }
}

Signpost* GameMap::CheckSignpostInteraction(Rectangle playerBounds) {
    // Create the "reach" box
    Rectangle reach = { 
        playerBounds.x - 10, playerBounds.y - 10, 
        playerBounds.width + 20, playerBounds.height + 20 
    };

    for (int i = 0; i < signpostCount; i++) {
        if (CheckCollisionRecs(reach, signposts[i].bounds)) {
            return &signposts[i]; 
        }
    }
    return nullptr;
}

void GameMap::UpdateEnemies(Rectangle playerBounds) {
    float deltaTime = GetFrameTime();
    
    // THE SEARCHING ALGO: Linear Search through all enemies
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i].isDefeated) continue; // Skip dead enemies
        
        // Find the center of the player and the enemy
        float playerCenterX = playerBounds.x + (playerBounds.width / 2);
        float playerCenterY = playerBounds.y + (playerBounds.height / 2);
        float enemyCenterX = enemies[i].bounds.x + (enemies[i].bounds.width / 2);
        float enemyCenterY = enemies[i].bounds.y + (enemies[i].bounds.height / 2);
        
        // 1. Calculate the distance using the Pythagorean theorem
        float distanceX = playerCenterX - enemyCenterX;
        float distanceY = playerCenterY - enemyCenterY;
        float totalDistance = sqrt((distanceX * distanceX) + (distanceY * distanceY));
        
        // 2. Are they inside the aggro range? CHASE!
        if (totalDistance < enemies[i].aggroRange) {
            isAggro = true;
            
            int enemyGridX = (int)(enemyCenterX / TILE_SIZE);
            int enemyGridY = (int)(enemyCenterY / TILE_SIZE);
            int playerGridX = (int)(playerCenterX / TILE_SIZE);
            int playerGridY = (int)(playerCenterY / TILE_SIZE);

            // Ask the Pathfinder where to go next!
            Point2D nextStep = GetNextPathStep(enemyGridX, enemyGridY, playerGridX, playerGridY);

            // Convert the target Grid Tile back into exactly the center pixels
            float targetPixelX = (nextStep.x * TILE_SIZE) + (TILE_SIZE / 2.0f);
            float targetPixelY = (nextStep.y * TILE_SIZE) + (TILE_SIZE / 2.0f);

            // Now do standard vector math aiming at the target tile instead of the player!
            float stepDistX = targetPixelX - enemyCenterX;
            float stepDistY = targetPixelY - enemyCenterY;
            float stepTotalDist = sqrt((stepDistX * stepDistX) + (stepDistY * stepDistY));

            if (stepTotalDist > 2.0f) { // Prevent jittering when reaching the exact center
                float dirX = stepDistX / stepTotalDist;
                float dirY = stepDistY / stepTotalDist;

                enemies[i].bounds.x += dirX * enemies[i].speed * deltaTime;
                if (CheckCollision(enemies[i].bounds)) enemies[i].bounds.x -= dirX * enemies[i].speed * deltaTime;

                enemies[i].bounds.y += dirY * enemies[i].speed * deltaTime;
                if (CheckCollision(enemies[i].bounds)) enemies[i].bounds.y -= dirY * enemies[i].speed * deltaTime;
            }
        } else {
            isAggro = false;
            // --- THE WALK BACK HOME LOGIC ---
            
            // 1. Calculate the distance from the enemy's CURRENT position to its SPAWN position
            float distToSpawnX = enemies[i].spawnX - enemies[i].bounds.x;
            float distToSpawnY = enemies[i].spawnY - enemies[i].bounds.y;
            float totalDistToSpawn = sqrt((distToSpawnX * distToSpawnX) + (distToSpawnY * distToSpawnY));
            
            // 2. Only walk if they aren't already standing exactly on their spawn point
            if (totalDistToSpawn > 1.0f) {
                
                // 3. Normalize the vector (just like the chase logic!)
                float dirX = distToSpawnX / totalDistToSpawn;
                float dirY = distToSpawnY / totalDistToSpawn;
                
                // Let's make them walk back at half speed so it looks like they are giving up
                float returnSpeed = enemies[i].speed * 0.5f; 
                
                // Resolve X Axis Movement
                enemies[i].bounds.x += dirX * returnSpeed * deltaTime;
                if (CheckCollision(enemies[i].bounds)) {
                    enemies[i].bounds.x -= dirX * returnSpeed * deltaTime; 
                }

                // Resolve Y Axis Movement
                enemies[i].bounds.y += dirY * returnSpeed * deltaTime;
                if (CheckCollision(enemies[i].bounds)) {
                    enemies[i].bounds.y -= dirY * returnSpeed * deltaTime; 
                }
            }
        }
    }
}

Enemy* GameMap::CheckEnemyCollision(Rectangle playerBounds) {
    for (int i = 0; i < enemyCount; i++) {
        if (!enemies[i].isDefeated && CheckCollisionRecs(playerBounds, enemies[i].bounds)) {
            return &enemies[i]; // Return the specific enemy we touched
        }
    }
    return nullptr;
}

void GameMap::MarkEnemyDefeated(Enemy* enemy) {
    enemy->isDefeated = true;
    if (defeatedCount < MAX_ENEMY_HISTORY) {
        defeatedHistory[defeatedCount] = enemy->uniqueID;
        defeatedCount++;
    }
}

Point2D GameMap::GetNextPathStep(int startX, int startY, int targetX, int targetY) {
    // 1. The Custom Queue (Max size is grid width * height)
    Point2D queue[MAP_ROWS * MAP_COLS];
    int front = 0;
    int back = 0;

    // 2. Memory Arrays (To remember where we walked)
    bool visited[MAP_ROWS][MAP_COLS] = {false};
    Point2D parent[MAP_ROWS][MAP_COLS]; // Remembers "who" walked onto this tile

    // Setup the starting point
    queue[back] = {startX, startY};
    back++;
    visited[startY][startX] = true;
    parent[startY][startX] = {-1, -1}; // The start has no parent

    // Directions: Up, Down, Left, Right
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    bool found = false;

    // 3. The Search Loop
    while (front < back) {
        Point2D current = queue[front];
        front++; // Dequeue the first item

        // Did we find the player's tile?
        if (current.x == targetX && current.y == targetY) {
            found = true;
            break;
        }

        // Look at all 4 neighbor tiles
        for (int i = 0; i < 4; i++) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            // Bounds check & wall check
            if (nx >= 0 && nx < MAP_COLS && ny >= 0 && ny < MAP_ROWS) {
                if (grid[ny][nx] != 1 && !visited[ny][nx]) {
                    visited[ny][nx] = true;         // Mark as seen
                    parent[ny][nx] = current;       // Draw an arrow back to where we came from
                    
                    queue[back] = {nx, ny};         // Enqueue this new tile to check later
                    back++;
                }
            }
        }
    }

    // If the player is surrounded by walls and unreachable, just stand still
    if (!found) return {startX, startY}; 

    // 4. Backtracking: Trace the arrows backward from the player to the enemy
    Point2D step = {targetX, targetY};
    
    // Keep stepping backward until the tile we are standing on is exactly one step away from the enemy's start
    while (parent[step.y][step.x].x != startX || parent[step.y][step.x].y != startY) {
        step = parent[step.y][step.x];
    }

    return step; // This is the immediate next tile the enemy must walk to!
}