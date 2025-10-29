
#define NOMINMAX
#include "Enemy.h"
#include "GameScene.h"
#include "HitEffect.h"
#include "Math.h"
#include "Player.h"
#include <cassert>
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
	accel_ = {-kWalkSpeed, 0.0f, 0.0f};
	velocity_ = accel_;
	walkTimer_ = 0.0f;
}
void Enemy::Update() {

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

		velocity_ = accel_;
		CollisionMapInfo collisionMapInfo;
		collisionMapInfo.move = velocity_;
		MapCollisionCheck(collisionMapInfo);

		//HitWall(collisionMapInfo);

	/*	if (tachWall_) {
			velocity_.x = 0.0f;
		}*/

		worldTransform_.translation_ += collisionMapInfo.move;

		walkTimer_ += 1.0f / 60.0f;

		float param = std::sinf(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime);
		float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
		worldTransform_.rotation_.x = Radian(degree);

		break;
	}
	case Enemy::Behavior::kDead: {
		// 死亡時の処理
		// 死亡アニメーションの時間を経過させる
		counter_ += 1.0f / 60.0f;            // 1フレーム分の時間を引く
		worldTransform_.rotation_.y += 0.3f; // Y座標を下げる
		worldTransform_.rotation_.x = EaseOut(ToRadian(kDeadMotionAngleStart), ToRadian(kDeadMotionAngleEnd), counter_ / kDeadTime);
		if (counter_ >= kDeadTime) {
			isDead_ = true; // 死亡アニメーションが終わったら、敵を削除するフラグを立てる
		}

		break;
	}
	}
	WorldTransformUpdate(&worldTransform_);
};
void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); };
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
void Enemy::MapCollisionCheck(CollisionMapInfo& collisionMapInfo) {
	CheckMapCollisionUp(collisionMapInfo);
	CheckMapCollisionDown(collisionMapInfo);
	CheckMapCollisionRight(collisionMapInfo);
	CheckMapCollisionLeft(collisionMapInfo);
}
Vector3 Enemy::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 OffsetTable[kNumCorner] = {

	    Vector3{+kWidth / 2.0f, -kHeight / 2.0f, 0.0f}, // kRightBottom
	    Vector3{-kWidth / 2.0f, -kHeight / 2.0f, 0.0f}, // kLeftBottom
	    Vector3{+kWidth / 2.0f, +kHeight / 2.0f, 0.0f}, // kRightTop
	    Vector3{-kWidth / 2.0f, +kHeight / 2.0f, 0.0f}  // kLeftTop
	};
	return center + OffsetTable[static_cast<uint32_t>(corner)];
}
void Enemy::CheckMapCollisionUp(CollisionMapInfo& info) {
	if (info.move.y <= 0.0f) {
		return;
	}
	std::array<Vector3, kNumCorner> PositionNew;
	for (uint32_t i = 0; i < PositionNew.size(); ++i) {
		PositionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}
	MapChipType mapChipType;
	// 真上の当たり判定
	bool hit = false;
	// 左上の当たり判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(PositionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右上の当たり判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(PositionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 当たっていたら
	if (hit) {
		// めり込み排除する方向へ移動
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0.0f, +kHeight / 2.0f, 0.0f));
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		// 上方向の移動量を計算
		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		info.isCeiling = true;
	}
}

void Enemy::CheckMapCollisionDown(CollisionMapInfo& info) {
	if (info.move.y >= 0.0f) {
		return;
	}

	std::array<Vector3, kNumCorner> PositionNew;
	for (uint32_t i = 0; i < PositionNew.size(); ++i) {
		PositionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	// 下方向の当たり判定
	bool hit = false;
	// 左下の当たり判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(PositionNew[kLeftBottom]);

	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下の当たり判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(PositionNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 当たっていたら
	if (hit) {
		// めり込み排除する方向へ移動
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0.0f, -kHeight / 2.0f, 0.0f));
		// めり込み先のマップチップの矩形を取得
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		// 下方向の移動量を計算
		info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
		info.isFloor = true;
	}
}

void Enemy::CheckMapCollisionRight(CollisionMapInfo& info) {

	if (info.move.x <= 0.0f) {
		return;
	}
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}
	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 右側の当たり判定
	bool hit = false;
	// 右上の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット
	if (hit) {

		// めり込み排除する方向へ移動
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0.0f, 0.0f));
		// めり込み先のマップチップの矩形を取得
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		// 下方向の移動量を計算
		info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
		info.isWall = true;
	}
}

void Enemy::CheckMapCollisionLeft(CollisionMapInfo& info) {
	if (info.move.x >= 0.0f) {
		return;
	}
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}
	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 左側の当たり判定
	bool hit = false;
	// 左上の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 左下の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット
	if (hit) {
		// めり込み排除する方向へ移動
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0.0f, 0.0f));
		// めり込み先のマップチップの矩形を取得
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		// 下方向の移動量を計算
		info.move.x = std::min(0.0f, rect.right - worldTransform_.translation_.x + (kWidth / 2.0f + kBlank));
		info.isWall = true;
	}
}

void Enemy::hitCeiling(const CollisionMapInfo& info) {
	if (info.isCeiling) {
		DebugText::GetInstance()->ConsolePrintf("hitCeiling\n");
		// めり込み排除
		velocity_.y = 0.0f;
	}
}
void Enemy::UpdatOnGround(const CollisionMapInfo& info) {
	if (onGround_) {

		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			// 落下判定
			std::array<Vector3, kNumCorner> positionsNew;
			// 移動後の各頂点座標を計算
			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
			}
			MapChipType mapChipType;
			// 真下の当たり判定
			bool hit = false;
			// 左下
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 右下
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 落下開始
			if (!hit) {
				DebugText::GetInstance()->ConsolePrintf("jump");
				onGround_ = false;
			}
		}

	} else {
		if (info.isFloor) {

			onGround_ = true;

			velocity_.x *= (1.0f - kAttenuationLanding);

			velocity_.y = 0.0f;
		}
	}
}
void Enemy::HitWall(const CollisionMapInfo& info){
	if (tachWall_) {
		if (!info.isWall) {
			// 壁から離れた
			tachWall_ = false;
		}
	} else {
		if (info.isWall) {
			velocity_.x *= (1.0f - kAttenuationWall);
			//velocity_.x =0.0f;
			tachWall_ = true;
		}
	}
}
