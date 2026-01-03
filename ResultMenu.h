#pragma once
#include <KamataEngine.h>

// リザルトメニューでの選択結果


class ResultMenu {
public:
	ResultMenu();
	~ResultMenu();
enum class ResultSelection {
	kNone,    // 選択中
	kNext,    // 次のステージへ
	kRetry,   // リトライ
	kTitle    // タイトルへ戻る
};
	// isClear: trueならクリア画面(Next/Title), falseならゲームオーバー(Retry/Title)
	void Initialize(bool isClear);

	ResultSelection Update();
	void Draw();

private:
	bool isClear_ = false; // クリア状態か否か
	int cursor_ = 0;       // カーソル位置
	
	// 背景用スプライト（ポーズと同様、画面を暗くするため）
	KamataEngine::Sprite* overlay_ = nullptr;
};