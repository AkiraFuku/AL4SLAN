#include "Skydome.h"
#include "assert.h"

void Skydome::Initialize(Model*model,Camera *camera) {
	assert(model);

	// 引数をメンバ変数に格納
	// モデル
	model_ = model;
	// カメラ
	camera_ = camera;
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
}

void Skydome::Update() { 
	worldTransform_.TransferMatrix();
}

void Skydome::Draw() {
	model_->Draw(worldTransform_, *camera_); 
}
