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

void shopMenu(Character* player) {
    while (true) {
        system("cls");
        cout << "\n=== 雜貨舖 ===\n";
        cout << "老闆: 歡迎！今天想買點什麼？ (持有金錢: " << player->getMoney() << ")\n";
        cout << "1. 小紅藥水 (HP+50)  - $20\n";
        cout << "2. 小藍藥水 (MP+30)  - $30\n";
        cout << "3. 鐵劍 (Atk+10)     - $100\n";
        cout << "4. 武器強化卷軸 - $50\n";
        cout << "5. 回城卷軸 (傳送) - $30\n";
        cout << "6. Exit\n";

        int choice;
        cin >> choice;

        if (choice == 6) break;

        Item* newItem = nullptr;

        switch (choice) {
        case 1:
            if (player->spendMoney(20)) newItem = new Consumable("小紅藥水", 20, 50, false);
            break;
        case 2:
            if (player->spendMoney(30)) newItem = new Consumable("小藍藥水", 30, 30, true);
            break;
        case 3:
            if (player->spendMoney(100)) newItem = new Equipment("鐵劍", 100, WEAPON, 10);
            break;
        case 4:
            if (player->spendMoney(50)) newItem = new Scroll("強化卷軸", 50, 2);
            break;
        case 5:
            if (player->spendMoney(30)) newItem = new Scroll("回城卷軸", 30, 1);
            break;
        }

        if (newItem != nullptr) {
            player->addItem(newItem);
            cout << "購買成功！\n";
            system("pause");
        }
        else if (choice >= 1 && choice <= 3) {
            cout << "金錢不足！\n";
            system("pause");
        }
    }
}
// 遊戲主迴圈
void runGameLoop(Character* player, MapSystem& mapSys) {
    bool isRunning = true;
    BattleSystem battleSys; // 建立戰鬥系統物件

    // 設定隨機數種子 (讓每次怪物、戰鬥結果都不同)
    srand(time(NULL));

    while (isRunning) {
        system("cls"); // 清空螢幕 (Windows 專用)

        // 1. 顯示介面
        player->showStats(); // 顯示角色狀態 (繼承自 Character)
        mapSys.displayMap(); // 顯示地圖
        mapSys.showCurrentInfo(); // 顯示當前位置資訊
        // 在 runGameLoop 裡

        cout << "[I] 背包 (Inventory)  [B] 商店/互動 (Buy)\n"; // 新增提示

        char key = _getch();

        if (key == 'i' || key == 'I') {
            // === 打開背包 ===
            system("cls");
            player->showInventory();
            cout << "\n輸入物品編號使用 (0 返回): ";
            int idx;
            cin >> idx;
            if (idx > 0) {
                player->useItem(idx - 1);
                system("pause");
            }
        }
        else if (key == 'b' || key == 'B') {
            // === 商店互動 ===
            // 只有在地圖節點是 SHOP 時才有效
            if (mapSys.getCurrentNodeType() == SHOP) {
                shopMenu(player);
            }
            else {
                cout << "\n這裡沒有商店。\n";
                system("pause"); // 讓玩家看到提示
            }
        }
        // ... 原本的移動邏輯 ...
        // 2. 玩家輸入 (不需按 Enter)
        // _getch() 會回傳按下的鍵值

        if (key == 'q' || key == 'Q') {
            // 離開遊戲
            break;
        }
        else if (key == 'w' || key == 's' || key == 'a' || key == 'd' ||
            key == 'W' || key == 'S' || key == 'A' || key == 'D') {

            // 3. 移動邏輯
            if (mapSys.movePlayer(key)) {
                // 移動成功後，可以在這裡判斷是否遇怪 (下個階段做)
                // if (mapSys.getCurrentNodeType() == WILD) { ... }
            }
            else {
                // 撞牆了，暫停一下讓玩家看到錯誤訊息
                cout << "按任意鍵繼續...";
                _getch();
            }
        }
        if (mapSys.movePlayer(key)) {
            // === 移動成功後，檢查是否遇怪 ===
            NodeType currentType = mapSys.getCurrentNodeType();

            if (currentType == WILD) {
                // 野外：30% 機率遇怪
                if (rand() % 100 < 30) {
                    // 隨機產生怪物 (II.i 生成怪物)
                    int rnd = rand() % 2;
                    if (rnd == 0) {
                        Monster m("長牙野豬", 1, 50, 8, 20, 10);
                        battleSys.startBattle(player, m);
                    }
                    else {
                        Monster m("灰狼", 2, 80, 12, 35, 20);
                        battleSys.startBattle(player, m);
                    }
                }
            }
            else if (currentType == BOSS) {
                // Boss 區：強制遇怪
                Monster boss("地獄守門犬", 10, 500, 30, 500, 1000);
                cout << "\n你感受到一股強大的殺氣...\n";
                Sleep(1000);
                battleSys.startBattle(player, boss);

                // 打贏 Boss 可以直接獲勝或退回上一格 (這裡暫時不處理)
            }
        }
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