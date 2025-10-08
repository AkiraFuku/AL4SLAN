#include "Player.h"
#include "MapchipField.h"
#include "MassFunction.h"
#include "assert.h"
#include <algorithm>
#include <numbers>

void Player::Initialize(Model* model, Model* modelAttack, uint32_t textureHandle, Camera* camera, const Vector3& position) {
	assert(model);
	/// 引数をメンバ変数に格納
	/// モデル
	model_ = model;
	// 攻撃モデル
	modelAttack_ = modelAttack;

	// テクスチャハンドル
	textureHandle_ = textureHandle;
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	worldTransformAttack_.Initialize();
	worldTransformAttack_.translation_ = position;
	worldTransformAttack_.rotation_ = worldTransform_.rotation_;
	worldTransformAttack_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransformMove_.Initialize();
	worldTransformMove_.translation_ = position;
	worldTransformMove_.scale_ = {1.0f, 1.0f, 1.0f};

	// カメラ
	camera_ = camera;

	// コントローラー

	// sound
	jumpSEHandle_ = Audio::GetInstance()->LoadWave("Sound/SE/jump.wav");
	attackSEHandle_ = Audio::GetInstance()->LoadWave("Sound/SE/attack.wav");
	DeathSEHandle_ = Audio::GetInstance()->LoadWave("Sound/SE/dead.wav");
}

void Player::Update() {

	Input::GetInstance()->GetJoystickState(0, state_);

	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case Player::Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Player::Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Player::Behavior::kDash:
			 BehaviorDashInitialize();
			break;

		}


		// 挙動リクエストを初期化
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {

	case Player::Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Player::Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Player::Behavior::kDash:
		BehaviorDashUpdate();
		break;
	}

	WorldTransformUpdate(&worldTransform_);
	WorldTransformUpdate(&worldTransformAttack_);
}
void Player::BehaviorRootUpdate() {

	// 入力処理
	inputMove();

	// 衝突判定
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;
	MapCollisionCheck(collisionMapInfo);

	// 衝突判定結果をワールドトランスフォームに反映
	ResultCollisionMapInfo(collisionMapInfo);
	// 天井に当たった場合の処理
	hitCeiling(collisionMapInfo);
	HitWall(collisionMapInfo);
	// 着地
	UpdatOnGround(collisionMapInfo);

	// 旋回
	if (turnTimer_ > 0.0f) {
		// 旋回時間を減少
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);
		// 旋回角度

		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}
	// 攻撃に切り替え
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || (state_.Gamepad.wButtons & XINPUT_GAMEPAD_X)) {

		//behaviorRequest_ = Behavior::kDash;
		behaviorRequest_ = Behavior::kAttack;
	}
	if (Input::GetInstance()->TriggerKey(DIK_X) ) {

		behaviorRequest_ = Behavior::kDash;
		//behaviorRequest_ = Behavior::kAttack;
	}

}

void Player::BehaviorAttackUpdate() {
	const Vector3 attackVelocity = {0.4f, 0.0f, 0.0f};
	velocity_ = {0.0f, 0.0f, 0.0f}; // 攻撃時は移動しない
	Vector3 velocity = {};
	attackParameter_++;
	switch (attackPhase_) {
	case Player::AttackPhase::kCharge:
	default: {

		// 攻撃チャージ中
		float t = static_cast<float>(attackParameter_) / kChageTime; // 1秒間のチャージ
		worldTransform_.scale_.z = EaseOut(1.0f, 0.3f, t);           // z軸方向に拡大
		worldTransform_.scale_.y = EaseOut(1.0f, 1.6f, t);           // y軸方向に拡大

		if (attackParameter_ >= kChageTime) {
			attackPhase_ = AttackPhase::kAttack;
			attackParameter_ = 0;
		}
		break;
	}
	case Player::AttackPhase::kAttack: {

		if (lrDirection_ == LRDirection::kRight) {

			velocity = attackVelocity;
		} else if (lrDirection_ == LRDirection::kLeft) {
			velocity = attackVelocity * -1.0f;
		}
		float t = static_cast<float>(attackParameter_) / kAttackTime; // 1秒間の攻撃
		worldTransform_.scale_.z = EaseOut(0.3f, 1.3f, t);
		worldTransform_.scale_.y = EaseIn(1.6f, 0.7f, t);
		if (attackParameter_ >= kAttackTime) {
			attackPhase_ = AttackPhase::kAfter;
			attackParameter_ = 0;
		}

		// 攻撃SE再生
		Audio::GetInstance()->PlayWave(attackSEHandle_, false);
		break;
	}

	case Player::AttackPhase::kAfter: {

		float t = static_cast<float>(attackParameter_) / kAfterTime; // 1秒間の攻撃後
		worldTransform_.scale_.z = EaseOut(1.3f, 1.0f, t);
		worldTransform_.scale_.y = EaseOut(0.7f, 1.0f, t);
		if (attackParameter_ >= kAfterTime) {
			// 攻撃完了。元のRoot状態に戻す
			behaviorRequest_ = Behavior::kRoot;
			attackPhase_ = AttackPhase::kUnknown; // 初期化
			attackParameter_ = 0;
		}
		break;
	}
	}
	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo = {};
	collisionMapInfo.move = velocity;
	collisionMapInfo.isFloor = false;
	collisionMapInfo.isWall = false;

	// マップ衝突チェック
	MapCollisionCheck(collisionMapInfo);
	worldTransform_.translation_ += collisionMapInfo.move;

	worldTransformAttack_.translation_ = worldTransform_.translation_;
	worldTransformAttack_.rotation_ = worldTransform_.rotation_;
}
void Player::BehaviorDashUpdate() {
const Vector3 dashVelocity = {0.4f, 0.0f, 0.0f};
	velocity_ = {0.0f, 0.0f, 0.0f}; // 攻撃時は移動しない
	Vector3 velocity = {};
	dashParameter_++;
	switch (dashPhase_) {
	case Player::DashPhase::kCharge:
	default: {

		// 攻撃チャージ中
		float t = static_cast<float>(dashParameter_) / kDashChageTime; // 1秒間のチャージ
		worldTransform_.scale_.z = EaseOut(1.0f, 0.3f, t);           // z軸方向に拡大
		worldTransform_.scale_.y = EaseOut(1.0f, 1.6f, t);           // y軸方向に拡大

		if (dashParameter_ >= kDashChageTime) {
			dashPhase_ = DashPhase::kDash;
			dashParameter_ = 0;
		}
		break;
	}
	case Player::DashPhase::kDash: {

		if (lrDirection_ == LRDirection::kRight) {

			velocity = dashVelocity;
		} else if (lrDirection_ == LRDirection::kLeft) {
			velocity = dashVelocity * -1.0f;
		}
		float t = static_cast<float>(dashParameter_) / kDashAttackTime; // 1秒間の攻撃
		worldTransform_.scale_.z = EaseOut(0.3f, 1.3f, t);
		worldTransform_.scale_.y = EaseIn(1.6f, 0.7f, t);
		if (dashParameter_ >= kDashAttackTime) {
			dashPhase_ = DashPhase::kAfter;
			dashParameter_ = 0;
		}

		// 攻撃SE再生
		
		break;
	}

	case Player::DashPhase::kAfter: {

		float t = static_cast<float>(dashParameter_) / kDashAfterTime; // 1秒間の攻撃後
		worldTransform_.scale_.z = EaseOut(1.3f, 1.0f, t);
		worldTransform_.scale_.y = EaseOut(0.7f, 1.0f, t);
		if (dashParameter_ >= kDashAfterTime) {
			// 攻撃完了。元のRoot状態に戻す
			behaviorRequest_ = Behavior::kRoot;
			dashPhase_ = DashPhase::kUnknown; // 初期化
			dashParameter_ = 0;
		}
		break;
	}
	}
	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo = {};
	collisionMapInfo.move = velocity;
	collisionMapInfo.isFloor = false;
	collisionMapInfo.isWall = false;

	// マップ衝突チェック
	MapCollisionCheck(collisionMapInfo);
	worldTransform_.translation_ += collisionMapInfo.move;
}

void Player::BehaviorRootInitialize() {}

void Player::BehaviorAttackInitialize() { attackParameter_ = 0; }

void Player::BehaviorDashInitialize() {dashParameter_ = 0;}


bool Player::isAttack() const {

	if (behavior_ == Behavior::kAttack) {
		return true;
	}
	return false;
}

void Player::Draw() {

	model_->Draw(worldTransform_, *camera_);
	if (behavior_ == Behavior::kAttack) {
		switch (attackPhase_) {
		case AttackPhase::kCharge:
		default:
			// 予備動作中は攻撃モデルを描画しない
			break;
		case AttackPhase::kAttack:
		case AttackPhase::kAfter:

			modelAttack_->Draw(worldTransformAttack_, *camera_);

			break;
		}
	}
}

void Player::MapCollisionCheck(CollisionMapInfo& collisionMapInfo) {
	CheckMapCollisionUp(collisionMapInfo);
	CheckMapCollisionDown(collisionMapInfo);
	CheckMapCollisionRight(collisionMapInfo);
	CheckMapCollisionLeft(collisionMapInfo);
}

void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
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

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {
	if (info.move.y >= 0.0f) {
		return;
	}

	std::array<Vector3, kNumCorner> PositionNew;
	for (uint32_t i = 0; i < PositionNew.size(); ++i) {
		PositionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
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
	if (mapChipType == MapChipType::kBlock) {
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

void Player::CheckMapCollisionRight(CollisionMapInfo& info) {

	info;
	if (info.move.x <= 0.0f) {
		return;
	}
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}
	MapChipType mapChipType;
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
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット
	if (hit) {
		// 現在座標が壁の外か判定
		// MapChipField::IndexSet indexSetNow;
		// indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, 0));
		// if (indexSetNow.xIndex != indexSet.xIndex){
		// めり込み排除する方向へ移動
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0.0f, 0.0f));
		// めり込み先のマップチップの矩形を取得
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		// 下方向の移動量を計算
		info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
		info.isWall = true;
		//}
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	info;
	if (info.move.x >= 0.0f) {
		return;
	}
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}
	MapChipType mapChipType;
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
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット
	if (hit) {
		// 現在座標が壁の外か判定
		//	MapChipField::IndexSet indexSetNow;
		// indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(-kWidth / 2.0f, 0, 0));

		// if (indexSetNow.xIndex != indexSet.xIndex){
		//  めり込み排除する方向へ移動
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0.0f, 0.0f));
		// めり込み先のマップチップの矩形を取得
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		// 下方向の移動量を計算
		info.move.x = std::min(0.0f, rect.right - worldTransform_.translation_.x + (kWidth / 2.0f + kBlank));
		info.isWall = true;
		//}
	}
}

void Player::inputMove() {
	const float deadZone = 8000; // デッドゾーン（無反応領域）
	float lx = (float)state_.Gamepad.sThumbLX;

	if (onGround_) {
		jumpCount_=0;


		bool keyRight = Input::GetInstance()->PushKey(DIK_RIGHT);
		bool keyLeft = Input::GetInstance()->PushKey(DIK_LEFT);
		bool stick = fabs(lx) > deadZone;

		// キー入力
		if (keyRight || keyLeft || stick) {
			Vector3 acceleration = {};
			if (keyRight) {
				// 右キーが押されている
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenution);
					// 旋回時の角度
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマー初期化
					turnTimer_ = kTimeTurn;
				}
				acceleration.x += kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
				}
			} else if (keyLeft) {
				// 左キーが押されている
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenution);
				}
				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					// 旋回時の角度
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマー初期化
					turnTimer_ = kTimeTurn;
				}
			} else if (stick) {
				acceleration.x += (lx / 32767.0f) * kAcceleration;

				if (lx > 0 && lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				} else if (lx < 0 && lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}

			velocity_ = Add(velocity_, acceleration);
			// 最大速度
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			// 減速

			velocity_.x *= (1.0f - kAttenution);
		}
		if (std::abs(velocity_.x) <= 0.0001f) {
			velocity_.x = 0.0f;
		}
		
	} else {
		// 落下速度
		velocity_ = Add(velocity_, Vector3(0, -kGravityAcceleration / 60.0f, 0));
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
	if ((Input::GetInstance()->TriggerKey(DIK_UP) || (state_.Gamepad.wButtons & XINPUT_GAMEPAD_A))&&jumpCount_<kLimitJumpCount) {
			jumpCount_++;
			velocity_ = Add(velocity_, Vector3(0, kJumpAcceleration / 60.0f, 0));

			Audio::GetInstance()->PlayWave(jumpSEHandle_, false);
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 OffsetTable[kNumCorner] = {

	    Vector3{+kWidth / 2.0f, -kHeight / 2.0f, 0.0f}, // kRightBottom
	    Vector3{-kWidth / 2.0f, -kHeight / 2.0f, 0.0f}, // kLeftBottom
	    Vector3{+kWidth / 2.0f, +kHeight / 2.0f, 0.0f}, // kRightTop
	    Vector3{-kWidth / 2.0f, +kHeight / 2.0f, 0.0f}  // kLeftTop
	};
	return center + OffsetTable[static_cast<uint32_t>(corner)];
}

void Player::ResultCollisionMapInfo(const CollisionMapInfo& info) { worldTransform_.translation_ += info.move; }

void Player::hitCeiling(const CollisionMapInfo& info) {
	if (info.isCeiling) {
		DebugText::GetInstance()->ConsolePrintf("hitCeiling\n");
		// めり込み排除
		velocity_.y = 0.0f;
	}
}

void Player::UpdatOnGround(const CollisionMapInfo& info) {

	if (onGround_) {
		info;
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			// 落下判定
			std::array<Vector3, kNumCorner> positionsNew;

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

		// // 着地している場合
		if (info.isFloor) {
			//
			onGround_ = true;
			// ｘ方向の速度を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			// y方向の速度をリセット
			velocity_.y = 0.0f;

			
		}
	}
}

void Player::HitWall(const CollisionMapInfo& info) {

	if (info.isWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}

void Player::OnCollision(const Enemy* enemy) {
	if (isAttack()) {
		return;
	}
	(void)enemy;

	isDead_ = true;
	Audio::GetInstance()->PlayWave(DeathSEHandle_, false);
	// velocity_+=Vector3(0.0f,kJumpAcceleration/60.0f,0.0f);
}
