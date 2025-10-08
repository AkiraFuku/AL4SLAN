#pragma once
#include <KamataEngine.h>
using namespace KamataEngine;

class Player;
class CameraController {
public:
	struct Rect {
		float left = 0.0f; // 左端
		float right = 1.0f; // 右端
		float bottom = 0.0f; // 下端
		float top = 1.0f;    // 上端
	};
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Camera *camera );

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void SetTarget(Player* target) { target_ = target; }
	void SetMoveArea(const Rect area) { moveArea_ = area; }

	void Reset();

	private:
		// カメラ
		Camera* camera_=nullptr;
		Player* target_=nullptr;
	    Rect moveArea_ = {0.0f, 100.0f, 0.0f, 100.0f};
		Vector3 targetOffset_ = {0.0f, 0.0f, -15.0f};
	    Vector3 desetination_ ;
	    static inline const float kInterpolationRate = 0.1f; // 
	    static inline const float kVelocityBias = 30.0f;     // 
		static inline const Rect targetMargin_ = {-9.0f, 9.0f, -5.0f, 5.0f}; // ターゲットのマージン


};
