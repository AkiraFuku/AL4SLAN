#pragma once
#include "KamataEngine.h"
#include <algorithm>
#include <numbers>
enum class LRDirection {
	kRight,
	kLeft,
};
class Direction {

	public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="initialDirection">初期の向き</param>
	void Initialize(LRDirection initialDirection);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 向きを指定（変更があれば旋回開始）
	/// </summary>
	/// <param name="direction">新しい向き</param>
	void SetDirection(LRDirection direction);

	/// <summary>
	/// 即座に向きを確定させる（攻撃時など）
	/// </summary>
	void ImmediateTurn();

	/// <summary>
	/// 現在のY軸回転角を取得
	/// </summary>
	float GetYRotation() const;

	/// <summary>
	/// 現在の向きを取得
	/// </summary>
	LRDirection GetDirection() const { return currentDirection_; }

private:
	

private:
	LRDirection currentDirection_ = LRDirection::kRight;

	float currentAngle_ = 0.0f;     // 現在の角度
	float startAngle_ = 0.0f;       // 旋回開始時の角度
	float targetAngle_ = 0.0f;      // 目標角度

	float turnTimer_ = 0.0f;        // 旋回タイマー
	static inline const float kTimeTurn = 0.3f; // 旋回にかかる時間
};
