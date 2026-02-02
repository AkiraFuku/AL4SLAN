#pragma once
#include "Enemy.h"
class SmartEnemy : public Enemy
{
public:
    void Update() override; // 親のUpdateをオーバーライド
};
