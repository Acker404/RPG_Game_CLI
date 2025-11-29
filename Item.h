#pragma once
#include <string>
#include <iostream>

using namespace std;

// 物品類型
enum ItemType { CONSUMABLE, EQUIPMENT, SCROLL };

// 裝備部位 (對應 III.i 道具部位)
enum EquipSlot { NONE, WEAPON, ARMOR, ACCESSORY };

class Item {
protected:
    string name;
    string description;
    int price;
    ItemType type;

public:
    Item(string n, string desc, int p, ItemType t)
        : name(n), description(desc), price(p), type(t) {
    }

    virtual ~Item() {}

    // Getters
    string getName() const { return name; }
    int getPrice() const { return price; }
    ItemType getType() const { return type; }

    // 純虛擬函式：每個物品的使用效果不同
    // 回傳 true 代表使用成功 (要扣掉)，false 代表沒用掉
    virtual bool use(class Character* user) = 0;

    virtual void showInfo() const {
        cout << "[" << name << "] $" << price << " - " << description;
    }
};

// === 消耗品類別 (藥水) ===
#include "Character.h" // 因為要操作 Character，所以要 include
// 注意：如果在 Character.h 也 include Item.h 會造成循環引用
// 解法：在 Item.h 上方用 forward declaration "class Character;" (我已經寫在上面了)
// 但實作 use 時需要完整定義，所以通常把實作放在 .cpp 或確保順序
// 為了簡單，我們這裡假設 Character 有 heal() 方法

class Consumable : public Item {
private:
    int healAmount; // 恢復量
    bool isMp;      // 是補魔嗎？

public:
    Consumable(string n, int price, int amount, bool mp = false)
        : Item(n, mp ? "恢復魔力" : "恢復生命", price, CONSUMABLE), healAmount(amount), isMp(mp) {
    }

    bool use(Character* user) override;
};

// === 裝備類別 ===
class Equipment : public Item {
private:
    EquipSlot slot; // 部位
    int atkBonus;   // 攻擊加成
    int defBonus;   // 防禦加成 (未來可擴充)

public:
    Equipment(string n, int price, EquipSlot s, int atk)
        : Item(n, "裝備後提升能力", price, EQUIPMENT), slot(s), atkBonus(atk), defBonus(0) {
    }

    bool use(Character* user) override {
        // 裝備的使用邏輯就是「穿上」
        // 這部分比較複雜，涉及換裝，我們先回傳 false (不消耗)
        // 之後由 Character 的 equip() 函式處理
        cout << "這是裝備，請在裝備欄進行穿戴。\n";
        return false;
    }

    int getAtkBonus() const { return atkBonus; }
    EquipSlot getSlot() const { return slot; }

    void showInfo() const override {
        string slotName = (slot == WEAPON ? "武器" : "防具");
        cout << "[" << slotName << "] " << name << " (Atk+" << atkBonus << ") $" << price;
    }
};