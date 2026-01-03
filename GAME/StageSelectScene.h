#pragma once
#include "Fade.h"
#include "IScene.h"
#include "KamataEngine.h" // DrawFormatStringなどを使う場合
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
	StageManager* stageMNG_;
	    // 現在選択しているステージ番号（初期値 1）
	    uint32_t selectStageNo_ = 1;
	ImGuiManager* imgui_ = ImGuiManager::GetInstance();
	// 選択できる最大ステージ数（3ステージある場合）
	const int kMaxStage_ = ;
	Phase phase_ = Phase::kFadeIn; // フェーズ
};
