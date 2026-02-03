#pragma once
#include <KamataEngine.h>
using namespace KamataEngine;
// ポーズメニューでの操作結果を表す定数
enum class PauseResult {
	kNone,   // 何も決定していない
	kResume, // ゲームに戻る（再開）
	kSelect, // ステージセレクト
	kGoTitle // タイトルへ戻る
};
class PauseMenu {
public:
	// コンストラクタ・デストラクタ
	PauseMenu();
	~PauseMenu();

	void Initialize();

	// 更新処理（戻り値で操作結果を返す）
	PauseResult Update();

	// 描画処理
	void Draw();

	// ポーズ中かどうかを取得
	bool IsPaused() const { return isPaused_; }

private:
	// ポーズ中フラグ
	bool isPaused_ = false;

	// メニューのカーソル位置 (0:再開, 1:タイトル)
	int cursor_ = 0;

	// 背景を暗くするためのスプライト
	Sprite* overlay_ = nullptr;
	XINPUT_STATE state_;
	XINPUT_STATE prevState_;


	Sprite* spriteResume_ = nullptr;
	Sprite* spriteSelect_ = nullptr;
	Sprite* spriteTitle_ = nullptr;
	
	// カーソル（矢印など）のスプライト
	Sprite* spriteCursor_ = nullptr;

	// テクスチャハンドル
	uint32_t texResume_ = 0;
	uint32_t texSelect_ = 0;
	uint32_t texTitle_ = 0;
	uint32_t texCursor_ = 0;

};
