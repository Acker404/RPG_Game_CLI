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
    // 先清空目前的列表
    for (auto c : userCharacters) delete c;
    userCharacters.clear();

    ifstream file(filePath);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string u, n, j;
        // 記得給預設值，這樣就算讀檔失敗，也不會變成負數亂碼
        int lv = 1, hp = 100, mhp = 100, mp = 50, mmp = 50, s = 10, w = 10, e = 0;
        int mon = 100; // 給個預設金錢 100

        // 1. 讀取基礎數值
        ss >> u >> n >> j >> lv >> hp >> mhp >> mp >> mmp >> s >> w >> e >> mon;

        if (u == username) {
            Character* newChar = nullptr;
            if (j == "Warrior") newChar = new Warrior(u, n, lv);
            else if (j == "Mage") newChar = new Mage(u, n, lv);

            if (newChar) {
                // 設定數值 & 金錢
                newChar->setStats(hp, mhp, mp, mmp, s, w, e);
                newChar->addMoney(mon - 100); // 因為建構子預設給100，所以用 add 加回去差異值，或者直接把 setStats 加個 money 參數
                // 更簡單的做法：Character 加一個 setMoney(mon)
                // 這裡假設你有 setMoney 或者直接用指標操作：
                // 為了方便，我們直接在 Character 加一個 public: void setMoney(int m) { money = m; }
                // 讀者請自行去 Character.h 補上 setMoney，或者用 addMoney 湊數
                // newChar->addMoney(mon - 100); // 暫時解法

                // 為了穩健，請在 Character.h 加入 void setMoney(int m) { money = m; }
                // 然後這裡呼叫：
                // newChar->setMoney(mon); 

                // 2. 讀取裝備
                string weaponName, armorName;
                ss >> weaponName >> armorName;

                if (weaponName != "NONE") {
                    Item* item = createItemByName(weaponName);
                    // 強制轉型並穿上
                    Equipment* equip = dynamic_cast<Equipment*>(item);
                    if (equip) newChar->equipItem(equip);
                }
                if (armorName != "NONE") {
                    Item* item = createItemByName(armorName);
                    Equipment* equip = dynamic_cast<Equipment*>(item);
                    if (equip) newChar->equipItem(equip);
                }

                // 3. 讀取背包
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
    loadUserCharacters(username); // 確保列表是最新的
    if (userCharacters.size() >= 3) {
        cout << "錯誤：角色已達上限 (3隻)，請先刪除舊角色。\n";
        return;
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

    // 寫入檔案 (Append 模式)
    ofstream file(filePath, ios::app);
    if (file.is_open()) {
        file << newChar->serialize() << endl;
        cout << "角色 " << name << " 創建成功！\n";
    }
    delete newChar; // 寫入後刪除暫存物件，下次 loadUserCharacters 會重新讀取
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
    // 這裡的邏輯比較麻煩：
    // 因為 userCharacters 只存了「當前使用者」的角色
    // 我們不能直接覆蓋檔案，否則「其他使用者」的角色會消失！

    // 正確邏輯：
    // 1. 讀取檔案中「所有」角色的字串 (readAllLines)
    // 2. 找出那些「不是當前使用者」的資料，保留下來
    // 3. 將「當前使用者」的最新資料 (userCharacters) 轉成字串加入
    // 4. 全部寫回檔案

    // 但這有點複雜，對於作業來說，我們可以用一個簡單的偷吃步：
    // 每次 loadUserCharacters 時，其實我們沒有保存其他人的資料。

    // === 簡易實作版 (假設這台電腦只有你一個人在測) ===
    // 或者，我們在 main 結束時，把當前角色的狀態更新進去。

    // 讓我們實作一個穩健的版本：

    // Step 1: 讀取舊檔所有資料
    vector<string> allLines = readAllLines();
    vector<string> newLines;

    // Step 2: 保留「非當前使用者」的資料
    // 我們需要知道當前 userCharacters 屬於哪個 username
    string currentUsername = "";
    if (!userCharacters.empty()) currentUsername = userCharacters[0]->getUsername();

    if (currentUsername == "") return; // 沒人登入，不用存

    for (string line : allLines) {
        stringstream ss(line);
        string u;
        ss >> u;
        if (u != currentUsername) {
            newLines.push_back(line);
        }
    }

    // Step 3: 加入當前使用者的最新資料
    for (Character* c : userCharacters) {
        newLines.push_back(c->serialize());
    }

    // Step 4: 寫回
    ofstream file(filePath); // 覆蓋模式
    for (string line : newLines) {
        file << line << endl;
    }
    cout << "遊戲資料已存檔。\n";
}