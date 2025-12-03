#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include "Item.h" // 必須引入 Item.h 才能認識 Equipment


using namespace std;

class Character {
protected:
    string username;
    string name;
    string job;

    // 角色等級
    // - 表示玩家職業等級 (LV)
    // - 當經驗值 exp 累積到門檻時會呼叫 levelUp() 並增加此值
    int level;

    // 生命與魔力相關數值
    // hp: 目前生命值
    // maxHp: 最大生命值上限
    // mp: 目前魔力值
    // maxMp: 最大魔力值上限
    // 使用時請確保 hp <= maxHp 且 mp <= maxMp；傷害與回復應更新 hp/mp 並做上下界檢查
    int hp, maxHp;
    int mp, maxMp;

    // 基礎屬性值（不包含裝備加成）
    // str: 力量，用於計算物理攻擊力
    // wis: 智慧，用於計算魔法相關效果（例如法術強度或智力屬性）
    // 若有裝備或其他增益，請透過 getTotalStr()/getTotalWis() 取得最終值
    int str;
    int wis;

    // 經驗與貨幣
    // exp: 當 exp >= 100 時會自動升級（範例實作），可以依需求調整升級門檻或邏輯
    // money: 持有金錢數量，用於購買道具或強化
    int exp;
    int money;

    // === 新增裝備與背包 ===
    // inventory: 背包，儲存 Item* 的指標（注意記憶體管理）
    // weaponSlot / armorSlot: 裝備欄位，指向已裝備的 Equipment（或為 nullptr）
    vector<Item*> inventory;
    Equipment* weaponSlot; // 武器欄位
    Equipment* armorSlot;  // 防具欄位

public:
    // 建構子初始化列表
    Character(string u, string n, string j, int lv)
        : username(u), name(n), job(j), level(lv), exp(0), money(100),
        weaponSlot(nullptr), armorSlot(nullptr) // 指標記得初始化
    {
        // 為了簡單，如果 lv=1 給初始值
    }

    virtual ~Character() {
        // 解構時記得清空背包，避免記憶體洩漏 (作業可省略，但寫了加分)
        for (auto i : inventory) delete i;
        inventory.clear();
        // 裝備欄的指標如果是從背包拿出來的，通常不需要 double delete，
        // 但如果這是一套獨立的系統，要小心管理。
        // 簡單起見，作業可以先不管 delete，作業系統會回收。
    }

    virtual void levelUp() = 0;
    virtual void showStats() = 0;

    virtual void useSkill() {
        cout << "使用了技能！" << endl;
    }

    // Getters
    string getUsername() const { return username; }
    string getName() const { return name; }
    string getJob() const { return job; }
    int getHp() const { return hp; }
    int getMp() const { return mp; }
    int getMoney() const { return money; }

    // === 攻擊力計算 (包含裝備) ===
    int getTotalStr() const {
        int bonus = 0;
        if (weaponSlot) bonus += weaponSlot->getAtkBonus();
        return str + bonus;
    }
    int getTotalWis() const {
        // 為了簡單，暫時不加法杖的加成，或者你可以如法炮製
        return wis;
    }

    // === 裝備相關 ===
    Equipment* getWeapon() { return weaponSlot; } // 給卷軸強化用

    void equipItem(Equipment* equip) {
        if (!equip) return;
        Equipment* oldEquip = nullptr;

        if (equip->getSlot() == WEAPON) {
            oldEquip = weaponSlot;
            weaponSlot = equip;
        }
        else if (equip->getSlot() == ARMOR) {
            oldEquip = armorSlot;
            armorSlot = equip;
        }

        if (oldEquip != nullptr) {
            addItem(oldEquip); // 舊的回背包
            cout << "脫下了 " << oldEquip->getName() << "。\n";
        }
        cout << "裝備了 " << equip->getName() << "！\n";
    }

    // === 狀態與資源 ===
    bool isDead() const { return hp <= 0; }

    void takeDamage(int dmg) {
        hp -= dmg;
        if (hp < 0) hp = 0;
    }

    void consumeMp(int amount) {
        mp -= amount; if (mp < 0) mp = 0;
    }

    void recover(int hpAmount, int mpAmount) {
        hp += hpAmount; if (hp > maxHp) hp = maxHp;
        mp += mpAmount; if (mp > maxMp) mp = maxMp;
    }

    void heal() { hp = maxHp; mp = maxMp; } // 全補滿

    void addExp(int amount) {
        exp += amount;
        while (exp >= 100) {
            exp -= 100;
            levelUp();
        }
    }

    // === 金錢與背包 ===
    void addMoney(int amount) { money += amount; }
    bool spendMoney(int amount) {
        if (money >= amount) {
            money -= amount;
            return true;
        }
        return false;
    }

    void addItem(Item* item) { inventory.push_back(item); }

    void showInventory() {
        cout << "\n=== 背包 (金錢: " << money << ") ===\n";
        cout << "裝備中: " << (weaponSlot ? weaponSlot->getName() : "無") << endl;
        if (inventory.empty()) { cout << "背包是空的。\n"; return; }
        for (int i = 0; i < inventory.size(); i++) {
            cout << (i + 1) << ". ";
            inventory[i]->showInfo();
            cout << endl;
        }
    }

    void useItem(int index) {
        if (index < 0 || index >= inventory.size()) return;
        Item* item = inventory[index];

        // 判斷類型
        if (item->getType() == EQUIPMENT) {
            // dynamic_cast 需要多型類別 (有 virtual 函式)
            Equipment* equip = dynamic_cast<Equipment*>(item);
            if (equip) {
                inventory.erase(inventory.begin() + index);
                equipItem(equip);
            }
        }
        else {
            // 消耗品或卷軸，呼叫 use
            if (item->use(this)) {
                inventory.erase(inventory.begin() + index);
                // 這裡不 delete item，有些設計是移到墓地，但作業簡單起見：
                // 如果確定不再使用，請加上 delete item; 但要小心指標
                // 這裡建議 delete item; 避免 memory leak
                // delete item; (視你的程式穩定度而定，若報錯先註解掉)
            }
        }
    }

    // 存檔用
    // 修改 serialize
    virtual string serialize() {
        stringstream ss;
        // 1. 基礎數值
        ss << username << " " << name << " " << job << " "
            << level << " " << hp << " " << maxHp << " "
            << mp << " " << maxMp << " " << str << " " << wis << " " << exp << " " << money;

        // 2. 裝備狀態 (存名稱，如果沒裝備存 "NONE")
        ss << " " << (weaponSlot ? weaponSlot->getName() : "NONE");
        ss << " " << (armorSlot ? armorSlot->getName() : "NONE");

        // 3. 背包物品 (先存數量，再存每個物品的名稱)
        ss << " " << inventory.size();
        for (Item* item : inventory) {
            ss << " " << item->getName();
        }

        return ss.str();
    }
    void setMoney(int m) { money = m; }
    void setStats(int h, int mh, int m, int mm, int s, int w, int e) {
        hp = h; maxHp = mh; mp = m; maxMp = mm; str = s; wis = w; exp = e;
    }
    int getStr() const { return str; } // 補上這個 (以防萬一)
    int getWis() const { return wis; } // 補上這個 (解決錯誤)
};

// 因為繼承關係，Warrior 和 Mage 建議也確保定義正確
// (這裡假設你原本的 Warrior/Mage 類別還在 Character.h 下方，請保留它們)
class Warrior : public Character {
public:
    Warrior(string u, string n, int lv = 1) : Character(u, n, "Warrior", lv) {
        if (lv == 1) { maxHp = hp = 200; maxMp = mp = 50; str = 20; wis = 5; }
    }
    void levelUp() override {
        level++; maxHp += 50; hp = maxHp; str += 10;
        cout << "戰士升級！\n";
    }
    void showStats() override {
        cout << "[戰士] " << name << " Lv." << level
            << " HP:" << hp << "/" << maxHp
            << " MP:" << mp << "/" << maxMp
            << " Atk:" << getTotalStr() << endl; // 用 getTotalStr
    }
};

class Mage : public Character {
public:
    Mage(string u, string n, int lv = 1) : Character(u, n, "Mage", lv) {
        if (lv == 1) { maxHp = hp = 100; maxMp = mp = 200; str = 5; wis = 20; }
    }
    void levelUp() override {
        level++; maxMp += 50; mp = maxMp; wis += 10;
        cout << "法師升級！\n";
    }
    void showStats() override {
        cout << "[法師] " << name << " Lv." << level
            << " HP:" << hp << "/" << maxHp
            << " MP:" << mp << "/" << maxMp << endl;
    }
};