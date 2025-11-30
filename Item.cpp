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
        Equipment* weapon = user->getWeapon();
        if (weapon) {
            weapon->enhance(); // 呼叫裝備的強化
            cout << "你的武器發出了耀眼的光芒！\n";
            return true;
        }
        else {
            cout << "你手上沒有武器，無法強化！\n";
            return false; // 沒用掉
        }
    }
    else if (effectType == 1) { // 傳送
        // 傳送邏輯比較特殊，通常在外部處理，這裡只回傳 true 讓它消失
        cout << "使用了回城卷軸！(若在戰鬥中則視為逃跑成功)\n";
        return true;
    }
    return false;
}