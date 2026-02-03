#pragma once
#include "KamataEngine.h"
#include <array>
#include <numbers>
using namespace KamataEngine;
class DeathParticles {
public:
	void Initialze(Model* model,Camera* camera,const Vector3& position );
	void Update();
	void Draw();
	bool IsFinished()  { return isFinished_; }

private:
	Model*model_=nullptr;
	Camera*camera_=nullptr;

	static inline const uint32_t kNumParticles=8;
	std::array<WorldTransform,kNumParticles> worldTransforms_;
	//存続時間＜秒＞
	static inline const float kDuration_=2.0f;
	//　移動速度
	static inline const float kSpeed_=0.05f;
	// 分裂した一個分の角度
	static inline const float kAngleUnit_=std::numbers::pi_v<float>*2.0f/kNumParticles ;
	//終了フラグ
	bool isFinished_=false;
	//経過時間
	float countor_=0.0f;

	ObjectColor objectColor_;
	Vector4 color_;


};
