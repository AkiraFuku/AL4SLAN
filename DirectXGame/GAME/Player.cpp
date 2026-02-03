#include "Player.h"
#include "Enemy.h"
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
	directionCtrl_.Initialize(LRDirection::kRight);
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

	invincibleTimer_ = 0;
	knockbackTimer_ = 0;

	// sound
	jumpSEHandle_ = Audio::GetInstance()->LoadWave("Sound/SE/jump.wav");
	attackSEHandle_ = Audio::GetInstance()->LoadWave("Sound/SE/attack.wav");


}

void Player::Update() {

	if (isClearAnimation_) {
        if (isMovingToTarget_) {
            // 1. 位置調整（目的地へじわっと移動）
            worldTransform_.translation_ = Lerp(worldTransform_.translation_, autoMoveTarget_, 0.1f);

            // 目的地に十分近づいたら回転フェーズへ
            float distance = Length(Subtract(autoMoveTarget_, worldTransform_.translation_));
            if (distance < 0.1f) {
                isMovingToTarget_ = false;
                
                // カメラの方を向く角度を計算
                Vector3 toCamera = Subtract(cameraPosForLookAt_, worldTransform_.translation_);
                targetRotation_.y = std::atan2(toCamera.x, toCamera.z);
                targetRotation_.z = 0.3f; // 左に傾ける
            }
        } else {
            // 2. 回転アニメーション
            worldTransform_.rotation_.y = Lerp(worldTransform_.rotation_.y, targetRotation_.y, 0.1f);
            worldTransform_.rotation_.z = Lerp(worldTransform_.rotation_.z, targetRotation_.z, 0.1f);
        }
        
     WorldTransformUpdate(&worldTransform_);
        return;
    }
	// if (isDead_) {
	// Audio::GetInstance()->PlayWave(DeathSEHandle_, false);

	//}

	Input::GetInstance()->GetJoystickState(0, state_);

	if (invincibleTimer_ > 0) {
		invincibleTimer_--;
	}

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
	// 旋回更新処理
	directionCtrl_.Update();

	// 回転の反映
	worldTransform_.rotation_.y = directionCtrl_.GetYRotation();
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

	prevState_ = state_;
}
void Player::BehaviorRootUpdate() {

	// 入力処理
	inputMove();

	// 衝突判定
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;

	if (mapCollider_) {

		mapCollider_->CheckCollision(worldTransform_.translation_, kWidth, kHeight, collisionMapInfo);
	}

	// 衝突判定結果をワールドトランスフォームに反映
	ResultCollisionMapInfo(collisionMapInfo);
	// 天井に当たった場合の処理
	hitCeiling(collisionMapInfo);
	// 壁に当たった場合の処理
	HitWall(collisionMapInfo);
	// 着地
	UpdateOnGround(collisionMapInfo);

	// 攻撃に切り替え
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || (state_.Gamepad.wButtons & XINPUT_GAMEPAD_X && !(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_X))) {

		behaviorRequest_ = Behavior::kAttack;

		directionCtrl_.ImmediateTurn(); // 即座に向く
	}
	if (Input::GetInstance()->TriggerKey(DIK_X) || (state_.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER && prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)) {

		behaviorRequest_ = Behavior::kDash;

		directionCtrl_.ImmediateTurn(); // 即座に向く
	}
	if (isWallHit_) {
		wallHitParameter_++;
		float t = static_cast<float>(wallHitParameter_) / static_cast<float>(kTimeWallHit);

		// 横(Z)に潰れて(0.7倍)、縦(Y)に伸びる(1.2倍)状態から、1.0倍に戻していく
		// ※左右どちらの壁に当たってもいいようにZを制御
		worldTransform_.scale_.z = EaseOut(0.7f, 1.0f, t);
		worldTransform_.scale_.y = EaseOut(1.2f, 1.0f, t);

		if (wallHitParameter_ >= kTimeWallHit) {
			isWallHit_ = false;
			worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
		}
	} else if (isLanding_) {
		landingParameter_++;

		float t = static_cast<float>(landingParameter_) / static_cast<float>(kTimeLanding);

		// 縦に潰れて(0.6倍)、横に広がる(1.4倍)ところから、通常(1.0倍)に戻していく
		// EaseOut(開始値, 終了値, t)
		worldTransform_.scale_.y = EaseOut(0.6f, 1.0f, t); // 高さ：潰れた状態 -> 元に戻る
		worldTransform_.scale_.z = EaseOut(1.4f, 1.0f, t); // 幅　：広がった状態 -> 元に戻る
		worldTransform_.scale_.x = 1.0f;                   // x軸（厚み）は今回は変えない（必要ならzと同じにする）

		// アニメーション終了
		if (landingParameter_ >= kTimeLanding) {
			isLanding_ = false;
			worldTransform_.scale_ = {1.0f, 1.0f, 1.0f}; // 念のためサイズをリセット
		}
	} else if (isJumping_) {
		jumpParameter_++;
		float t = static_cast<float>(jumpParameter_) / static_cast<float>(kTimeJumpSquash);

		if (t <= 0.5f) {
			// 前半：踏み込み（さらに低く、横に広く）
			float internalT = t * 2.0f; // 0.0 -> 1.0
			worldTransform_.scale_.y = EaseOut(1.0f, 0.5f, internalT);
			worldTransform_.scale_.z = EaseOut(1.0f, 1.5f, internalT);
		} else {
			// 後半：飛び上がり（縦に長く、横に細く）
			float internalT = (t - 0.5f) * 2.0f; // 0.0 -> 1.0
			worldTransform_.scale_.y = EaseOut(0.5f, 1.3f, internalT);
			worldTransform_.scale_.z = EaseOut(1.5f, 0.7f, internalT);
		}

		// アニメーション終了（空中でも一定時間で元に戻す）
		if (jumpParameter_ >= kTimeJumpSquash) {
			isJumping_ = false;
			worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
		}
	} else {
		// 着地中でも攻撃中でもダッシュ中でもなければサイズを通常に保つ
		// （これを書かないと、ダッシュ後にサイズがおかしくなる場合があるため安全策）
		if (behavior_ == Behavior::kRoot) {
			worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
		}
	}
}

void Player::BehaviorAttackUpdate() {
	const Vector3 attackVelocity = {0.4f, 0.0f, 0.0f};
	inputMove();

	attackParameter_++;
	switch (attackPhase_) {
	case Player::AttackPhase::kCharge:
	default: {

		// 攻撃チャージ中
		// float t = static_cast<float>(attackParameter_) / kChageTime; // 1秒間のチャージ
		// worldTransform_.scale_.z = EaseOut(1.0f, 0.3f, t);           // z軸方向に拡大
		// worldTransform_.scale_.y = EaseOut(1.0f, 1.6f, t);           // y軸方向に拡大

		if (attackParameter_ >= kChageTime) {
			attackPhase_ = AttackPhase::kAttack;
			attackParameter_ = 0;
		}
		break;
	}
	case Player::AttackPhase::kAttack: {

		if (attackParameter_ >= kAttackTime) {
			attackPhase_ = AttackPhase::kAfter;
			attackParameter_ = 0;
		}

		// 攻撃SE再生
		if (!Audio::GetInstance()->IsPlaying(attackSEPlayHandle_) && !attackSEPlayed_) {
			attackSEPlayHandle_=Audio::GetInstance()->PlayWave(attackSEHandle_, false);
			attackSEPlayed_ = true; // SE再生中フラグを立てる
		}

		break;
	}

	case Player::AttackPhase::kAfter: {
		attackSEPlayed_ = false;
		if (attackParameter_ >= kAfterTime) {
			// 攻撃完了。元のRoot状態に戻す
			behaviorRequest_ = Behavior::kRoot;
			attackPhase_ = AttackPhase::kUnknown; // 初期化
			attackParameter_ = 0;
		}
		break;
	}
	}
	CollisionMapInfo collisionMapInfo = {};
	collisionMapInfo.move = velocity_;
	if (mapCollider_) {
		mapCollider_->CheckCollision(worldTransform_.translation_, kWidth, kHeight, collisionMapInfo);
	}
	ResultCollisionMapInfo(collisionMapInfo);
	hitCeiling(collisionMapInfo);
	HitWall(collisionMapInfo);
	UpdateOnGround(collisionMapInfo);

	// 攻撃用ワールドトランスフォームの計算
	// 攻撃オフセット値（横方向と縦方向）
	const float attackOffsetX = (kWidth + kAttackWidth) / 2.0f;
	const float attackOffsetY = (kHeight + kAttackHeight) / 2.0f;

	// 入力を取得
	// スティックのデッドゾーン
	const int16_t deadZone = 8000;
	bool isUpInput = Input::GetInstance()->PushKey(DIK_UP) || (state_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) || state_.Gamepad.sThumbLY > deadZone;

	bool isDownInput = Input::GetInstance()->PushKey(DIK_DOWN) || (state_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) || state_.Gamepad.sThumbLY < -deadZone;
	bool isLRInput = Input::GetInstance()->PushKey(DIK_A) || Input::GetInstance()->PushKey(DIK_D) || (state_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ||
	                 (state_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) || state_.Gamepad.sThumbLX < -deadZone || state_.Gamepad.sThumbLX > deadZone;

	// プレイヤーの現在位置
	Vector3 playerPos = worldTransform_.translation_;

	// 基本の回転はプレイヤーに合わせる
	worldTransformAttack_.rotation_ = worldTransform_.rotation_;
	if (isLRInput) {
		// 上下入力がない場合：キャラクターの向き（左右）に配置
		if (GetDirection() == LRDirection::kRight) {
			worldTransformAttack_.translation_ = playerPos + Vector3{attackOffsetX, 0.0f, 0.0f};
		} else {
			worldTransformAttack_.translation_ = playerPos + Vector3{-attackOffsetX, 0.0f, 0.0f};
		}
	}

	else if (isUpInput) {
		// 上入力がある場合：上に配置
		worldTransformAttack_.translation_ = playerPos + Vector3{0.0f, attackOffsetY, 0.0f};
		// worldTransformAttack_.rotation_ = worldTransform_.rotation_;
		//  (オプション) モデルを上に向ける回転が必要な場合はここでZ軸などを回転させる
		if (GetDirection() == LRDirection::kRight) {
			worldTransformAttack_.rotation_.z = std::numbers::pi_v<float> / 2.0f;
		} else {
			worldTransformAttack_.rotation_.z = -std::numbers::pi_v<float> / 2.0f;
		}

	} else if (isDownInput) {
		// 下入力がある場合：下に配置
		worldTransformAttack_.translation_ = playerPos + Vector3{0.0f, -attackOffsetY, 0.0f};
		// worldTransformAttack_.rotation_ = worldTransform_.rotation_;
		//  (オプション) モデルを下に向ける回転
		if (GetDirection() == LRDirection::kRight) {
			worldTransformAttack_.rotation_.z = -std::numbers::pi_v<float> / 2.0f;
		} else {
			worldTransformAttack_.rotation_.z = std::numbers::pi_v<float> / 2.0f;
		}

	} else {
		// 上下入力がない場合：キャラクターの向き（左右）に配置
		if (GetDirection() == LRDirection::kRight) {
			worldTransformAttack_.translation_ = playerPos + Vector3{attackOffsetX, 0.0f, 0.0f};
		} else {
			worldTransformAttack_.translation_ = playerPos + Vector3{-attackOffsetX, 0.0f, 0.0f};
		}
	}

	worldTransformAttack_.scale_ = {1.0f, 1.0f, 1.0f};
}
void Player::BehaviorDashUpdate() {
	const Vector3 dashVelocity = {dashSpeed, 0.0f, 0.0f};
	// velocity_ = {0.0f, 0.0f, 0.0f}; // 攻撃時は移動しない
	// Vector3 velocity = {};
	dashParameter_++;
	switch (dashPhase_) {
	case Player::DashPhase::kCharge:
	default: {

		// 攻撃チャージ中
		float t = static_cast<float>(dashParameter_) / kDashChageTime; // 1秒間のチャージ
		worldTransform_.scale_.z = EaseOut(1.0f, 0.3f, t);             // z軸方向に拡大
		worldTransform_.scale_.y = EaseOut(1.0f, 1.6f, t);             // y軸方向に拡大

		if (dashParameter_ >= kDashChageTime) {
			dashPhase_ = DashPhase::kDash;
			dashParameter_ = 0;
		}
		break;
	}
	case Player::DashPhase::kDash: {

		if (GetDirection() == LRDirection::kRight) {

			velocity_ = dashVelocity; // ダッシュ中は速度を一定に保つ
		} else if (GetDirection() == LRDirection::kLeft) {
			velocity_ = dashVelocity * -1.0f;
		}
		float t = static_cast<float>(dashParameter_) / kDashAttackTime; // 1秒間の攻撃
		worldTransform_.scale_.z = EaseOut(0.3f, 1.3f, t);
		worldTransform_.scale_.y = EaseIn(1.6f, 0.7f, t);
		if (dashParameter_ >= kDashAttackTime) {
			dashPhase_ = DashPhase::kAfter;
			dashParameter_ = 0;
		}
		if (Input::GetInstance()->PushKey(DIK_LEFT) || Input::GetInstance()->PushKey(DIK_RIGHT)) {

			inputMove();
			// ダッシュ中に移動入力が入ったらダッシュをキャンセルして通常移動に戻る
			worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

			// 攻撃完了。元のRoot状態に戻す
			behaviorRequest_ = Behavior::kRoot;
			dashPhase_ = DashPhase::kUnknown; // 初期化
			dashParameter_ = 0;
			// ダッシュの慣性を引き継ぐ
			// if (GetDirection() == LRDirection::kRight) {
			//	velocity_.x = +kLimitRunSpeed * 0.4f; // 通常移動の上限速度の80%でスタート
			//} else {
			//	velocity_.x = -kLimitRunSpeed * 0.4f;
			//}
		}
		// 攻撃SE再生

		break;
	}

	case Player::DashPhase::kAfter: {

		velocity_.x *= 0.95f;
		float t = static_cast<float>(dashParameter_) / kDashAfterTime; // 1秒間の攻撃後
		worldTransform_.scale_.z = EaseOut(1.3f, 1.0f, t);
		worldTransform_.scale_.y = EaseOut(0.7f, 1.0f, t);
		if (dashParameter_ >= kDashAfterTime) {
			// 攻撃完了。元のRoot状態に戻す
			behaviorRequest_ = Behavior::kRoot;
			dashPhase_ = DashPhase::kUnknown; // 初期化
			dashParameter_ = 0;
			//// ダッシュの慣性を引き継ぐ
			// if (GetDirection() == LRDirection::kRight) {
			//	velocity_.x = +kLimitRunSpeed * 0.8f; // 通常移動の上限速度の80%でスタート
			// } else {
			//	velocity_.x = -kLimitRunSpeed * 0.8f;
			// }
		}
		break;
	}
	}
	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo = {};
	collisionMapInfo.move = velocity_;

	if (mapCollider_) {
		mapCollider_->CheckCollision(worldTransform_.translation_, kWidth, kHeight, collisionMapInfo);
	}
	worldTransform_.translation_ += collisionMapInfo.move;
}

void Player::BehaviorRootInitialize() {}

void Player::BehaviorAttackInitialize() { attackParameter_ = 0; }

void Player::BehaviorDashInitialize() { dashParameter_ = 0; }

void Player::WallKick() {}

bool Player::isAttack() const {

	if (behavior_ == Behavior::kAttack) {
		return true;
	}
	return false;
}

//void Player::StartClearAnimation(const Vector3& cameraPos) {
//
//	isClearAnimation_ = true;
//
//    // 1. カメラの方を向くY軸角度を計算
//    // プレイヤーからカメラへのベクトル
//    Vector3 toCamera = {
//        cameraPos.x - worldTransform_.translation_.x,
//        0.0f, // 高さ方向は無視して水平に振り向く場合
//        cameraPos.z - worldTransform_.translation_.z
//    };
//    
//    // atan2を使って角度(ラジアン)を求める
//    targetRotation_.y = std::atan2(toCamera.x, toCamera.z);
//
//    // 2. カメラから見て左に傾ける (Z軸回転)
//    // 度数法でいう15度〜20度くらいをラジアンに変換 (約0.3f)
//    targetRotation_.z = 0.3f;
//}

void Player::StartGrabAnimation(const Vector3& targetPos, const Vector3& cameraPos) {
	isClearAnimation_ = true;
    isMovingToTarget_ = true;
    autoMoveTarget_ = targetPos;
    cameraPosForLookAt_ = cameraPos;
}

void Player::Draw() {
	// 無敵時間中は点滅させる処理
	if (invincibleTimer_ > 0) {
		// 10フレーム中、5フレームだけ描画する（チカチカする）
		if (invincibleTimer_ % 10 < 5) {
			return; // 描画関数を呼ばずに抜ける＝消える
		}
	}
	model_->Draw(worldTransform_, *camera_);
	if (behavior_ == Behavior::kAttack) {

		modelAttack_->Draw(worldTransformAttack_, *camera_);
	}
}
void Player::SetMapchipField(MapChipField* mapChipField) {
	mapCollider_ = std::make_unique<MapCollider>();
	mapChipField_ = mapChipField;
	if (mapCollider_) {
		mapCollider_->Initialize(mapChipField_);
	}
}

void Player::inputMove() {

	if (knockbackTimer_ > 0) {
		knockbackTimer_--;
		// ノックバック中は重力だけ適用して、左右入力は受け付けない
		velocity_.y = std::max(velocity_.y - kGravityAcceleration / 60.0f, -kLimitFallSpeed);
		return;
	}

	const float deadZone = 8000; // デッドゾーン（無反応領域）
	float lx = (float)state_.Gamepad.sThumbLX;
	bool keyRight = Input::GetInstance()->PushKey(DIK_RIGHT);
	bool keyLeft = Input::GetInstance()->PushKey(DIK_LEFT);
	bool stick = fabs(lx) > deadZone;

	// 接地状態ならジャンプ回数リセット
	if (onGround_) {
		jumpCount_ = 0;
	} else {
		// 落下速度
		velocity_ = Add(velocity_, Vector3(0, -kGravityAcceleration / 60.0f, 0));

		// ★ここが重要：地面にいない時だけ壁ずり落ちを適用する
		if (tachWall_ && !Input::GetInstance()->PushKey(DIK_DOWN)) {
			// 壁に触れているときは落下速度を抑える
			velocity_.y = std::max(velocity_.y, -kWallSlideSpeed);
		}

		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

	// キー入力 (左右移動)
	if (keyRight || keyLeft || stick) {
		Vector3 acceleration = {};

		if (keyRight) {
			// 右キーが押されている
			if (velocity_.x < 0.0f) {
				velocity_.x *= (1.0f - kAttenution);
				//// 旋回時の角度
				// turnFirstRotationY_ = worldTransform_.rotation_.y;
				//// 旋回タイマー初期化
				// turnTimer_ = kTimeTurn;
			}
			acceleration.x += kAcceleration;
			if (!isAttack() && GetDirection() != LRDirection::kRight) {
				// 攻撃している時は向きを変えない
				directionCtrl_.SetDirection(LRDirection::kRight);
			}
		} else if (keyLeft) {
			// 左キーが押されている
			if (velocity_.x > 0.0f) {
				velocity_.x *= (1.0f - kAttenution);
			}
			acceleration.x -= kAcceleration;
			if (!isAttack() && GetDirection() != LRDirection::kLeft) {
				// lrDirection_ = LRDirection::kLeft;
				//// 旋回時の角度
				// turnFirstRotationY_ = worldTransform_.rotation_.y;
				//// 旋回タイマー初期化
				// turnTimer_ = kTimeTurn;
				directionCtrl_.SetDirection(LRDirection::kLeft);
			}
		} else if (stick) {
			acceleration.x += (lx / 32767.0f) * kAcceleration;

			if (lx > 0 && GetDirection() != LRDirection::kRight) {
				if (!isAttack()) {
					directionCtrl_.SetDirection(LRDirection::kRight);
				}

				/*turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = kTimeTurn;*/
			} else if (lx < 0 && GetDirection() != LRDirection::kLeft) {
				if (!isAttack()) {
					directionCtrl_.SetDirection(LRDirection::kLeft);
				}
				/*turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = kTimeTurn;*/
			}
		}

		velocity_ = Add(velocity_, acceleration);
		// 最大速度
		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
	} else {
		// 減速
		velocity_.x *= (1.0f - kAttenution);
	}
	//
	if (std::abs(velocity_.x) <= 0.0001f) {
		velocity_.x = 0.0f;
	}

	// ---------------------------------------------------------
	// ジャンプ処理
	// ---------------------------------------------------------
	if ((Input::GetInstance()->TriggerKey(DIK_UP)) || ((state_.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_A))) {

		// 優先順位1: 地面にいるなら「通常ジャンプ」
		// 床と壁の両方に触れている場合、ここで引っかかり、下の壁ジャンプは無視されます。
		if (onGround_) {
			velocity_.y += kJumpAcceleration / 60.0f; // Addではなく直接加算かY成分の上書きを推奨

			// アニメーション用フラグをセット
			isJumping_ = true;
			jumpParameter_ = 0;
			isLanding_ = false; // 着地演出と被らないようにリセット

			/*if (!Audio::GetInstance()->IsPlaying(jumpSEHandle_)) {
			    Audio::GetInstance()->PlayWave(jumpSEHandle_, false);
			}*/
			// 通常ジャンプ時のX速度は維持（必要ならここで調整）
		}
		// 優先順位2: 地面にいなくて、壁に触れているなら「壁ジャンプ」
		else if (tachWall_) {
			// 反対方向に弾くような壁ジャンプ
			if (GetDirection() == LRDirection::kRight) {
				velocity_ = {-kJumpAcceleration / 120.0f, kJumpAcceleration / 60.0f, 0};
			} else {
				velocity_ = {+kJumpAcceleration / 120.0f, kJumpAcceleration / 60.0f, 0};
			}
			// 壁ジャンプ時はジャンプ回数をリセット
			jumpCount_ = 0;
			/*if (!Audio::GetInstance()->IsPlaying(jumpSEHandle_)) {
			    Audio::GetInstance()->PlayWave(jumpSEHandle_, false);
			}*/
		}
		// 優先順位3: それ以外（空中にいて壁にも触れていない）なら「空中ジャンプ」
		else if (jumpCount_ < kLimitJumpCount) {
			velocity_.y += (kJumpAcceleration) / 60.0f;
			jumpCount_++; // なってなければジャンプSE再生
			if (!Audio::GetInstance()->IsPlaying(jumpSEPlayHandle_)) {
				jumpSEPlayHandle_=Audio::GetInstance()->PlayWave(jumpSEHandle_, false);
			}
		}
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

void Player::UpdateOnGround(const CollisionMapInfo& info) {

	if (onGround_) {
		// info; // この行は削除してもOK
		if (velocity_.y > 0.0f) {
			onGround_ = false;
			isLanding_ = false;
		} else {
			// 落下判定
			std::array<Vector3, kNumCorner> positionsNew;

			// ▼▼▼ 修正: + info.move を削除しました ▼▼▼
			// 移動後の各頂点座標を計算 (worldTransform_.translation_ は既に移動済み)
			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
			}
			// ▲▲▲ 修正ここまで ▲▲▲

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
				isLanding_ = false; // 落下したら着地モーションキャンセル
			}
		}

	} else {
		// 地面に接触している場合
		if (info.isFloor) {
			if (!onGround_) {
				// 空中から地面に着いた瞬間
				isLanding_ = true;
				landingParameter_ = 0;
			}
			onGround_ = true;
			// 着地時の速度を0にする
			velocity_.y = 0.0f;

			velocity_.x *= (1.0f - kAttenuationGround);
		}
	}
}

void Player::HitWall(const CollisionMapInfo& info) {

	// 1. 移動による物理衝突があった場合（最優先）
	if (info.isWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
		if (!tachWall_) {
			isWallHit_ = true;
			wallHitParameter_ = 0;
		}
		tachWall_ = true;
		return; // 確実に壁に触れているのでここで終了
	}

	// 2. 物理衝突がない場合でも、見た目上壁に接しているかを判定する（チャタリング防止）
	// 接地判定(UpdatOnGround)と同様に、少し横を調べる

	const float kWallSearchDistance = 0.06f; // 壁判定を行う距離（kGroundSearchHeightと同程度）
	bool hit = false;

	// 現在の各頂点座標を取得（移動補正後の座標）
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
	}

	// 左右どちらを調べるか？（入力方向や向きで判断、あるいは両方）
	// ここではシンプルに「現在の向き(lrDirection_)」の方向を調べます
	if (GetDirection() == LRDirection::kRight) {
		// 右側の壁をチェック（右下と右上）
		MapChipField::IndexSet indexSet;
		MapChipType mapChipType;

		// 右下
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(kWallSearchDistance, 0, 0));
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipType::kBlock)
			hit = true;

		// 右上
		if (!hit) {
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop] + Vector3(kWallSearchDistance, 0, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock)
				hit = true;
		}
	} else if (GetDirection() == LRDirection::kLeft) {
		// 左側の壁をチェック（左下と左上）
		MapChipField::IndexSet indexSet;
		MapChipType mapChipType;

		// 左下
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(-kWallSearchDistance, 0, 0));
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipType::kBlock)
			hit = true;

		// 左上
		if (!hit) {
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop] + Vector3(-kWallSearchDistance, 0, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock)
				hit = true;
		}
	}

	// 判定結果を反映
	tachWall_ = hit;

	// 壁に張り付いているなら速度を抑える（オプション）
	if (tachWall_) {
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

Vector3 Player::GetAttackPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransformAttack_.translation_.x;
	worldPos.y = worldTransformAttack_.translation_.y;
	worldPos.z = worldTransformAttack_.translation_.z;
	return worldPos;
}

AABB Player::GetAttackAABB() {
	Vector3 worldPos = GetAttackPosition();
	AABB aabb;
	aabb.min = {worldPos.x - kAttackWidth / 2.0f, worldPos.y - kAttackHeight / 2.0f, worldPos.z - kAttackWidth / 2.0f};
	aabb.max = {worldPos.x + kAttackWidth / 2.0f, worldPos.y + kAttackHeight / 2.0f, worldPos.z + kAttackWidth / 2.0f};
	return aabb;
}

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}

void Player::OnCollision(const Enemy* enemy) {

	Vector3 enemyPos = enemy->GetWorldTransform().translation_;
	Vector3 playerPos = GetWorldPosition();
	if (isAttack() || invincibleTimer_ > 0) {

		return;
	}
	(void)enemy;
	invincibleTimer_ = 120;
	knockbackTimer_ = 20;

	// 2. ノックバック方向の計算 (敵から離れる方向)
	float knockbackForceX = 0.2f; // 左右の弾き飛ばし強度
	float knockbackForceY = 0.1f; // 上への跳ね返り強度

	if (playerPos.x < enemyPos.x) {
		// 敵が右にいるので左へ
		velocity_.x = -knockbackForceX;
	} else {
		// 敵が左にいるので右へ
		velocity_.x = knockbackForceX;
	}

	// 上方向にも少し跳ね上げる（地面にめり込まないようにするため）
	velocity_.y = knockbackForceY;
	// isDead_ = true;

	// velocity_+=Vector3(0.0f,kJumpAcceleration/60.0f,0.0f);
}
