#pragma once
#include "Fade.h"
#include "IScene.h"
#include "KamataEngine.h"
#include "Skydome.h"
#include "StageManager.h"
using namespace KamataEngine;

class StageSelectScene : public IScene {
public:
	~StageSelectScene() override;
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

	KamataEngine::Camera camera_;   // カメラ
	Skydome* skydome_ = nullptr;    // スカイドーム本体
	Model* modelSkydome_ = nullptr; // スカイドームのモデル
	std::vector<Sprite*> uiSprites_;

	// カーソル（矢印や枠）のスプライト
	Sprite* spriteCursor_ = nullptr;

	// タイトル画像（装飾用："STAGE SELECT"などの文字）
	Sprite* spriteHeader_ = nullptr;

	uint32_t bgmHandle_ = 0;
	uint32_t playHandle_ = 0;
	float bgmVolume_ = 1.0f; // 現在の音量 (1.0 = MAX)
    const float kFadeOutSpeed = 1.0f / 60.0f; // 1秒(60フレーム)でフェードアウトする場合

};