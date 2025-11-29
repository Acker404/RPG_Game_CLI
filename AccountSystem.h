#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <conio.h> // 用於 _getch() 做密碼隱藏輸入 (可選，但很加分)

using namespace std;

class AccountSystem {
private:
    string accountFile = "data/accounts.txt"; // 相對路徑：帳號資料庫
    bool isLoggedIn;
    string currentUser;

    // 輔助函式：檢查帳號是否已存在
    bool isAccountExist(string username);
    // 輔助函式：驗證帳密
    bool verifyCredentials(string username, string password);
    // 輔助函式：更新密碼到檔案
    void updatePasswordInFile(string username, string newPassword);

public:
    AccountSystem();

    // 核心功能
    bool login();           // 對應 VIII.i 登入
    void registerUser();    // 對應 VIII.i 註冊 (作業雖未明寫註冊細節，但要有帳號才能登入)
    void resetPassword();   // 對應 VIII.ii 忘記密碼

    // 取得當前狀態
    bool getLoginStatus() const { return isLoggedIn; }
    string getCurrentUser() const { return currentUser; }
    void logout();
};