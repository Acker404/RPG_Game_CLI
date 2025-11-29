#include "AccountSystem.h"
#include <filesystem> // C++17 標準，用於建立資料夾
#include <direct.h>   // 舊版 Windows 用 _mkdir

AccountSystem::AccountSystem() {
    isLoggedIn = false;
    currentUser = "";

    // 建構子執行時，先確保 data 資料夾存在
    _mkdir("./data");
}

bool AccountSystem::isAccountExist(string username) {
    ifstream file(accountFile);
    string u, p;
    while (file >> u >> p) {
        if (u == username) return true;
    }
    return false;
}

bool AccountSystem::verifyCredentials(string username, string password) {
    ifstream file(accountFile);
    string u, p;
    while (file >> u >> p) {
        if (u == username && p == password) return true;
    }
    return false;
}

void AccountSystem::updatePasswordInFile(string username, string newPassword) {
    // 讀出所有帳號
    vector<pair<string, string>> accounts;
    ifstream fileIn(accountFile);
    string u, p;
    while (fileIn >> u >> p) {
        if (u == username) p = newPassword; // 修改目標帳號的密碼
        accounts.push_back({ u, p });
    }
    fileIn.close();

    // 寫回檔案
    ofstream fileOut(accountFile);
    for (auto acc : accounts) {
        fileOut << acc.first << " " << acc.second << endl;
    }
}

void AccountSystem::registerUser() {
    string username, password;
    cout << "\n=== 註冊帳號 ===\n";
    cout << "輸入帳號: "; cin >> username;

    if (isAccountExist(username)) {
        cout << "錯誤: 帳號已存在！\n";
        return;
    }

    cout << "輸入密碼: "; cin >> password;

    ofstream file(accountFile, ios::app); // Append 模式
    if (file.is_open()) {
        file << username << " " << password << endl;
        cout << "註冊成功！請登入。\n";
    }
    else {
        cout << "錯誤: 無法寫入檔案 (請檢查 data 資料夾)。\n";
    }
}

bool AccountSystem::login() {
    string username, password;
    int errorCount = 0;

    while (true) {
        cout << "\n=== 系統登入 ===\n";
        cout << "輸入帳號: "; cin >> username;

        if (!isAccountExist(username)) {
            cout << "帳號不存在。\n";
            return false;
        }

        cout << "輸入密碼: "; cin >> password;

        if (verifyCredentials(username, password)) {
            cout << "登入成功！歡迎回來，" << username << "。\n";
            isLoggedIn = true;
            currentUser = username;
            return true;
        }
        else {
            errorCount++;
            cout << "密碼錯誤！(" << errorCount << "/3)\n";

            // 對應 VIII.ii 錯誤三次重設密碼
            if (errorCount >= 3) {
                cout << "\n!!! 警告：密碼錯誤次數過多 !!!\n";
                char choice;
                cout << "是否重設密碼？(y/n): "; cin >> choice;
                if (choice == 'y' || choice == 'Y') {
                    resetPassword();
                    errorCount = 0; // 重設後歸零
                }
                else {
                    return false;
                }
            }
        }
    }
}

void AccountSystem::resetPassword() {
    cout << "\n=== 重設密碼流程 ===\n";
    string username;
    cout << "請確認您的帳號: "; cin >> username;

    if (!isAccountExist(username)) {
        cout << "查無此帳號。\n";
        return;
    }

    // 這裡為了簡化，作業通常只要展示「輸入兩次新密碼」的邏輯
    // 實務上應該要驗證舊密碼或安全問題，但作業需求是"輸入錯誤三次提供重設"
    string newPass1, newPass2;
    while (true) {
        cout << "請輸入新密碼: "; cin >> newPass1;
        cout << "請再次輸入新密碼: "; cin >> newPass2;

        if (newPass1 == newPass2) {
            updatePasswordInFile(username, newPass1);
            cout << "密碼重設成功！請重新登入。\n";
            break;
        }
        else {
            cout << "兩次密碼不符，請重試。\n";
        }
    }
}

void AccountSystem::logout() {
    isLoggedIn = false;
    currentUser = "";
    cout << "已登出。\n";
}