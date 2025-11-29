#include <iostream>
#include <clocale>
#include <conio.h> // 重要：用於偵測鍵盤輸入 (_getch)
#include "AccountSystem.h"
#include "CharacterManager.h"
#include "MapSystem.h" // 新增
#include "BattleSystem.h" // 新增
using namespace std;

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

        // 2. 玩家輸入 (不需按 Enter)
        // _getch() 會回傳按下的鍵值
        char key = _getch();

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

                // 離開迴圈代表結束冒險，回到選單
                cout << "正在返回角色選單...\n";
                system("pause");
            }
        }
    }
    return 0;
}