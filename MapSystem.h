#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "Monster.h"
using namespace std;

// 地圖節點類型
enum NodeType {
    WALL = 0,   // 牆壁/無法移動
    TOWN = 1,   // 城鎮 (安全區)
    WILD = 2,   // 野外 (有怪)
    SHOP = 3,   // 商店
    BOSS = 4    // Boss 區
};

struct MapNode {
    string name;      // 地點名稱 (e.g. "新手村")
    NodeType type;    // 類型
    string description; // 描述

    MapNode(string n = "未探索", NodeType t = WALL, string d = "")
        : name(n), type(t), description(d) {
    }
};

class MapSystem {
private:
    int width, height;
    int playerX, playerY; // 玩家當前座標
    vector<vector<MapNode>> mapGrid; // 二維地圖

public:
    MapSystem();

    // 初始化地圖配置 (硬把地圖寫死在程式裡)
    void initMap();

    // 移動邏輯 (回傳 true 代表移動成功，false 代表撞牆)
    bool movePlayer(char direction);

    // === 修改這裡：加入 monsters 參數 ===
    void displayMap(const vector<Monster>& monsters);
    void showCurrentInfo(); // 顯示當前位置詳細資訊

    // Getters 用於判斷遭遇事件
    NodeType getCurrentNodeType() const { return mapGrid[playerY][playerX].type; }
    string getCurrentLocationName() const { return mapGrid[playerY][playerX].name; }
    // 加入 public 方法
    void teleportToTown() {
        playerX = 2;
        playerY = 2;
        cout << "咻！你被傳送回到了城鎮中心。\n";
    }
};