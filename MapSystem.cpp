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

    // 處理輸入 (大小寫通吃)
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

void MapSystem::displayMap() {
    // 這裡我們畫出一個簡單的 5x5 網格
    // 玩家位置用紅色或特殊符號標示

    cout << "\n      === 世界地圖 ===\n\n";

    for (int y = 0; y < height; y++) {
        cout << "  "; // 縮排
        for (int x = 0; x < width; x++) {
            if (x == playerX && y == playerY) {
                // 玩家位置：用 [] 包起來，並加上Ｐ
                cout << "[★你★]";
            }
            else {
                // 其他位置
                if (mapGrid[y][x].type == WALL)
                    cout << " . . . "; // 牆壁顯示點點
                else
                    cout << mapGrid[y][x].name;
            }
            if (x < width - 1) cout << " - "; // 橫向連接線
        }
        cout << "\n";

        // 畫縱向連接線 (除了最後一行)
        if (y < height - 1) {
            cout << "            |         |         |         |         |\n";
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
