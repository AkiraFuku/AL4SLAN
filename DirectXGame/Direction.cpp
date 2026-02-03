#include "Direction.h"
#include "MassFunction.h"
void Direction::Initialize(LRDirection initialDirection) {

	currentDirection_ = initialDirection;
	// 右なら PI/2, 左なら 3PI/2
	if (currentDirection_ == LRDirection::kRight) {
		currentAngle_ = std::numbers::pi_v<float> / 2.0f;
	} else {
		currentAngle_ = std::numbers::pi_v<float> * 3.0f / 2.0f;
	}
	targetAngle_ = currentAngle_;
	turnTimer_ = 0.0f;
}


void Direction::Update() {
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
		if (turnTimer_ <= 0.0f) {
			turnTimer_ = 0.0f;
			currentAngle_ = targetAngle_;
		} else {
			// 時間経過割合 (1.0 -> 0.0 なので、進行度は 1.0 - ratio)
			float t = 1.0f - (turnTimer_ / kTimeTurn);
			currentAngle_ = EaseInOut(startAngle_, targetAngle_, t);
		}
	}
}
void Direction::SetDirection(LRDirection direction) {
// 向きが変わった場合のみ処理
	if (currentDirection_ != direction) {
		currentDirection_ = direction;
		startAngle_ = currentAngle_; // 現在の角度からスタート

		// 目標角度の設定
		if (currentDirection_ == LRDirection::kRight) {
			targetAngle_ = std::numbers::pi_v<float> / 2.0f;
		} else {
			targetAngle_ = std::numbers::pi_v<float> * 3.0f / 2.0f;
		}

		turnTimer_ = kTimeTurn; // タイマーリセット
	}
}

void Direction::ImmediateTurn() {
	turnTimer_ = 0.0f;
	if (currentDirection_ == LRDirection::kRight) {
		currentAngle_ = std::numbers::pi_v<float> / 2.0f;
	} else {
		currentAngle_ = std::numbers::pi_v<float> * 3.0f / 2.0f;
	}
	targetAngle_ = currentAngle_;
}

float Direction::GetYRotation() const { return currentAngle_;}

