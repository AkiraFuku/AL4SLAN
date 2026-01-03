
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
	WorldTransformUpdate(&worldTransform_);
}
void Enemy::Update() {
	if (InCamera()) {
        return; 
    }
	if (collisionCooldown_ > 0.0f) {
		collisionCooldown_ -= 1.0f / 60.0f;
	}

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
		
		if (mapCollider_) {
			mapCollider_->CheckCollision(worldTransform_.translation_, kWidth - kBlank, kHeight - kBlank, collisionMapInfo);
		}

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
	// 写ってないを返す
	const float kActiveRange = 30.0f;
	return (std::abs(GetWorldPosition().x - camera_->translation_.x) > kActiveRange);
}

void Enemy::SetMapChipField(MapChipField* mapChipField) {

	mapCollider_ = std::make_unique<MapCollider>();

	mapChipField_ = mapChipField;
	if (mapCollider_) {
		mapCollider_->Initialize(mapChipField_);
	}
}


void Enemy::ResultCollisionMapInfo(const CollisionMapInfo& info) { worldTransform_.translation_ += info.move; }

void Enemy::OnCollisionWithEnemy() {
	if (collisionCooldown_ > 0.0f) {
		return;
	}
	// 速度を反転させる
	velocity_.x *= -1.0f;

	// 補足: 連続して判定が起きないように、少しだけ位置をずらす処理を入れるとより安定します
	// 例: velocity_.x がプラスなら少し右へ、マイナスなら少し左へ強制移動など
	// 今回はシンプルに反転のみとします
	collisionCooldown_ = 0.2f;
}
void Enemy::OnLandOnEnemy(float targetTopY) {
	// 落下中のみ着地処理を行う（上昇中に頭をぶつけた場合は除外するため）
	if (velocity_.y <= 0.0f) {
		// Ｙ速度をリセット（着地）
		velocity_.y = 0.0f;

		// 位置を相手の頭上に補正
		// (相手のTop + 自分の身長の半分)
		worldTransform_.translation_.y = targetTopY + (kHeight / 2.0f);
	}
}
void Enemy::AddPosition(const Vector3& offset) { worldTransform_.translation_ += offset; }