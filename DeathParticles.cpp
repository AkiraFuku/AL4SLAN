#include "DeathParticles.h"
#include "MassFunction.h"
#include <algorithm>
void DeathParticles::Initialze(Model* model, Camera* camera, const Vector3& position) {
model_=model;
camera_=camera;
for (WorldTransform& worldTransform:worldTransforms_) {
	worldTransform.Initialize();
	worldTransform.translation_=position;
}
objectColor_.Initialize();
color_={1,1,1,1};

}
void DeathParticles::Update(){

	if (isFinished_) {
		return;
	}

	//カウンターの時間経過
	countor_+=1.0f/60.0f;
	//時間判定
	if (countor_>=kDuration_) {
		countor_=kDuration_;
		isFinished_= true;
	}

	for (uint32_t i = 0; i < kNumParticles; ++i) {
		///速度ベクトル
		Vector3 velocity={kSpeed_,0.0f,0.0f};
		// 回転角の計算
		float angle = kAngleUnit_*i;
		//ｚ軸まわり回転行列
		Matrix4x4 matrixRotation=MakeRotateZMatrix(angle);
		velocity =Transform(velocity,matrixRotation);
		//移動処理
		worldTransforms_[i].translation_+=velocity;
	}
	//カラー
	color_.w=std::clamp(1.0f - countor_ / kDuration_, 0.0f, 1.0f);
	objectColor_.SetColor(color_);


for (WorldTransform& worldTransform:worldTransforms_) {
 WorldTransformUpdate(&worldTransform);
}


}
void DeathParticles::Draw(){
	if (isFinished_) {
		return;
	}

	for (WorldTransform& worldTransform:worldTransforms_) {
	model_->Draw(worldTransform,*camera_,&objectColor_);
	}

}
