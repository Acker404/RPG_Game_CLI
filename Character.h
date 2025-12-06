#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include "Item.h" // 必須引入 Item.h 才能認識 Equipment

using namespace std;

class Character {
protected:
    string username;
    string name;
    string job;
    int level;
    int exp;
    int money;

    // === 四大核心屬性 ===
    int str; // 力量 (影響 HP, 物攻)
    int wis; // 智慧 (影響 MP, 魔攻)
    int agi; // 敏捷 (影響 閃避)
    int luk; // 幸運 (影響 爆擊)
    int statPoints; // 升級未分配的點數

    // === 衍生數值 (由屬性計算得出) ===
    int hp, maxHp;
    int mp, maxMp;

    // === 背包與裝備 ===
    vector<Item*> inventory;
    Equipment* weaponSlot;
    Equipment* armorSlot;

public:
    Character(string u, string n, string j, int lv)
        : username(u), name(n), job(j), level(lv), exp(0), money(100),
        weaponSlot(nullptr), armorSlot(nullptr), statPoints(5) // <--- 改成 5
    {
        // 預設全屬性為 5
        str = wis = agi = luk = 5;
        maxHp = 50; maxMp = 20; hp = maxHp; mp = maxMp;
    }

    virtual ~Character() {
        // 清空背包
        for (auto i : inventory) delete i;
        inventory.clear();
        // 裝備部分如果是獨立物件也要考慮 delete，但在這個架構下裝備通常來自 ItemFactory 或 new，
        // 簡單起見由系統回收或這裡不處理指標 (避免 double free 如果它是從 inventory 移過來的)
    }

    // === 核心：屬性計算公式 ===
    void updateDerivedStats() {
        int calculatedMaxHp = (str * 10) + 50;
        int calculatedMaxMp = (wis * 10) + 20;

        maxHp = calculatedMaxHp;
        maxMp = calculatedMaxMp;

        if (hp > maxHp) hp = maxHp;
        if (mp > maxMp) mp = maxMp;
    }

    // === 升級系統 ===
    void addExp(int amount) {
        exp += amount;
        int reqExp = level * 100;
        while (exp >= reqExp) {
            exp -= reqExp;
            levelUp();
            reqExp = level * 100;
        }
    }

    virtual void levelUp() {
        level++;
        statPoints += 5;
        cout << "\n>>> 恭喜升級！等級提升為 " << level << " <<<\n";
        cout << ">>> 獲得 5 點屬性點，請至 [C] 角色選單 進行配點！ <<<\n";
        updateDerivedStats();
        heal();
    }

    // === 配點功能 ===
    void allocateStats() {
        while (statPoints > 0) {
            system("cls");
            cout << "=== 屬性配點 (剩餘點數: " << statPoints << ") ===\n";
            cout << "1. 力量 (STR): " << str << " (影響血量、物攻)\n";
            cout << "2. 智慧 (WIS): " << wis << " (影響魔力、魔攻)\n";
            cout << "3. 敏捷 (AGI): " << agi << " (影響閃避率)\n";
            cout << "4. 幸運 (LUK): " << luk << " (影響爆擊率)\n";
            cout << "0. 結束配點\n";
            cout << "請選擇要提升的屬性: ";

            int choice;
            cin >> choice;

            if (choice == 0) break;

            switch (choice) {
            case 1: str++; break;
            case 2: wis++; break;
            case 3: agi++; break;
            case 4: luk++; break;
            default: continue;
            }
            statPoints--;
            updateDerivedStats();
            cout << "屬性已提升！\n";
            system("pause");
        }
    }

    // === Getters ===
    string getUsername() const { return username; }
    string getName() const { return name; }
    string getJob() const { return job; }
    int getHp() const { return hp; }
    int getMp() const { return mp; }
    int getMoney() const { return money; }

    int getStr() const { return str; }
    int getWis() const { return wis; }
    int getAgi() const { return agi; }
    int getLuk() const { return luk; }

    // 計算總攻擊力 (基礎 + 裝備)
    int getTotalStr() const {
        int bonus = (weaponSlot ? weaponSlot->getAtkBonus() : 0);
        return str + bonus;
    }

    int getDodgeRate() const { return agi / 2; }
    int getCritRate() const { return luk / 2; }

    // === 狀態與資源 ===
    bool isDead() const { return hp <= 0; }
    void takeDamage(int dmg) { hp -= dmg; if (hp < 0) hp = 0; }
    void consumeMp(int amount) { mp -= amount; if (mp < 0) mp = 0; }

    void recover(int hpAmount, int mpAmount) {
        hp += hpAmount; if (hp > maxHp) hp = maxHp;
        mp += mpAmount; if (mp > maxMp) mp = maxMp;
    }

    void heal() { hp = maxHp; mp = maxMp; }

    // === 金錢與背包 ===
    void addMoney(int amount) { money += amount; }
    void setMoney(int m) { money = m; }
    bool spendMoney(int amount) {
        if (money >= amount) { money -= amount; return true; }
        return false;
    }

    void addItem(Item* item) { inventory.push_back(item); }

    // 補回缺失的 showInventory
    void showInventory() {
        cout << "\n=== 背包 (金錢: " << money << ") ===\n";
        cout << "裝備中武器: " << (weaponSlot ? weaponSlot->getName() : "無") << endl;
        cout << "裝備中防具: " << (armorSlot ? armorSlot->getName() : "無") << endl;

        if (inventory.empty()) {
            cout << "背包是空的。\n";
            return;
        }
        for (int i = 0; i < inventory.size(); i++) {
            cout << (i + 1) << ". ";
            inventory[i]->showInfo();
            cout << endl;
        }
    }

    // 補回缺失的 useItem
    void useItem(int index) {
        if (index < 0 || index >= inventory.size()) return;
        Item* item = inventory[index];

        if (item->getType() == EQUIPMENT) {
            Equipment* equip = dynamic_cast<Equipment*>(item);
            if (equip) {
                inventory.erase(inventory.begin() + index);
                equipItem(equip);
            }
        }
        else {
            if (item->use(this)) {
                inventory.erase(inventory.begin() + index);
                // delete item; // 避免 crash 若有指標問題先註解，穩定後可打開
            }
        }
    }

    // === 裝備操作 ===
    Equipment* getWeapon() { return weaponSlot; }

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
        updateDerivedStats(); // 重新計算數值
    }

    // 補回缺失的 useSkill
    virtual void useSkill() {
        cout << "使用了技能！(基礎)" << endl;
    }

    // 純虛擬函式
    virtual void showStats() = 0;

    // === 存檔與讀檔 ===
    virtual string serialize() {
        stringstream ss;
        ss << username << " " << name << " " << job << " "
            << level << " " << hp << " " << maxHp << " "
            << mp << " " << maxMp << " "
            << str << " " << wis << " " << agi << " " << luk << " " << statPoints << " "
            << exp << " " << money;

        ss << " " << (weaponSlot ? weaponSlot->getName() : "NONE");
        ss << " " << (armorSlot ? armorSlot->getName() : "NONE");

        ss << " " << inventory.size();
        for (Item* item : inventory) {
            ss << " " << item->getName();
        }

        return ss.str();
    }

    void setStats(int h, int mh, int m, int mm, int s, int w, int a, int l, int sp, int e) {
        hp = h; maxHp = mh; mp = m; maxMp = mm;
        str = s; wis = w; agi = a; luk = l; statPoints = sp;
        exp = e;
    }
};

// === 職業定義 (確保建構子正確呼叫 updateDerivedStats) ===
class Warrior : public Character {
public:
    Warrior(string u, string n, int lv = 1) : Character(u, n, "Warrior", lv) {
        if (lv == 1) {
            str = 15; wis = 5; agi = 10; luk = 5;
            updateDerivedStats(); // 計算 HP/MP
            hp = maxHp; mp = maxMp; // 補滿
        }
    }
    void showStats() override {
        cout << "\n=== 戰士狀態表 ===\n";
        cout << "名稱: " << name << " (Lv." << level << ")\n";
        cout << "HP: " << hp << "/" << maxHp << "  MP: " << mp << "/" << maxMp << "\n";
        cout << "力量: " << str << "  智慧: " << wis << "\n";
        cout << "敏捷: " << agi << "  幸運: " << luk << "\n";
        cout << "攻擊力: " << getTotalStr() << "  剩餘點數: " << statPoints << "\n";
        cout << "==================\n";
    }
};

class Mage : public Character {
public:
    Mage(string u, string n, int lv = 1) : Character(u, n, "Mage", lv) {
        if (lv == 1) {
            str = 5; wis = 20; agi = 5; luk = 10;
            updateDerivedStats();
            hp = maxHp; mp = maxMp;
        }
    }
    void showStats() override {
        cout << "\n=== 法師狀態表 ===\n";
        cout << "名稱: " << name << " (Lv." << level << ")\n";
        cout << "HP: " << hp << "/" << maxHp << "  MP: " << mp << "/" << maxMp << "\n";
        cout << "力量: " << str << "  智慧: " << wis << "\n";
        cout << "敏捷: " << agi << "  幸運: " << luk << "\n";
        cout << "攻擊力: " << getTotalStr() << "  剩餘點數: " << statPoints << "\n";
        cout << "==================\n";
    }
};