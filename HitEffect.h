#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;
class HitEffect {
public:
	enum class State{
		kSpread,
		kFade,
		kDead
	};

	void Update();
	void Draw() ;
	static void SetModel(Model* model) { model_ = model; }
	static void SetCamera(Camera* camera) { camera_ = camera; }
	static HitEffect* Create(const Vector3& position) ;

	bool IsDead() const {return state_==State::kDead;}
private:
	HitEffect()=default;

	void Initialize(const Vector3& position) ;

	static inline const uint32_t kSpreadTime = 10;

	static inline const uint32_t kFadeTime = 20;

	static inline const uint32_t kLifetime = kSpreadTime + kFadeTime;
	
	static Model* model_ ;
	static Camera* camera_;
	// 円形エフェクト
	WorldTransform circleWorldTransform_;
	//楕円形エフェクト
	static const inline uint32_t kellipseEffectNum = 2;
	std::array<WorldTransform,kellipseEffectNum>ellipseWorldTransforms_;

	State state_=State::kSpread;

	uint32_t counter_=0;

	ObjectColor objectColor_;

};
