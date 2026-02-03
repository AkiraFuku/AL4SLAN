#pragma once
#include <memory>
#include "IScene.h"
#include "KamataEngine.h" // 必要に応じてinclude
#include "StageManager.h"
class SceneManager {
public:
	// シングルトンインスタンス取得
	static SceneManager* GetInstance();

	// ゲームのメインループを実行する関数
	int Run();

	// シーン変更リクエスト
	void ChangeScene(SceneType sceneType);

	// ステージを進める関数を追加
    void NextStage() { currentStage_++; }
    
    // ステージ番号をリセットする関数（タイトルに戻る時など）
    void ResetStage() { currentStage_ = 1; }


	// 現在のステージ番号を取得する関数
	int GetCurrentStage() const { return currentStage_; }
	void SetCurrentStage(int stageNo) { currentStage_ = stageNo; }
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

	int currentStage_ = 1;

	
};