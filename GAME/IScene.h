#pragma once

// シーンの種類を定義
enum class SceneType {
	// kUnknown = 0, // 不明なシーン
	kTitle,
	kGame,
	kPause,  // ポーズシーン
	kEnd,    // 終了シーン
	kSelect, // セレクトシーン
};

class IScene {
public:
	virtual ~IScene() {};
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
};