#pragma once
#include <string>
#include <iostream>

using namespace std;

class Monster {
public:
    string name;
    int level;
    int hp;
    int maxHp;
    int attack; // 攻擊力
    int expReward; // 掉落經驗值
    int moneyReward; // 掉落金錢 (為商店功能鋪路)

    // 建構子
    Monster(string n, int lv, int h, int atk, int exp, int money)
        : name(n), level(lv), hp(h), maxHp(h), attack(atk), expReward(exp), moneyReward(money) {
    }

    // 判斷是否存活
    bool isDead() const { return hp <= 0; }

    // 受到傷害
    void takeDamage(int dmg) {
        hp -= dmg;
        if (hp < 0) hp = 0;
    }

    // 顯示怪物資訊 (II.ii 要求)
    void showInfo() const {
        cout << "------------------------\n";
        cout << "【怪物】" << name << " (Lv." << level << ")\n";
        cout << " HP: " << hp << " / " << maxHp << "\n";
        cout << " 攻擊力: " << attack << "\n";
        cout << "------------------------\n";
    }
};