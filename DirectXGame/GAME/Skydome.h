#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;
class Skydome {
private:
	WorldTransform worldTransform_; ///< ワールドトランスフォーム
	Model* model_ = nullptr;        ///< モデル
	Camera* camera_ = nullptr;      ///< カメラ
public:
	
	Vector3 GetPosition() const { return worldTransform_.translation_; }
	Vector3 GetScale() const { return worldTransform_.scale_; }
	Vector3 GetRotation() const { return worldTransform_.rotation_; }
	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; }
	void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; }
	void SetRotation(const Vector3& rotation) { worldTransform_.rotation_ = rotation; }

	// 初期化
	void Initialize(Model*model,Camera*camera);
	// 更新
	void Update();
	// 描画
	void Draw();
};
