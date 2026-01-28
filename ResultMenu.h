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
	kSelect,  // セレクト画面へ 
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

	XINPUT_STATE state_;
    XINPUT_STATE prevState_;
	// 項目スプライト
	KamataEngine::Sprite* spriteNext_   = nullptr;
	KamataEngine::Sprite* spriteRetry_  = nullptr;
	KamataEngine::Sprite* spriteSelect_ = nullptr;
	KamataEngine::Sprite* spriteTitle_  = nullptr;
	KamataEngine::Sprite* spriteCursor_ = nullptr;

	// テクスチャハンドル
	uint32_t texNext_   = 0;
	uint32_t texRetry_  = 0;
	uint32_t texSelect_ = 0;
	uint32_t texTitle_  = 0;
	uint32_t texCursor_ = 0;
};