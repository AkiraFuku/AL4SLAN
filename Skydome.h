#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;
class Skydome {
private:
	WorldTransform worldTransform_; ///< ワールドトランスフォーム
	Model* model_ = nullptr;        ///< モデル
	Camera* camera_ = nullptr;      ///< カメラ
public:
	
	// 初期化
	void Initialize(Model*model,Camera*camera);
	// 更新
	void Update();
	// 描画
	void Draw();
};
