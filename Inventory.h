#ifndef INVENTORY_H
#define INVENTORY_H

#include "config.h"
#include <string>

// Node for the linked list
struct InventoryNode {
    Item data;
    InventoryNode* next;
    
    InventoryNode(Item item) : data(item), next(nullptr) {}
};

// LinkedList-based Inventory class
class Inventory {
private:
    InventoryNode* head;
    int itemCount;
    static const int MAX_INVENTORY = 20;

public:
    Inventory();
    ~Inventory();
    
    // Core inventory operations
    bool AddItem(Item newItem);
    bool RemoveItem(int itemID);
    bool RemoveItemByIndex(int index);
    Item GetItemByID(int itemID);
    Item GetItemByIndex(int index) const;
    
    // Utility functions
    int GetItemCount() const { return itemCount; }
    bool IsFull() const { return itemCount >= MAX_INVENTORY; }
    bool IsEmpty() const { return head == nullptr; }
    void Clear();
    void DisplayInventory() const;
    
    // Stack-like operations (for potions/consumables)
    bool UseItem(int itemID);  // Remove 1 quantity
    
    // Search and modify
    bool FindItem(int itemID) const;
    bool IncreaseQuantity(int itemID, int amount);
    bool DecreaseQuantity(int itemID, int amount);
    
    // Check for key
    bool HasKey() const;
    int GetItemQuantity(int itemID) const;
};

#endif
