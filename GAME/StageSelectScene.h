#pragma once
#include "Fade.h"
#include "IScene.h"
#include "KamataEngine.h"
#include "StageManager.h"

using namespace KamataEngine;

class StageSelectScene : public IScene {
public:
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン
		kFadeOut, // フェードアウト
	};
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	// 【変更】計算しやすいように int に変更します
	int selectStageNo_ = 1;

	ImGuiManager* imgui_ = ImGuiManager::GetInstance();
	
	Phase phase_ = Phase::kFadeIn; 

	 XINPUT_STATE state_;
	 XINPUT_STATE prevState_;
};