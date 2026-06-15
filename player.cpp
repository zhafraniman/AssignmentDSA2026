#include "player.h"
 
Player::Player(float startX, float startY) {
    position = { startX, startY };
    speed    = 200.0f;
    size     = { 20.0f, 12.0f };
    sprite   = LoadTexture("src/sprite/player.png");
 
    name  = "Bober";
    level = 1;
    maxHp = 25;
    hp    = 25;
}
 
Player::~Player() {
    UnloadTexture(sprite);
}
 
void Player::Update(GameMap& map, Vector2 inputDirection) {
    float deltaTime = GetFrameTime();
 
    Vector2 moveAmount = inputDirection;
 
    // Diagonal normalisation — prevent faster diagonal movement
    if (moveAmount.x != 0.0f && moveAmount.y != 0.0f) {
        moveAmount.x *= 0.7071f;
        moveAmount.y *= 0.7071f;
    }
 
    // Resolve X axis
    position.x += moveAmount.x * speed * deltaTime;
    if (map.CheckCollision(GetBounds()))
        position.x -= moveAmount.x * speed * deltaTime;
 
    // Resolve Y axis
    position.y += moveAmount.y * speed * deltaTime;
    if (map.CheckCollision(GetBounds()))
        position.y -= moveAmount.y * speed * deltaTime;
}
 
Rectangle Player::GetBounds() const {
    float offsetX = (sprite.width  - size.x) / 2.0f;
    float offsetY =  sprite.height - size.y;
    return { position.x + offsetX, position.y + offsetY, size.x, size.y };
}
 
void Player::Draw() {
    DrawTextureV(sprite, position, WHITE);
    // DrawRectangleLinesEx(GetBounds(), 1, RED); // Debug hitbox
}
 
void Player::Teleport(float newX, float newY) {
    position.x = newX;
    position.y = newY;
}
 
// --- Inventory delegates ---
 
bool Player::AddItem(Item newItem) {
    return inventory.AddItem(newItem);
}
 
Item Player::GetInventoryItem(int index) const {
    return inventory.GetItemByIndex(index);
}
 
int Player::GetInventoryCount() const {
    return inventory.GetItemCount();
}
 
bool Player::RemoveItem(int itemID) {
    return inventory.RemoveItem(itemID);
}
 
bool Player::UseItem(int itemID) {
    return inventory.UseItem(itemID);
}
 
// BUG FIX: These two were declared in player.h but never defined,
// causing a linker error whenever anything tried to call them
// (e.g. BattleSystem::Draw querying item quantities).
bool Player::HasIronKey() const {
    return inventory.HasIronKey();
}
 
int Player::GetItemQuantity(int itemID) const {
    return inventory.GetItemQuantity(itemID);
}

void Player::SetName(const std::string& newName) {
    name = newName;
}

void Player::Reset(float startX, float startY) {
    position = { startX, startY };
    level = 1;
    maxHp = 25;
    hp    = 25;
    inventory.Clear(); // Empties your linked list!
}
