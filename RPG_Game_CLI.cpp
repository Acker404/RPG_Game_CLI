#include <iostream>
#include "AccountSystem.h"

using namespace std;

int main() {

    AccountSystem accountSys;
    int choice;

    while (true) {
        if (!accountSys.getLoginStatus()) {
            // === 未登入狀態選單 ===
            cout << "\n========================\n";
            cout << "   RPG 遊戲 - 登入系統   \n";
            cout << "========================\n";
            cout << "1. 登入 (Login)\n";
            cout << "2. 註冊 (Register)\n";
            cout << "3. 離開 (Exit)\n";
            cout << "請選擇: ";
            cin >> choice;

            switch (choice) {
            case 1:
                accountSys.login();
                break;
            case 2:
                accountSys.registerUser();
                break;
            case 3:
                return 0;
            default:
                cout << "無效輸入。\n";
            }
        }
        else {
            // === 已登入狀態 (未來這裡是遊戲主選單) ===
            cout << "\n========================\n";
            cout << "   " << accountSys.getCurrentUser() << " 的遊戲大廳\n";
            cout << "========================\n";
            cout << "1. 開始遊戲 (Start Game) - [開發中]\n";
            cout << "2. 角色管理 (Character) - [開發中]\n";
            cout << "3. 登出 (Logout)\n";
            cout << "請選擇: ";
            cin >> choice;

            switch (choice) {
            case 1:
                cout << "進入遊戲世界... (功能尚未實作)\n";
                break;
            case 2:
                cout << "進入角色創建介面... (功能尚未實作)\n";
                break;
            case 3:
                accountSys.logout();
                break;
            default:
                cout << "無效輸入。\n";
            }
        }
    }
    return 0;
}