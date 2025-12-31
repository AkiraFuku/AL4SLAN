#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>
#include "SceneManager.h"
using namespace KamataEngine;
//GameScene* gameScene = nullptr;
//TitleScene* titleScene = nullptr;
//enum class Scene {
//
//	kUnknown = 0, // 不明なシーン
//	kTitle,       // タイトルシーン
//	kGame,        // ゲームシーン
//	kPause,       // ポーズシーン
//	kEnd,         // 終了シーン
//
//	kSelect, // セレクトシーン
//
//};
//Scene scene = Scene::kUnknown;
//
//void ChangeScene();
//void UpdateScene();
//void DrawScene();

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize();
	//DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	/*scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();*/
	//SceneManager::GetInstance()->ChangeScene(SceneType::kTitle);
	//
	//// メインループ
	//while (true) {
	//	// メッセージ処理

	//	// エンジンの更新
	//	if (KamataEngine::Update()) {
	//		break; // 終了したらループを抜ける
	//	}
	//	ChangeScene();
	//	UpdateScene();
	//	dxCommon->PreDraw();
	//	DrawScene();
	//	dxCommon->PostDraw();
	//}
	SceneManager::GetInstance()->Run();
	//delete gameScene;
	//delete titleScene;
	KamataEngine::Finalize();
	return 0;
}
//void ChangeScene() {
//	switch (scene) {
//
//	case Scene::kTitle:
//		if (titleScene->IsFinished()) {
//			// シーン変更
//			scene = Scene::kGame;
//			// タイトルシーンの終了処理
//			delete titleScene;
//			titleScene = nullptr;
//			// ゲームシーンのインスタンスを生成
//			gameScene = new GameScene;
//			// ゲームシーンの初期化
//			gameScene->Initialize();
//		}
//		break;
//	case Scene::kGame:
//		if (gameScene->IsGameEnd()) {
//			// シーン変更
//			scene = Scene::kTitle;
//			delete gameScene;
//			gameScene = nullptr;
//			titleScene = new TitleScene;
//			titleScene->Initialize();
//		} else if (gameScene->IsFinished()) {
//			gameScene = nullptr;
//			gameScene = new GameScene;
//			gameScene->Initialize();
//		}
//		break;
//	}
//}
//void UpdateScene() {
//	// シーンの更新処理
//	switch (scene) {
//	case Scene::kTitle:
//		titleScene->Update();
//		break;
//	case Scene::kGame:
//		gameScene->Update();
//		break;
//	}
//}
//void DrawScene() {
//	// シーンの描画処理
//	switch (scene) {
//	case Scene::kTitle:
//		titleScene->Draw();
//		break;
//	case Scene::kGame:
//		gameScene->Draw();
//		break;
//	}
//}
