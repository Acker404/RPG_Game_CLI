#pragma once
#include "Character.h"
#include "Monster.h"
#include <windows.h> // 為了使用 Sleep()
#include <iostream>
#include <conio.h> // 為了 _getch()

class BattleSystem {
public:
    // 開始戰鬥 (傳入玩家與怪物)
    void startBattle(Character* player, Monster monster);

private:
    // 玩家回合
    void playerTurn(Character* player, Monster& monster, bool& escaped);
    // 怪物回合
    void enemyTurn(Character* player, Monster& monster);
    // 戰鬥結算
    void battleResult(Character* player, Monster& monster);
};