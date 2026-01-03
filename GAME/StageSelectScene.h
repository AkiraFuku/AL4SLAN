#pragma once
#include "Fade.h"
#include "IScene.h"
#include "KamataEngine.h"
#include "StageManager.h" // インクルード

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
	// StageManager* stageMNG_; // GetInstanceを使うのでメンバ変数は不要
	
	uint32_t selectStageNo_ = 1;
	ImGuiManager* imgui_ = ImGuiManager::GetInstance();
	
	// const int kMaxStage_ = ; // ← 削除 (構文エラーの原因)
	
	Phase phase_ = Phase::kFadeIn; 
};