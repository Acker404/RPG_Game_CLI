#include "CharacterManager.h"
#include <iostream>
#include "Item.h"
CharacterManager::CharacterManager() {
    // 建構子
}

CharacterManager::~CharacterManager() {
    // 釋放記憶體，避免 Memory Leak
    for (auto c : userCharacters) {
        delete c;
    }
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
    // ... (清除舊資料的代碼保持不變) ...
    for (auto c : userCharacters) delete c;
    userCharacters.clear();

    ifstream file(filePath);
    if (!file.is_open()) return;

    string line;
    // === 修改讀取邏輯 ===
    while (getline(file, line)) {
        if (line.empty()) continue;

        // 1. 跳過標題列 (如果開頭是 "帳號" 就跳過)
        // 或者是檢查是否含有中文字標題
        if (line.find("帳號") == 0) continue;

        // 2. CSV 解析技巧：把所有逗號 ',' 替換成空格 ' '
        // 這樣我們原本的 stringstream >> 邏輯就完全不用改！
        for (int i = 0; i < line.size(); i++) {
            if (line[i] == ',') line[i] = ' ';
        }

        stringstream ss(line);
        // ... (以下原本的讀取邏輯完全不用動) ...
        string u, n, j;
        int lv, hp, mhp, mp, mmp, s, w, a, l, sp, e, mon;

        ss >> u >> n >> j >> lv >> hp >> mhp >> mp >> mmp
            >> s >> w >> a >> l >> sp >> e >> mon;

        // ... (後面判斷 username 與 new Character 的邏輯保持不變) ...
        if (u == username) {
            // ... 記得保留你原本寫好的 setStats 等邏輯 ...
            Character* newChar = nullptr;
            if (j == "Warrior") newChar = new Warrior(u, n, lv);
            else if (j == "Mage") newChar = new Mage(u, n, lv);

            if (newChar) {
                newChar->setStats(hp, mhp, mp, mmp, s, w, a, l, sp, e);
                newChar->setMoney(mon);

                string weaponName, armorName;
                ss >> weaponName >> armorName;
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

                int invSize;
                ss >> invSize;
                for (int i = 0; i < invSize; i++) {
                    string itemName;
                    ss >> itemName;
                    Item* item = createItemByName(itemName);
                    if (item) newChar->addItem(item);
                }
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

    // 檢查名稱是否重複 (簡單檢查)
    for (auto c : userCharacters) {
        if (c->getName() == name) {
            cout << "錯誤：角色名稱已存在！\n";
            return;
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
        cout << "無效的選擇。\n";
        return;
    }
    // === 修改寫入部分 ===
    // 判斷檔案是否為空 (如果是新檔案，要先寫標題)
    ifstream fCheck(filePath);
    bool isEmpty = (fCheck.peek() == ifstream::traits_type::eof());
    fCheck.close();

    ofstream file(filePath, ios::app); // Append 模式
    if (file.is_open()) {
        if (isEmpty) {
            file << "帳號,角色名稱,職業,等級,HP,MaxHP,MP,MaxMP,力量,智慧,敏捷,幸運,剩餘點數,經驗值,金錢,武器,防具,背包數量,背包物品..." << endl;
        }
        file << newChar->serialize() << endl;
        cout << "角色 " << name << " 創建成功！\n";
    }
    delete newChar;
}



void CharacterManager::deleteCharacter(string username) {
    loadUserCharacters(username);
    if (userCharacters.empty()) {
        cout << "無角色可刪除。\n";
        return;
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

        // 讀取所有檔案內容，排除要刪除的那一行
        vector<string> allLines = readAllLines();
        ofstream file(filePath); // 覆寫模式

        for (const string& line : allLines) {
            stringstream ss(line);
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
                userCharacters[i]->showStats(); // 多型展示
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
            return nullptr; // 回傳 nullptr 表示登出
        }
        else {
            // 嘗試解析數字選擇
            try {
                int index = stoi(input);
                if (index > 0 && index <= userCharacters.size()) {
                    // 重要：這裡我們複製一份被選中的角色回傳，或者直接回傳指標
                    // 為了避免析構問題，我們這裡簡單回傳指標，
                    // 但要小心 CharacterManager 解構時會 delete 這些指標。
                    // 比較安全的作法是：被選中的角色，責任轉移給 GameEngine。

                    // 這裡我們採用 Clone 概念，或者簡單地讓外部接管指標
                    // 為求簡單，我們讓 Manager 保持擁有權，回傳指標僅供操作
                    cout << "已選擇角色: " << userCharacters[index - 1]->getName() << "\n";
                    return userCharacters[index - 1];
                }
            }
            catch (...) {}
            cout << "無效輸入。\n";
        }
    }
}
void CharacterManager::saveToFile() {
    // ... (讀取舊資料、篩選非當前使用者的邏輯保持不變) ...
    vector<string> allLines = readAllLines();
    vector<string> newLines;

    string currentUsername = "";
    if (!userCharacters.empty()) currentUsername = userCharacters[0]->getUsername();

    // 注意：讀取舊資料時，因為 allLines 是直接讀每一行
    // 我們要過濾掉舊的標題列，以免重複寫入
    for (string line : allLines) {
        if (line.find("帳號") == 0) continue; // 跳過舊標題

        // 解析這行是誰的 (需要先轉成空格處理才能讀 username)
        string tempLine = line;
        for (int i = 0; i < tempLine.size(); i++) if (tempLine[i] == ',') tempLine[i] = ' ';
        stringstream ss(tempLine);
        string u;
        ss >> u;

        if (u != currentUsername && !u.empty()) {
            newLines.push_back(line); // 保留其他人的 CSV 資料
        }
    }

    // 加入當前使用者的最新 CSV 資料
    for (Character* c : userCharacters) {
        newLines.push_back(c->serialize());
    }

    // === 寫入檔案 ===
    ofstream file(filePath); // 覆蓋模式

    // 1. 先寫入標題列 (CSV Header)
    file << "帳號,角色名稱,職業,等級,HP,MaxHP,MP,MaxMP,力量,智慧,敏捷,幸運,剩餘點數,經驗值,金錢,武器,防具,背包數量,背包物品..." << endl;

    // 2. 寫入資料
    for (string line : newLines) {
        file << line << endl;
    }
    cout << "遊戲資料已存檔 (CSV格式)。\n";
}