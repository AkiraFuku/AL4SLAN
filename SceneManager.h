#pragma once
#include <memory>
#include "IScene.h"
#include "KamataEngine.h" // 必要に応じてinclude

class SceneManager {
public:
	// シングルトンインスタンス取得
	static SceneManager* GetInstance();

	// ゲームのメインループを実行する関数
	int Run();

	// シーン変更リクエスト
	void ChangeScene(SceneType sceneType);

private:
	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

	// 現在のシーン
	IScene* currentScene_ = nullptr;
	
	// 次のシーン予約用
	SceneType nextSceneType_ = SceneType::kTitle;
	bool isSceneChanged_ = false;
};