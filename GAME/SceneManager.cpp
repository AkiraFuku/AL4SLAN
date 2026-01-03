#include "SceneManager.h"
#include "GameScene.h"
#include "StageSelectScene.h"
#include "KamataEngine.h" // エンジン機能を使うため
#include "TitleScene.h"
#include "Fade.h"
#include <string>
using namespace KamataEngine;

SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

void SceneManager::ChangeScene(SceneType sceneType) {
	nextSceneType_ = sceneType;
	isSceneChanged_ = true;
}

int SceneManager::Run() {
	// 最初のシーンを初期化（例：タイトルから開始）
	ChangeScene(SceneType::kTitle);

	// DirectXの初期化情報の取得などが必要ならここで行う
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	
	StageManager::GetInstance()->RoadStage();
	Fade::GetInstance()->Initialize();
	// --- メインループ ---
	while (true) {
		// エンジンの更新処理（×ボタンで終了した場合などはループを抜ける）
		if (KamataEngine::Update()) {
			break;
		}

		// --- シーン切り替え処理 ---
		if (isSceneChanged_) {
			// 古いシーンの削除
			if (currentScene_) {
				delete currentScene_;
				currentScene_ = nullptr;
			}

			// 新しいシーンの生成
			switch (nextSceneType_) {
			case SceneType::kTitle:
				currentScene_ = new TitleScene();
				break;
			case SceneType::kGame:
				currentScene_ = new GameScene(currentStage_);
				break;

			case SceneType::kSelect:
				currentScene_ = new StageSelectScene();
				break;
			}

			// シーンの初期化
			if (currentScene_) {
				currentScene_->Initialize();
			}
			isSceneChanged_ = false;
		}

		// --- 現在のシーンの更新 ---
		if (currentScene_) {
			currentScene_->Update();
		}

		// --- 描画処理 ---
		dxCommon->PreDraw();
		if (currentScene_) {
			currentScene_->Draw();
		}
		dxCommon->PostDraw();
	}

	// ループを抜けたら終了処理
	if (currentScene_) {
		delete currentScene_;
		currentScene_ = nullptr;


	}
	
	return 0; // 正常終了
}