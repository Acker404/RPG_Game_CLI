#include "CharacterManager.h"
#include <iostream>
#include <filesystem>
#include <direct.h> 
#include "Item.h" // 引入 ItemFactory

CharacterManager::CharacterManager() {
    _mkdir("./data");
}

CharacterManager::~CharacterManager() {
    for (auto c : userCharacters) delete c;
    userCharacters.clear();
}

vector<string> CharacterManager::readAllLines() {
    vector<string> lines;
    ifstream file(filePath);
    string line;
    while (getline(file, line)) {
        if (!line.empty()) lines.push_back(line);
    }
    return lines;
}

void CharacterManager::loadUserCharacters(string username) {
    // 清除舊資料
    for (auto c : userCharacters) delete c;
    userCharacters.clear();

    ifstream file(filePath);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        // 1. 跳過標題列
        if (line.find("帳號") == 0) continue;

        // 2. CSV 解析技巧：把逗號 ',' 替換成空格 ' '
        for (int i = 0; i < line.size(); i++) {
            if (line[i] == ',') line[i] = ' ';
        }

        stringstream ss(line);
        string u, n, j;
        int lv, hp, mhp, mp, mmp, s, w, a, l, sp, e, mon;

        // 讀取基礎數值
        ss >> u >> n >> j >> lv >> hp >> mhp >> mp >> mmp
            >> s >> w >> a >> l >> sp >> e >> mon;

        if (u == username) {
            Character* newChar = nullptr;
            if (j == "Warrior") newChar = new Warrior(u, n, lv);
            else if (j == "Mage") newChar = new Mage(u, n, lv);

            if (newChar) {
                newChar->setStats(hp, mhp, mp, mmp, s, w, a, l, sp, e);
                newChar->setMoney(mon);

                // 讀取裝備 (武器、防具、飾品)
                string weaponName, armorName, accessoryName;
                ss >> weaponName >> armorName >> accessoryName;

                if (weaponName != "NONE") {
                    Item* item = createItemByName(weaponName);
                    Equipment* equip = dynamic_cast<Equipment*>(item);
                    if (equip) newChar->equipItem(equip);
                }
                if (armorName != "NONE") {
                    Item* item = createItemByName(armorName);
                    Equipment* equip = dynamic_cast<Equipment*>(item);
                    if (equip) newChar->equipItem(equip);
                }
                if (accessoryName != "NONE") {
                    Item* item = createItemByName(accessoryName);
                    Equipment* equip = dynamic_cast<Equipment*>(item);
                    if (equip) newChar->equipItem(equip);
                }

                // === 這裡是你之前漏掉的：讀取背包 ===
                int invSize;
                ss >> invSize;
                for (int i = 0; i < invSize; i++) {
                    string itemName;
                    ss >> itemName;
                    Item* item = createItemByName(itemName);
                    if (item) newChar->addItem(item);
                }

                // === 這裡是你之前漏掉的：加入列表 ===
                userCharacters.push_back(newChar);
            }
        }
    }
}

void CharacterManager::createCharacter(string username) {
    loadUserCharacters(username);
    if (userCharacters.size() >= 3) {
        cout << "錯誤：角色已達上限 (3隻)。\n"; return;
    }

    string name;
    int jobChoice;
    cout << "\n=== 創建新角色 ===\n";
    cout << "請輸入角色名稱: "; cin >> name;

    for (auto c : userCharacters) {
        if (c->getName() == name) {
            cout << "錯誤：角色名稱已存在！\n"; return;
        }
    }

    cout << "請選擇職業:\n";
    cout << "1. 戰士 (Warrior) - 高血量、高防禦\n";
    cout << "2. 法師 (Mage)    - 高魔力、範圍傷害\n";
    cout << "請選擇: "; cin >> jobChoice;

    Character* newChar = nullptr;
    if (jobChoice == 1) newChar = new Warrior(username, name);
    else if (jobChoice == 2) newChar = new Mage(username, name);
    else {
        cout << "無效的選擇。\n"; return;
    }

    // 寫入檔案
    ifstream fCheck(filePath);
    bool isEmpty = (fCheck.peek() == ifstream::traits_type::eof());
    fCheck.close();

    ofstream file(filePath, ios::app);
    if (file.is_open()) {
        if (isEmpty) {
            file << "帳號,角色名稱,職業,等級,HP,MaxHP,MP,MaxMP,力量,智慧,敏捷,幸運,剩餘點數,經驗值,金錢,武器,防具,飾品,背包數量,背包物品..." << endl;
        }
        file << newChar->serialize() << endl;
        cout << "角色 " << name << " 創建成功！\n";
    }
    delete newChar;
}

void CharacterManager::deleteCharacter(string username) {
    loadUserCharacters(username);
    if (userCharacters.empty()) {
        cout << "無角色可刪除。\n"; return;
    }

    cout << "\n=== 刪除角色 ===\n";
    for (int i = 0; i < userCharacters.size(); i++) {
        cout << (i + 1) << ". ";
        userCharacters[i]->showStats();
    }
    cout << "請輸入要刪除的編號 (0 取消): ";
    int choice;
    cin >> choice;

    if (choice > 0 && choice <= userCharacters.size()) {
        string targetName = userCharacters[choice - 1]->getName();

        vector<string> allLines = readAllLines();
        ofstream file(filePath); // 覆寫模式

        for (string line : allLines) {
            // === 修正：這裡也要處理 CSV 逗號才能比對帳號 ===
            string tempLine = line;
            for (int i = 0; i < tempLine.size(); i++) if (tempLine[i] == ',') tempLine[i] = ' ';

            stringstream ss(tempLine);
            string u, n;
            ss >> u >> n;

            // 如果這行是當前使用者的角色，且名稱是要刪除的名稱，就跳過
            if (u == username && n == targetName) {
                continue;
            }
            file << line << endl;
        }
        cout << "角色已刪除。\n";
    }
}

void CharacterManager::saveToFile() {
    vector<string> allLines = readAllLines();
    vector<string> newLines;

    string currentUsername = "";
    if (!userCharacters.empty()) currentUsername = userCharacters[0]->getUsername();

    for (string line : allLines) {
        if (line.find("帳號") == 0) continue;

        string tempLine = line;
        for (int i = 0; i < tempLine.size(); i++) if (tempLine[i] == ',') tempLine[i] = ' ';
        stringstream ss(tempLine);
        string u;
        ss >> u;

        if (u != currentUsername && !u.empty()) {
            newLines.push_back(line);
        }
    }

    for (Character* c : userCharacters) {
        newLines.push_back(c->serialize());
    }

    ofstream file(filePath);
    file << "帳號,角色名稱,職業,等級,HP,MaxHP,MP,MaxMP,力量,智慧,敏捷,幸運,剩餘點數,經驗值,金錢,武器,防具,飾品,背包數量,背包物品..." << endl;

    for (string line : newLines) {
        file << line << endl;
    }
    cout << "遊戲資料已存檔 (CSV格式)。\n";
}

// selectCharacterMenu 保持你原本的即可，不用動
Character* CharacterManager::selectCharacterMenu(string username) {
    while (true) {
        loadUserCharacters(username);

        cout << "\n========================\n";
        cout << "   角色選擇選單 (" << userCharacters.size() << "/3)\n";
        cout << "========================\n";

        if (userCharacters.empty()) {
            cout << "(目前沒有角色)\n";
        }
        else {
            for (int i = 0; i < userCharacters.size(); i++) {
                cout << (i + 1) << ". ";
                userCharacters[i]->showStats();
            }
        }

        cout << "------------------------\n";
        cout << "C. 創建新角色 (Create)\n";
        cout << "D. 刪除角色 (Delete)\n";
        cout << "L. 登出 (Logout)\n";
        cout << "請選擇: ";

        string input;
        cin >> input;

        if (input == "C" || input == "c") {
            createCharacter(username);
        }
        else if (input == "D" || input == "d") {
            deleteCharacter(username);
        }
        else if (input == "L" || input == "l") {
            return nullptr;
        }
        else {
            try {
                int index = stoi(input);
                if (index > 0 && index <= userCharacters.size()) {
                    cout << "已選擇角色: " << userCharacters[index - 1]->getName() << "\n";
                    return userCharacters[index - 1];
                }
            }
            catch (...) {}
            cout << "無效輸入。\n";
        }
    }
}