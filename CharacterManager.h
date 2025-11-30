#pragma once
#include "Character.h"
#include <vector>
#include <fstream>

class CharacterManager {
private:
    string filePath = "data/characters.txt";
    vector<Character*> userCharacters; // 暫存目前登入使用者的角色列表

    // 載入所有角色的輔助函式 (回傳所有資料行的 vector)
    vector<string> readAllLines();

public:
    CharacterManager();
    ~CharacterManager(); // 解構子要負責釋放記憶體

    // 核心功能
    void loadUserCharacters(string username); // 讀取特定使用者的角色
    void createCharacter(string username);    // 創角
    void deleteCharacter(string username);    // 刪角
    void saveToFile();
    // 選單
    Character* selectCharacterMenu(string username);
};