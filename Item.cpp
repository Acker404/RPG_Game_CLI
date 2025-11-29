#include "Item.h"
#include "Character.h" // 這裡才真正引入 Character

// 因為 Item.h 裡如果已經寫了實作，這裡就不用寫。
// 但因為 Consumable 需要呼叫 user->recover()，編譯器必須知道 Character 的長相。
// 所以請把 Item.h 裡的 Consumable::use 函式內容改成只寫宣告： bool use(Character* user) override;
// 然後在這裡實作：

/* 請將 Item.h 裡的 Consumable::use 改成：
   bool use(Character* user) override; // 只留這一行

   然後在 Item.cpp 寫：
*/


bool Consumable::use(Character* user) {
    if (isMp) {
        user->recover(0, healAmount);
        cout << "使用了 " << name << "，恢復了 " << healAmount << " 點 MP！\n";
    } else {
        user->recover(healAmount, 0);
        cout << "使用了 " << name << "，恢復了 " << healAmount << " 點 HP！\n";
    }
    return true;
}
