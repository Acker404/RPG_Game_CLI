#include <iostream>
#include <clocale>
#include <conio.h> // 重要：用於偵測鍵盤輸入 (_getch)
#include "AccountSystem.h"
#include "CharacterManager.h"
#include "MapSystem.h" // 新增
#include "BattleSystem.h" // 新增
#include "Item.h"
using namespace std;
// 在 main.cpp 上方加入
#include "Item.h"
// 在 runGameLoop 之前，加入一個生成怪物的輔助函式
vector<Monster> generateMonstersForLocation(NodeType type) {
    vector<Monster> mobs;
	int moster_num = rand(); // 用於調整怪物屬性
    if (type == WILD) {
        // 隨機生成 1~3 隻
        int count = (rand() % 3) + 1;
		
        for (int i = 0; i < count; i++) {
            int rnd = rand() % 2;
            if (rnd == 0) mobs.push_back(Monster("長牙野豬", 1, 50 + ((moster_num % 50) / 4), 8 + ((moster_num % 8) / 4), 20 + ((moster_num % 20) / 4), 10 + ((moster_num % 10) / 4)));
            else mobs.push_back(Monster("灰狼", 2, 80 + ((moster_num % 80) / 4), 12 + +((moster_num % 12) / 4), 35 + ((moster_num % 35) / 4), 20 + ((moster_num % 20) / 4)));
        }
    }
    else if (type == BOSS) {
        // Boss 只有一隻
        mobs.push_back(Monster("地獄守門犬", 10, 500 + ((moster_num % 500) / 4), 30 + ((moster_num % 30) / 4), 500 + ((moster_num % 500) / 4), 1000 + ((moster_num % 1000) / 4)));
    }
    return mobs;
}
void shopMenu(Character* player) {
    while (true) {
        system("cls");
        cout << "\n=== 雜貨舖 ===\n";
        cout << "老闆: 歡迎！今天想買點什麼？ (持有金錢: " << player->getMoney() << ")\n";
        cout << "1. 小紅藥水 (HP+50)  - $20\n";
        cout << "2. 小藍藥水 (MP+30)  - $30\n";
        cout << "3. 鐵劍 (Atk+10)     - $80\n";
        cout << "4. 法杖 (wis+10)     - $80\n"; 
        cout << "5. 戒指 (lucky+5)     - $30\n";
        cout << "6. 衣服 (agi+5)     - $50\n";
        cout << "7. 武器強化卷軸 - $50\n";
        cout << "8. 回城卷軸 (傳送) - $30\n";
        cout << "0. Exit\n";

        int choice;
        cin >> choice;

        if (choice == 0) break;

        Item* newItem = nullptr;

        switch (choice) {
        case 1:
            if (player->spendMoney(20)) newItem = new Consumable("小紅藥水", 20, 50, false);
            break;
        case 2:
            if (player->spendMoney(30)) newItem = new Consumable("小藍藥水", 30, 30, true);
            break;
        case 3:
            if (player->spendMoney(80)) newItem = new Equipment("鐵劍", 80, WEAPON, 10);
            break;
        case 4:
            if (player->spendMoney(80)) newItem = new Equipment("法杖", 80, WEAPON, 10);
            break; 
        case 5:
            if (player->spendMoney(30)) newItem = new Equipment("戒指", 30, ACCESSORY, 5);
            break;
        case 6:
            if (player->spendMoney(50)) newItem = new Equipment("衣服", 50, ARMOR, 5);
            break;
        case 7:
            if (player->spendMoney(50)) newItem = new Scroll("強化卷軸", 50, 2);
            break;
        case 8:
            if (player->spendMoney(30)) newItem = new Scroll("回城卷軸", 30, 1);
            break;
        }

        if (newItem != nullptr) {
            player->addItem(newItem);
            cout << "購買成功！\n";
            system("pause");
        }
        else if (choice >= 1 && choice <= 8) {
            cout << "金錢不足！\n";
            system("pause");
        }
    }
}
// 遊戲主迴圈
void runGameLoop(Character* player, MapSystem& mapSys) {
    bool isRunning = true;
    BattleSystem battleSys;
    srand(time(NULL));

    // 當前地圖上的怪物列表
    vector<Monster> currentMonsters;

    // 剛進入遊戲，先生成一次 (如果出生點有怪的話)
    currentMonsters = generateMonstersForLocation(mapSys.getCurrentNodeType());

    while (isRunning) {
        system("cls");

        // 1. 顯示介面 (傳入怪物列表)
        player->showStats();
        mapSys.displayMap(currentMonsters); // <--- 改這裡
        mapSys.showCurrentInfo();

        cout << "[I] 背包  [B] 商店  [C] 角色狀態(配點)  [Q] 離開\n"; // 新增提示

        char key = _getch();

        if (key == 'c' || key == 'C') {
            // === 角色狀態與配點 ===
            system("cls");
            player->showStats(); // 這會顯示剩餘點數

            cout << "\n按 [Y] 進入配點模式，按其他鍵返回: ";
            char c2 = _getch();
            if (c2 == 'y' || c2 == 'Y') {
                player->allocateStats(); // 進入 Character 定義的配點迴圈
            }
        }
        // 2. 玩家輸入

        if (key == 'q' || key == 'Q') {
            break;
        }
        else if (key == 'w' || key == 's' || key == 'a' || key == 'd' ||
            key == 'W' || key == 'S' || key == 'A' || key == 'D') {

            // 移動玩家
            if (mapSys.movePlayer(key)) {
                // === 移動成功，生成新地點的怪物 ===
                // 如果是城鎮或商店，怪物清空；如果是野外，隨機生成
                NodeType type = mapSys.getCurrentNodeType();
                if (type == TOWN || type == SHOP) {
                    currentMonsters.clear();
                }
                else {
                    currentMonsters = generateMonstersForLocation(type);
                }
                if (rand() % 100 < 20) {
                    int healHp = 10; // 回復量
                    int healMp = 5;

                    // 只有當受傷時才顯示訊息，避免洗版
                    if (player->getHp() < 500 || player->getMp() < 200) { // 這裡數值只是大概，recover 會自動判斷上限
                        player->recover(healHp, healMp);
                        cout << "\n>>> [旅途休息] 邊走邊調整呼吸，氣色好多了 (HP+" << healHp << ", MP+" << healMp << ") <<<\n";
                        // 稍微暫停讓玩家看到 (可選)
                        // system("pause"); 
                    }
                }
            }
            else {
                cout << "無法移動！\n";
                system("pause"); // 稍微暫停讓玩家看到錯誤
            }
        }
        // === 新增：戰鬥選擇邏輯 ===
        else if (key >= '1' && key <= '3') {
            // 將 char '1' 轉為 int 0, '2' 轉為 1...
            int index = key - '1';

            if (index >= 0 && index < currentMonsters.size()) {
                // 開始戰鬥！
                Monster target = currentMonsters[index];
                battleSys.startBattle(player, target);

                // 戰鬥結束後，這隻怪物應該消失
                // (如果玩家逃跑或輸了，要看你的設計，這裡假設打贏就消失)
                if (target.isDead()) { // 注意：因為傳值呼叫，這裡 target 的狀態可能沒更新
                    // 簡單做法：只要戰鬥結束且玩家沒死，就當作贏了移除它
                    // 更嚴謹的做法是讓 BattleSystem 回傳結果
                    if (!player->isDead()) {
                        currentMonsters.erase(currentMonsters.begin() + index);
                    }
                }
                // 如果是 Boss 且打贏了，可能會有特殊劇情
            }
        }
        else if (key == 'i' || key == 'I') {
            // (背包邏輯保持不變...)
            system("cls");
            player->showInventory();
            cout << "\n輸入物品編號使用 (0 返回): ";
            int idx; cin >> idx;
            if (idx > 0) { player->useItem(idx - 1); system("pause"); }
        }
        else if (key == 'b' || key == 'B') {
            // (商店邏輯保持不變...)
            if (mapSys.getCurrentNodeType() == SHOP) shopMenu(player);
        }
        // === 檢查傳送旗標 ===
        if (player->getTeleportFlag()) {
            // 呼叫地圖系統的傳送功能
            mapSys.teleportToTown();

            // 記得把旗標關掉，不然會一直傳送
            player->setTeleportFlag(false);

            cout << ">>> 咻！你被傳送回到了城鎮中心。 <<<\n";
            system("pause"); // 讓玩家看到訊息
        }
		Sleep(100); // 小延遲，避免過快迴圈
    }
}
int main() {
    setlocale(LC_ALL, "");

    AccountSystem accountSys;
    CharacterManager charMgr;
    // MapSystem 在進入遊戲後再建立，或者在這裡建立

    int choice;

    while (true) {
        // ... (這部分保持不變：登入邏輯) ...
        if (!accountSys.getLoginStatus()) {
            cout << "\n=== 系統登入 ===\n1. 登入\n2. 註冊\n3. 離開\n請選擇: ";
            cin >> choice;
            if (choice == 1) accountSys.login();
            else if (choice == 2) accountSys.registerUser();
            else if (choice == 3) return 0;
        }
        else {
            string currentUser = accountSys.getCurrentUser();
            Character* selectedChar = charMgr.selectCharacterMenu(currentUser);

            if (selectedChar == nullptr) {
                accountSys.logout();
            }
            else {
                // === 進入遊戲世界 ===
                cout << "\n正在讀取地圖...\n";
                MapSystem mapSys; // 建立地圖系統

                // 啟動遊戲主迴圈
                runGameLoop(selectedChar, mapSys);
                charMgr.saveToFile();
                // 離開迴圈代表結束冒險，回到選單
                cout << "正在返回角色選單...\n";
                system("pause");
            }
        }
    }
    return 0;
}