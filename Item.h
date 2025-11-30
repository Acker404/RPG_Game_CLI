#pragma once
#include <string>
#include <iostream>

using namespace std;

// 前向宣告：告訴編譯器 Character 是一個類別，細節等下在 .cpp 才會用到
class Character;

enum ItemType { CONSUMABLE, EQUIPMENT, SCROLL };
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

    string getName() const { return name; }
    int getPrice() const { return price; }
    ItemType getType() const { return type; }

    // 純虛擬函式
    virtual bool use(Character* user) = 0;

    virtual void showInfo() const {
        cout << "[" << name << "] $" << price << " - " << description;
    }
};

// === 消耗品 ===
class Consumable : public Item {
private:
    int healAmount;
    bool isMp;

public:
    Consumable(string n, int price, int amount, bool mp = false)
        : Item(n, mp ? "恢復魔力" : "恢復生命", price, CONSUMABLE), healAmount(amount), isMp(mp) {
    }

    // 注意：這裡只寫宣告，不寫實作，因為這時候 Character 還沒定義完整
    bool use(Character* user) override;
};

// === 裝備 ===
class Equipment : public Item {
protected: // 改成 protected 讓子類別或 friend 可以存取，或者加 getter
    EquipSlot slot;
    int atkBonus;

public:
    Equipment(string n, int price, EquipSlot s, int atk)
        : Item(n, "裝備後提升能力", price, EQUIPMENT), slot(s), atkBonus(atk) {
    }

    // 裝備無法直接使用，需透過裝備欄
    bool use(Character* user) override;

    int getAtkBonus() const { return atkBonus; }
    EquipSlot getSlot() const { return slot; }

    // 強化功能
    void enhance() {
        atkBonus += 5;
        price += 50;
        cout << "裝備 [" << name << "] 強化成功！目前攻擊加成: " << atkBonus << endl;
    }

    void showInfo() const override {
        string slotName = (slot == WEAPON ? "武器" : "防具");
        cout << "[" << slotName << "] " << name << " (Atk+" << atkBonus << ") $" << price;
    }
};

// === 卷軸 ===
class Scroll : public Item {
private:
    int effectType; // 1: 傳送, 2: 強化

public:
    Scroll(string n, int price, int type)
        : Item(n, "特殊魔法卷軸", price, SCROLL), effectType(type) {
    }

    // 同樣只宣告，實作放 .cpp
    bool use(Character* user) override;
};