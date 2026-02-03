#include <cassert>
#include <random>
#include <numbers>
#include "HitEffect.h"
#include "MassFunction.h"


Model* HitEffect::model_ = nullptr;
Camera* HitEffect::camera_ = nullptr;


void HitEffect::Initialize(const Vector3& position){
		std::random_device seedGenerator;
		std::mt19937_64 randomEngine;
	randomEngine.seed(seedGenerator());
	std::uniform_real_distribution<float> rotationDistribution(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);

	for (WorldTransform& worldTransform:ellipseWorldTransforms_) {
		//worldTransform.scale_={0.1f,1.5f,1.0f};
		worldTransform.rotation_={0.0f,0.0f,rotationDistribution(randomEngine)};
		worldTransform.translation_=position;
		worldTransform.Initialize();
	}
	circleWorldTransform_.translation_=position;
	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_.z = -1.0f;
	objectColor_.Initialize();
}
void HitEffect::Update(){
	if (IsDead()) {
	 return;
	}
	switch (state_) {
	case HitEffect::State::kSpread:{
		++counter_;
		float scale= 0.5f+ static_cast<float>(counter_)/kSpreadTime*0.5f;
		const float slashScale = 2.0f;
		for (WorldTransform& worldTransform:ellipseWorldTransforms_) {
			worldTransform.scale_={0.1f,scale*slashScale,1.0f};
		}
			const float circleScale = 1.0;

		circleWorldTransform_.scale_ = {scale * circleScale, scale * circleScale, 1.0f};
		if ( counter_ >= kSpreadTime) {
			state_ = State::kFade;
			counter_ = 0; // カウンターをリセット
		}
		break;
	}
	case HitEffect::State::kFade:{
			++counter_;
		objectColor_.SetColor(Vector4{1.0f, 1.0f, 1.0f, 1.0f - static_cast<float>(counter_) / kFadeTime});

		if (++counter_ >= kFadeTime) {
			state_ = State::kDead;
		}

		break;
	}
		default:
		break;
	}

for (WorldTransform& worldTransform:ellipseWorldTransforms_) {
	WorldTransformUpdate(&worldTransform);
}
WorldTransformUpdate(&circleWorldTransform_);
}
void HitEffect::Draw(){
	assert(model_);
	assert(camera_);
	if (IsDead()) {
	 return;
	}
	for (WorldTransform& worldTransform:ellipseWorldTransforms_) {
		model_->Draw(worldTransform,*camera_,&objectColor_);
	}
	model_->Draw(circleWorldTransform_,*camera_,&objectColor_);
}
HitEffect* HitEffect::Create(const Vector3& position){ 
	HitEffect* instance = new HitEffect();
	
	assert(instance);
	instance->Initialize(position);
	return instance;
}