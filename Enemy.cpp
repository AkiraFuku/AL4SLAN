
#include "Enemy.h"
#include "GameScene.h"
#include "HitEffect.h"
#include "MapchipField.h"
#include "Math.h"
#include "Player.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <numbers>

void Enemy::Initialize(Model* model, Camera* camera, Vector3& position) {
	// NULLチェック
	assert(model);

	model_ = model;

	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	// 角度調整
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	velocity_ = {-kWalkSpeed, 0.0f, 0.0f};
	walkTimer_ = 0.0f;
}
void Enemy::Update() {

	// カメラ外だと動かない

	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振るまいを変更する
		behavior_ = behaviorRequest_;

		// 各振るまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kDead:
		default:
			counter_ = 0.0f;
			break;
		}

		// 振るまいリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {

	case Enemy::Behavior::kWalk:
	default: {
		velocity_.y += kGravity;

		// 必要に応じて落下速度制限を入れる場合
		velocity_.y = max(velocity_.y, kLimitFallSpeed);
		// 移動量設定
		CollisionMapInfo collisionMapInfo;
		collisionMapInfo.move = velocity_;

		// マップ衝突判定
		MapCollisionCheck(collisionMapInfo);

		// 判定結果を反映
		ResultCollisionMapInfo(collisionMapInfo);
		if (collisionMapInfo.isFloor) {
			velocity_.y = 0.0f;
		}
		// 壁に当たったら反転するなどのAIロジックが必要ならここに追加
		if (collisionMapInfo.isWall) {
			velocity_.x *= -1.0f; // 例: 壁に当たったら反転
		}

		walkTimer_ += 1.0f / 60.0f;

		float param = std::sinf(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime);
		float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
		worldTransform_.rotation_.x = Radian(degree);
		WorldTransformUpdate(&worldTransform_);
		break;
	}
	case Enemy::Behavior::kDead: {
		// 死亡時の処理
		// 死亡アニメーションの時間を経過させる
		counter_ += 1.0f / 60.0f;            // 1フレーム分の時間を引く
		worldTransform_.rotation_.y += 0.3f; // Y座標を下げる
		worldTransform_.rotation_.x = EaseOut(ToRadian(kDeadMotionAngleStart), ToRadian(kDeadMotionAngleEnd), counter_ / kDeadTime);

		WorldTransformUpdate(&worldTransform_);
		if (counter_ >= kDeadTime) {
			isDead_ = true; // 死亡アニメーションが終わったら、敵を削除するフラグを立てる
		}

		break;
	}
	}
};
void Enemy::Draw() {
	float minX_world = camera_->translation_.x - kViewRangeX;
	float maxX_world = camera_->translation_.x + kViewRangeX;
	float minY_world = camera_->translation_.y - kViewRangeY;
	float maxY_world = camera_->translation_.y + kViewRangeY;

	if (worldTransform_.translation_.x + kWidth / 2.0f >= minX_world && // エネミーの右端が画面の左端より右にある
	    worldTransform_.translation_.x - kWidth / 2.0f <= maxX_world && // エネミーの左端が画面の右端より左にある
	    worldTransform_.translation_.y + kHeight / 2.0f >= minY_world && worldTransform_.translation_.y - kHeight / 2.0f <= maxY_world) {
		// 描画実行
		model_->Draw(worldTransform_, *camera_);
	}
};
Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}

void Enemy::OnCollision(const Player* player) {
	//(void)player;
	// プレイヤーとの衝突時の処理をここに記述
	// 例えば、敵を消す、ダメージを与えるなど
	// 今回は何もしない
	// ただし、死亡フラグを立てるなどの処理は行う

	/// isDead_ = true; // プレイヤーと衝突したら死亡
	if (behavior_ == Behavior::kDead) {
		return;
	}
	if (player->isAttack()) {
		// 敵の振るまいをやられに変更
		behaviorRequest_ = Behavior::kDead;

		Vector3 effectPos = Division(Add(worldTransform_.translation_, player->GetWorldTransform().translation_), 2.0f);
		gameScene_->CreateHitEffect(effectPos);

		isCollisionDisabled_ = true; // 衝突を無効化
	}
}

void Enemy::HitAttack(const Player* player) {
	if (behavior_ == Behavior::kDead) {
		return;
	}

	// 敵の振るまいをやられに変更
	behaviorRequest_ = Behavior::kDead;

	Vector3 effectPos = Division(Add(worldTransform_.translation_, player->GetWorldTransform().translation_), 2.0f);
	gameScene_->CreateHitEffect(effectPos);

	isCollisionDisabled_ = true; // 衝突を無効化
}
bool Enemy::InCamera() {
	const float kActiveRange = 30.0f;
	return (std::abs(GetWorldPosition().x - camera_->translation_.x) > kActiveRange);
}

// ★追加: 角の座標を取得する関数
Vector3 Enemy::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0.0f}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0.0f}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0.0f}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0.0f}  // kLeftTop
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}
// ★追加: マップ衝突判定の統括関数
void Enemy::MapCollisionCheck(CollisionMapInfo& info) {
	if (mapChipField_) {
		CheckMapCollisionUp(info);
		CheckMapCollisionDown(info);
		CheckMapCollisionRight(info);
		CheckMapCollisionLeft(info);
	}
}
void Enemy::CheckMapCollisionUp(CollisionMapInfo& info) {
	if (info.move.y <= 0.0f)
		return;

	std::vector<Vector3> positionsNew(kNumCorner);
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	bool hit = false;
	// 左上
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock)
		hit = true;
	// 右上
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock)
		hit = true;

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, +kHeight / 2.0f, 0));
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		info.isCeiling = true;
	}
}

void Enemy::CheckMapCollisionDown(CollisionMapInfo& info) {
	if (info.move.y >= 0.0f)
		return;

	std::vector<Vector3> positionsNew(kNumCorner);
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	bool hit = false;
	// 左下
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock)
		hit = true;
	// 右下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock)
		hit = true;

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, -kHeight / 2.0f, 0));
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
		info.isFloor = true;
	}
}

void Enemy::CheckMapCollisionRight(CollisionMapInfo& info) {
	if (info.move.x <= 0.0f)
		return;

	std::vector<Vector3> positionsNew(kNumCorner);
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	bool hit = false;
	// 右上
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock)
		hit = true;
	// 右下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock)
		hit = true;

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, 0));
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
		info.isWall = true;
	}
}
void Enemy::CheckMapCollisionLeft(CollisionMapInfo& info) {
	if (info.move.x >= 0.0f)
		return;

	std::vector<Vector3> positionsNew(kNumCorner);
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	bool hit = false;
	// 左上
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock)
		hit = true;
	// 左下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock)
		hit = true;

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, 0));
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = min(0.0f, rect.right - worldTransform_.translation_.x + (kWidth / 2.0f + kBlank));
		info.isWall = true;
	}
}

void Enemy::ResultCollisionMapInfo(const CollisionMapInfo& info) { worldTransform_.translation_ += info.move; }

void Enemy::OnCollisionEnemy(Enemy* other) {
    // 自分のAABBと相手のAABBを取得
    AABB myBox = GetAABB();
    AABB otherBox = other->GetAABB();

    // 衝突判定 (AABB同士の交差チェック)
    if (myBox.min.x < otherBox.max.x && myBox.max.x > otherBox.min.x &&
        myBox.min.y < otherBox.max.y && myBox.max.y > otherBox.min.y &&
        myBox.min.z < otherBox.max.z && myBox.max.z > otherBox.min.z) {

        // 重なり量を計算
        float overlapX = min(myBox.max.x, otherBox.max.x) -max(myBox.min.x, otherBox.min.x);
        float overlapY = min(myBox.max.y, otherBox.max.y) -max(myBox.min.y, otherBox.min.y);

        // 横方向の重なりが、縦方向より小さい場合（＝横からぶつかったとみなす）
        // または、今回は歩行敵同士なので横方向の押し出しを優先したい場合
        if (overlapX < overlapY) {
            // 自分の位置と相手の位置の前後関係を調べる
            Vector3 myPos = GetWorldPosition();
            Vector3 otherPos = other->GetWorldPosition();

            // 自分が左にいるなら左へ、右にいるなら右へ押し出す
            // お互いに半分ずつずれるように 0.5f を掛ける
            float pushAmount = overlapX * 0.5f;

            if (myPos.x < otherPos.x) {
                worldTransform_.translation_.x -= pushAmount;
            } else {
                worldTransform_.translation_.x += pushAmount;
            }
        }
        // 必要であればここに縦方向(Y)の処理も追加できますが、
        // ジャンプして踏みつけたりしない限り、横だけで十分な場合が多いです。
    }
}