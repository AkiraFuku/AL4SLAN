#pragma once
#include "KamataEngine.h"

#include "Skydome.h"
#include "Gaid.h"
#include "IScene.h"
using namespace KamataEngine;
class TitleScene: public IScene {
public:
	enum class Phase {

		kFadeIn, // フェードイン
		kMain,   // メイン
		kGaid, //ゲーム説明
		kFadeOut, // フェードアウト

	};

	~TitleScene() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;

	bool IsFinished()  { return finished_; }

private:
	static inline const float kTimeTitleMove = 2.0f;

	// ビュープロジェクション
	Camera camera_;
	WorldTransform worldTransformTitle_;
	WorldTransform worldTransformPlayer_;
	float counter_ = 0.0f;
	bool finished_ = false;
	Model* titleModel_;
	

	Model* playerModel_;
	
	Gaid* gaid_ = nullptr;
	Phase phase_ = Phase::kFadeIn; // フェーズ
	XINPUT_STATE state_;
	XINPUT_STATE prevState_;
	// スカイドーム
	Model* modelSkydome_ = nullptr;
	Skydome* skydome_ = nullptr;

	
	
};
