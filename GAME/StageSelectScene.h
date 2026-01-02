#pragma once
#include "IScene.h"
#include "KamataEngine.h" // DrawFormatStringなどを使う場合

using namespace KamataEngine;
class StageSelectScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	// 現在選択しているステージ番号（初期値 1）
	int selectStageNo_ = 1;
	ImGuiManager* imgui_ = ImGuiManager::GetInstance();
	// 選択できる最大ステージ数（3ステージある場合）
	const int kMaxStage_ = 2;
};
