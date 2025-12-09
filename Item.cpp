#include "Item.h"
#include "Character.h" // 這裡才真正引入 Character 的定義

// Consumable 的實作
bool Consumable::use(Character* user) {
    if (isMp) {
        user->recover(0, healAmount);
        cout << "使用 " << name << "，恢復 MP " << healAmount << endl;
    }
    else {
        user->recover(healAmount, 0);
        cout << "使用 " << name << "，恢復 HP " << healAmount << endl;
    }
    return true; // 消耗掉
}

// Equipment 的 use 實作 (其實 Character 裡處理了，這裡只是備用)
bool Equipment::use(Character* user) {
    cout << "這是裝備，請在背包介面選擇使用來穿戴。\n";
    return false; // 不消耗 (穿戴邏輯由 Character::useItem 處理)
}

// Scroll 的實作
bool Scroll::use(Character* user) {
    if (effectType == 2) { // 強化
        // ... (原本的強化邏輯保持不變) ...
        user->enhanceWeapon();
        return true;
    }
    else if (effectType == 1) { // 傳送
        // 設定旗標，告訴主程式該傳送了
        user->setTeleportFlag(true);
        cout << "使用了回城卷軸！空間開始扭曲...\n";
        return true; // 消耗掉卷軸
    }
    return false;
}
// 簡單的物品工廠：根據名稱回傳對應的物件
Item* createItemByName(string name) {
    // === 消耗品 ===
    if (name == "小紅藥水") return new Consumable("小紅藥水", 20, 50, false);
    if (name == "小藍藥水") return new Consumable("小藍藥水", 30, 30, true);

    // === 裝備 ===
    if (name == "鐵劍") return new Equipment("鐵劍", 100, WEAPON, 10);
    // 如果你有更多裝備，請在這裡加

    // === 卷軸 ===
    if (name == "強化卷軸") return new Scroll("強化卷軸", 50, 2);
    if (name == "回城卷軸") return new Scroll("回城卷軸", 30, 1);

    return nullptr; // 找不到
}