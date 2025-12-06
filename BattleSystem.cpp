#include "BattleSystem.h"

void BattleSystem::startBattle(Character* player, Monster monster) {
    system("cls");
    cout << "\n!!! 遭遇敵人 !!!\n";
    monster.showInfo();
    Sleep(1000); // 延遲讓玩家看清楚

    bool escaped = false;

    // 戰鬥迴圈：只要雙方都活著，且沒逃跑，就繼續打
    while (!player->isDead() && !monster.isDead() && !escaped) {
        system("cls");
        // 顯示雙方狀態
        cout << "\n================ 戰鬥中 ================\n";
        player->showStats();
        cout << "\n       VS       \n\n";
        monster.showInfo();
        cout << "========================================\n";

        // 1. 玩家回合
        playerTurn(player, monster, escaped);

        // 如果怪物死了或逃跑成功，結束迴圈
        if (monster.isDead() || escaped) break;

        // 2. 怪物回合
        cout << "\n輪到 " << monster.name << " 攻擊了...\n";
        Sleep(800);
        enemyTurn(player, monster);

        // 如果玩家死了，結束迴圈
        if (player->isDead()) break;

        cout << "\n按任意鍵進入下一回合...";
        _getch();
    }

    // 戰鬥結束處理
    if (!escaped) {
        battleResult(player, monster);
    }
}
void BattleSystem::playerTurn(Character* player, Monster& monster, bool& escaped) {
    cout << "\n[你的行動]\n1. 攻擊\n2. 技能\n3. 逃跑\n請選擇: ";
    char choice = _getch();
    int damage = 0;

    switch (choice) {
    case '1': // 普通攻擊
        // === 幸運檢定：爆擊判定 ===
        if (rand() % 100 < player->getCritRate()) {
            cout << "\n[CRITICAL!] 幸運爆發！出現了致命一擊！\n";
            damage = (player->getTotalStr() * 1.5) + (rand() % 5); // 1.5倍傷害
        }
        else {
            damage = player->getTotalStr() + (rand() % 5);
        }

        cout << "\n你攻擊了 " << monster.name << "！\n";
        Sleep(500);
        monster.takeDamage(damage);
        cout << ">>> 造成了 " << damage << " 點傷害！\n";
        break;
    case '2': // 技能
        if (player->getJob() == "Warrior") {
            // 戰士技能：強力一擊 (消耗 MP 10)
            if (player->getMp() >= 10) {
                player->useSkill(); // 顯示技能文字
                damage = player->getTotalStr() + (rand() % 5) * 2; // 兩倍傷害
                player->consumeMp(10);
                monster.takeDamage(damage);
                cout << ">>> 造成了 " << damage << " 點暴擊傷害！\n";
            }
            else {
                cout << "魔力不足！改為普通攻擊。\n";
                damage = player->getTotalStr() + (rand() % 5);
                monster.takeDamage(damage);
                cout << ">>> 造成了 " << damage << " 點傷害！\n";
            }
        }
        else if (player->getJob() == "Mage") {
            // 法師技能：火球術 (消耗 MP 20)
            if (player->getMp() >= 20) {
                player->useSkill();
                damage = player->getWis() * 3; // 智力3倍傷害
                player->consumeMp(20);
                monster.takeDamage(damage);
                cout << ">>> 造成了 " << damage << " 點魔法傷害！\n";
            }
            else {
                cout << "魔力不足！改為法杖敲擊。\n";
                damage = player->getTotalStr() + (rand() % 5); // 法師普攻很弱

                monster.takeDamage(damage);
                cout << ">>> 造成了 " << damage << " 點傷害！\n";
            }
        }
        break;

    case '3': // 逃跑
        cout << "\n你試圖逃跑...\n";
        Sleep(1000);
        if (rand() % 100 < 50) { // 50% 機率
            cout << "逃跑成功！\n";
            escaped = true;
        }
        else {
            cout << "逃跑失敗！被怪物絆倒了。\n";
        }
        break;

    default:
        cout << "發呆錯過了機會...\n";
        break;
    }
}

void BattleSystem::enemyTurn(Character* player, Monster& monster) {
    cout << "\n輪到 " << monster.name << " 攻擊了...\n";
    Sleep(800);

    // === 敏捷檢定：閃避判定 ===
    // 玩家閃避率 = Agi / 2 (例如 20 敏捷 = 10% 閃避)
    // 這裡設個上限，例如最高 50%
    int dodgeChance = player->getDodgeRate();
    if (dodgeChance > 50) dodgeChance = 50;

    if (rand() % 100 < dodgeChance) {
        cout << ">>> [MISS] 你的身手太敏捷了，完全閃過了 " << monster.name << " 的攻擊！\n";
    }
    else {
        int dmg = monster.attack + (rand() % 3);
        player->takeDamage(dmg);
        cout << ">>> " << monster.name << " 對你造成了 " << dmg << " 點傷害！\n";
        cout << "你的剩餘血量: " << player->getHp() << endl;
    }
}

void BattleSystem::battleResult(Character* player, Monster& monster) {
    Sleep(1000);
    system("cls");

    if (player->isDead()) {
        cout << "\n================================\n";
        cout << "   你已經被打敗了... (GAME OVER)   \n";
        cout << "================================\n";
        // 這裡可以做死亡懲罰，例如掉錢或回城
        player->heal(); // 暫時：復活補滿血
        cout << "幸運的是，你被路過的冒險者救回城鎮。\n";
    }
    else {
        cout << "\n================================\n";
        cout << "           VICTORY!             \n";
        cout << "================================\n";
        cout << "獲得經驗值: " << monster.expReward << "\n";
        cout << "獲得金錢: " << monster.moneyReward << "\n";

        player->addExp(monster.expReward);     // 增加經驗值
        player->addMoney(monster.moneyReward); // <--- 補上這一行！真正把錢加進去
    }
    system("pause");
}