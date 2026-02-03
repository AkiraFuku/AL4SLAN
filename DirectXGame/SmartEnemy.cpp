#include "SmartEnemy.h"
#include "MapchipField.h"
// Enemyのbehavior_はprivateなのでprotectedに変更する必要あり
// また、InCamera()もpublicでなければ呼び出せないため、Enemy.hでpublic宣言を確認・修正すること

void SmartEnemy::Update() {
// 1. 死亡時などは親の基本処理に任せる
// Enemy.hのbehavior_をprotectedに変更してアクセス可能にする
// InCamera()はpublicなので呼び出し可能
    if (behavior_ == Behavior::kDead ||Enemy::InCamera()) {
        Enemy::Update();
        return;
    }

    // 2. 足元の崖判定
    if (mapChipField_) {
        float nextX = worldTransform_.translation_.x + velocity_.x;
        float footY = worldTransform_.translation_.y - (kHeight / 2.0f) - 0.1f;
        MapChipField::IndexSet footIndex = mapChipField_->GetMapChipIndexSetByPosition({nextX, footY, 0.0f});
        MapChipType footType = mapChipField_->GetMapChipTypeByIndex(footIndex.xIndex, footIndex.yIndex);

        if (footType == MapChipType::kBlank) {
            velocity_.x *= -1.0f;
        }
    }

    // 3. 基本的な移動・衝突判定処理は親クラスのロジックを実行
    Enemy::Update();
}
