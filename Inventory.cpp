#include "Inventory.h"
#include <iostream>

Inventory::Inventory() : head(nullptr), itemCount(0) {}

Inventory::~Inventory() {
    Clear();
}

void Inventory::Clear() {
    while (head != nullptr) {
        InventoryNode* temp = head;
        head = head->next;
        delete temp;
    }
    itemCount = 0;
}

bool Inventory::AddItem(Item newItem) {
    // Check if inventory is full
    if (IsFull()) {
        return false;
    }
    
    // Check if item already exists (for stackable items)
    InventoryNode* current = head;
    while (current != nullptr) {
        if (current->data.id == newItem.id) {
            current->data.quantity += newItem.quantity;
            return true;
        }
        current = current->next;
    }
    
    // Item doesn't exist, create new node
    InventoryNode* newNode = new InventoryNode(newItem);
    
    // Add to front of list
    newNode->next = head;
    head = newNode;
    itemCount++;
    
    return true;
}

bool Inventory::RemoveItem(int itemID) {
    if (head == nullptr) return false;
    
    // If head needs to be removed
    if (head->data.id == itemID) {
        InventoryNode* temp = head;
        head = head->next;
        delete temp;
        itemCount--;
        return true;
    }
    
    // Search for item in rest of list
    InventoryNode* current = head;
    while (current->next != nullptr) {
        if (current->next->data.id == itemID) {
            InventoryNode* temp = current->next;
            current->next = temp->next;
            delete temp;
            itemCount--;
            return true;
        }
        current = current->next;
    }
    
    return false;
}

bool Inventory::RemoveItemByIndex(int index) {
    if (index < 0 || index >= itemCount) return false;
    
    // If removing first item
    if (index == 0) {
        InventoryNode* temp = head;
        head = head->next;
        delete temp;
        itemCount--;
        return true;
    }
    
    // Navigate to position before target
    InventoryNode* current = head;
    for (int i = 0; i < index - 1; i++) {
        if (current->next == nullptr) return false;
        current = current->next;
    }
    
    InventoryNode* temp = current->next;
    current->next = temp->next;
    delete temp;
    itemCount--;
    
    return true;
}

Item Inventory::GetItemByID(int itemID) {
    InventoryNode* current = head;
    while (current != nullptr) {
        if (current->data.id == itemID) {
            return current->data;
        }
        current = current->next;
    }
    return Item{0, "EMPTY", "No item found", 0};
}

Item Inventory::GetItemByIndex(int index) const {
    if (index < 0 || index >= itemCount) {
        return Item{0, "EMPTY", "Index out of bounds", 0};
    }
    
    InventoryNode* current = head;
    for (int i = 0; i < index; i++) {
        if (current == nullptr) {
            return Item{0, "EMPTY", "Not found", 0};
        }
        current = current->next;
    }
    
    return current->data;
}

bool Inventory::FindItem(int itemID) const {
    InventoryNode* current = head;
    while (current != nullptr) {
        if (current->data.id == itemID) {
            return true;
        }
        current = current->next;
    }
    return false;
}

bool Inventory::IncreaseQuantity(int itemID, int amount) {
    InventoryNode* current = head;
    while (current != nullptr) {
        if (current->data.id == itemID) {
            current->data.quantity += amount;
            return true;
        }
        current = current->next;
    }
    return false;
}

bool Inventory::DecreaseQuantity(int itemID, int amount) {
    InventoryNode* current = head;
    while (current != nullptr) {
        if (current->data.id == itemID) {
            if (current->data.quantity >= amount) {
                current->data.quantity -= amount;
                if (current->data.quantity == 0) {
                    RemoveItem(itemID);
                }
                return true;
            }
            return false;  // Not enough quantity
        }
        current = current->next;
    }
    return false;  // Item not found
}

bool Inventory::UseItem(int itemID) {
    return DecreaseQuantity(itemID, 1);
}

bool Inventory::HasKey() const {
    return FindItem(ITEM_IRON_KEY);
}

int Inventory::GetItemQuantity(int itemID) const {
    InventoryNode* current = head;
    while (current != nullptr) {
        if (current->data.id == itemID) {
            return current->data.quantity;
        }
        current = current->next;
    }
    return 0;
}

void Inventory::DisplayInventory() const {
    std::cout << "\n=== INVENTORY ===" << std::endl;
    std::cout << "Items: " << itemCount << " / " << MAX_INVENTORY << std::endl;
    
    InventoryNode* current = head;
    int index = 0;
    
    while (current != nullptr) {
        std::cout << "[" << index << "] " 
                  << current->data.name 
                  << " x" << current->data.quantity 
                  << " (ID: " << current->data.id << ")" << std::endl;
        current = current->next;
        index++;
    }
    std::cout << "==================\n" << std::endl;
}
