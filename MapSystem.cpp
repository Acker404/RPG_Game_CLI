#pragma once
#include "MapSystem.h"
#include <iomanip> // 用於排版

MapSystem::MapSystem() {
    width = 5;
    height = 5;
    playerX = 2; // 初始在中間
    playerY = 2;

    // 初始化 5x5 空白地圖
    mapGrid.resize(height, vector<MapNode>(width));
    initMap();
}

void MapSystem::initMap() {
    // 先把所有地方設為牆壁 (WALL)
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            mapGrid[y][x] = MapNode("   X   ", WALL, "這裡是無法通行的區域。");

    // === 設定地圖節點 (參考你的圖片結構) ===
    // (x, y) 座標系：x是橫向，y是縱向，(0,0)在左上角

    // 中心：城鎮
    mapGrid[2][2] = MapNode(" 城鎮中心 ", TOWN, "安全的地方，周圍有許多冒險者。");

    // 北邊：森林
    mapGrid[1][2] = MapNode(" 郊外草原 ", WILD, "這裡有些弱小的動物。");
    mapGrid[0][2] = MapNode(" 黑森林 ", WILD, "陰森的森林，感覺有危險。"); // Boss前置

    // 西邊：商店
    mapGrid[2][1] = MapNode("  雜貨舖  ", SHOP, "可以在這裡買賣物品。");

    // 東邊：野外
    mapGrid[2][3] = MapNode("  荒野  ", WILD, "一片荒蕪，小心強盜。");
    mapGrid[2][4] = MapNode(" 廢棄礦坑 ", BOSS, "傳說中有巨大的怪物棲息。"); // Boss

    // 南邊
    mapGrid[3][2] = MapNode("  城堡  ", TOWN, "莊嚴的城堡，守衛森嚴。");
}

bool MapSystem::movePlayer(char direction) {
    int newX = playerX;
    int newY = playerY;

    // 處理輸入
    switch (direction) {
    case 'w': case 'W': newY--; break; // 向上
    case 's': case 'S': newY++; break; // 向下
    case 'a': case 'A': newX--; break; // 向左
    case 'd': case 'D': newX++; break; // 向右
    default: return false;
    }

    // 1. 檢查邊界
    if (newX < 0 || newX >= width || newY < 0 || newY >= height) {
        cout << "\n[系統] 前方沒有路了！\n";
        return false;
    }

    // 2. 檢查是否為牆壁
    if (mapGrid[newY][newX].type == WALL) {
        cout << "\n[系統] 那邊無法通行 (障礙物)！\n";
        return false;
    }

    // 更新位置
    playerX = newX;
    playerY = newY;
    return true;
}

void MapSystem::displayMap(const vector<Monster>& monsters) {
    // 設定欄位寬度常數，方便調整
    const int MAP_CELL_WIDTH = 10; // 地圖每一格的寬度 (因中文字寬度問題，建議設寬一點)
    const int MONSTER_NAME_WIDTH = 14;

    cout << "\n";
    cout << "      === 世界地圖 ===                  === 周邊怪物 ===\n\n";

    for (int y = 0; y < height; y++) {
        // === 1. 畫地圖左半邊 ===
        cout << "  "; // 最左邊縮排
        for (int x = 0; x < width; x++) {
            // 準備要顯示的字串
            string cellDisplay;
            if (x == playerX && y == playerY) {
                cellDisplay = "[我]";
            }
            else {
                if (mapGrid[y][x].type == WALL) cellDisplay = "| . . . |";
                else cellDisplay = mapGrid[y][x].name;
            }

            // 使用 setw 設定寬度，並靠左對齊 (left)
            cout << left << setw(MAP_CELL_WIDTH) << cellDisplay ;

            // 畫橫向連接線 (如果不是最後一格)
            if (x < width - 1) cout << "- ";
        }

        // === 2. 畫右側怪物列表 ===
        cout << "   |   "; // 中間分隔線

        if (y == 0) {
            // 標題列
            cout << "#  " << left << setw(MONSTER_NAME_WIDTH) << "怪物名稱" << "強度";
        }
        else if (y <= monsters.size()) {
            // 顯示怪物資訊
            // 注意：monsters 的 index 是 y-1
            const Monster& m = monsters[y - 1];
            cout << y << ". "
                << left << setw(MONSTER_NAME_WIDTH) << m.name
                << "Lv." << m.level;
        }
        else if (y == monsters.size() + 1 && !monsters.empty()) {
            cout << "(按 1-" << monsters.size() << " 戰鬥)";
        }

        cout << "\n"; // 換行

        // === 3. 畫地圖的縱向連接線 (除了最後一行) ===
        if (y < height - 1) {
            cout << "  "; // 對齊第一格的中間
            for (int k = 0; k < width - 1; k++) {
                // 這裡手動調整直線的位置，讓它看起來在地圖格子下方
                cout << "|-------|"; // 根據 MAP_CELL_WIDTH 調整這裡的空白數
                if (k < width - 1) cout << " - ";
            }
            cout << "|\n";
        }
    }
}

void MapSystem::showCurrentInfo() {
    MapNode& node = mapGrid[playerY][playerX];
    cout << "\n----------------------------------------\n";
    cout << "目前位置: " << node.name << "\n";
    cout << "描述: " << node.description << "\n";
    cout << "操作: [W]上 [S]下 [A]左 [D]右 [Q]離開遊戲\n";
    cout << "----------------------------------------\n";
}
