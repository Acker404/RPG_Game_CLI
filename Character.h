#pragma once
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

// 基礎角色類別
class Character {
protected:
    string username; // 所屬帳號
    string name;     // 角色名稱
    string job;      // 職業
    int level;
    int hp, maxHp;
    int mp, maxMp;
    int str; // 力量 (影響物理攻擊)
    int wis; // 智慧 (影響魔法攻擊)
    int exp;

public:
    Character(string u, string n, string j, int lv)
        : username(u), name(n), job(j), level(lv), exp(0) {
    }

    virtual ~Character() {} // 虛擬解構子，確保子類別正確銷毀

    // === 純虛擬函式 (Pure Virtual Function) ===
    // 強制子類別必須實作這些功能，這就是多型的精隨
    virtual void levelUp() = 0;
    virtual void showStats() = 0;

    // Getters
    string getUsername() const { return username; }
    string getName() const { return name; }
    string getJob() const { return job; }

    // 將資料轉為字串以便存檔 (格式: username name job level hp maxHp mp maxMp str wis exp)
    virtual string serialize() {
        stringstream ss;
        ss << username << " " << name << " " << job << " "
            << level << " " << hp << " " << maxHp << " "
            << mp << " " << maxMp << " " << str << " " << wis << " " << exp;
        return ss.str();
    }

    // 設定基礎數值 (給讀取檔案用)
    void setStats(int h, int mh, int m, int mm, int s, int w, int e) {
        hp = h; maxHp = mh; mp = m; maxMp = mm; str = s; wis = w; exp = e;
    }
};

// === 職業：戰士 ===
class Warrior : public Character {
public:
    Warrior(string u, string n, int lv = 1) : Character(u, n, "Warrior", lv) {
        // 戰士初始數值：高血量、高力量
        if (lv == 1) {
            maxHp = hp = 200;
            maxMp = mp = 50;
            str = 20;
            wis = 5;
        }
    }

    void levelUp() override {
        level++;
        maxHp += 50; hp = maxHp; // 升級補滿血
        str += 10;
        cout << "恭喜！" << name << " 升級成為 Lv." << level << " (戰士特性: 血量與力量大幅提升)\n";
    }

    void showStats() override {
        cout << "[" << job << "] " << name << " | Lv." << level
            << " | HP: " << hp << "/" << maxHp
            << " | STR: " << str << "\n";
    }
};

// === 職業：法師 ===
class Mage : public Character {
public:
    Mage(string u, string n, int lv = 1) : Character(u, n, "Mage", lv) {
        // 法師初始數值：低血量、高魔力、高智慧
        if (lv == 1) {
            maxHp = hp = 100;
            maxMp = mp = 200;
            str = 5;
            wis = 20;
        }
    }

    void levelUp() override {
        level++;
        maxMp += 50; mp = maxMp;
        wis += 10;
        cout << "恭喜！" << name << " 升級成為 Lv." << level << " (法師特性: 魔力與智慧大幅提升)\n";
    }

    void showStats() override {
        cout << "[" << job << "] " << name << " | Lv." << level
            << " | HP: " << hp << "/" << maxHp
            << " | MP: " << mp << "/" << maxMp
            << " | WIS: " << wis << "\n";
    }
};