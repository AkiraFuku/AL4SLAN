#include "PauseMenu.h"
PauseMenu::PauseMenu() {}

PauseMenu::~PauseMenu() {
	if (overlay_) {
		delete overlay_;
	}
}

void PauseMenu::Initialize() {
	isPaused_ = false;
	cursor_ = 0;

}

PauseResult PauseMenu::Update() {
	ImGuiManager::GetInstance()->Begin();

	
	ImGui::Begin("Pause Menu");
	ImGui::Text("== PAUSE ==");
	if (cursor_ == 0) {
		ImGui::Text("> Resume");
		ImGui::Text("  Go to Title");
	}
	else {
		ImGui::Text("  Resume");
		ImGui::Text("> Go to Title");
	}
	ImGui::End();
	ImGuiManager::GetInstance()->End();

	// PキーでポーズのON/OFF切り替え
	if (Input::GetInstance()->TriggerKey(DIK_P)) {
		isPaused_ = !isPaused_;
		// ポーズ解除なら「再開」として返す
		if (!isPaused_) {
			return PauseResult::kResume;
		}
	}

	// ポーズ中でなければ何もしない
	if (!isPaused_) {
		return PauseResult::kNone;
	}

	// --- 以下、ポーズ中の操作 ---

	// カーソル移動 (上下キー)
	if (Input::GetInstance()->TriggerKey(DIK_UP)) {
		cursor_--;
		if (cursor_ < 0) cursor_ = 1;
	}
	if (Input::GetInstance()->TriggerKey(DIK_DOWN)) {
		cursor_++;
		if (cursor_ > 1) cursor_ = 0;
	}

	// 決定 (スペースキー)
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		if (cursor_ == 0) {
			// ゲームに戻る
			isPaused_ = false;
			return PauseResult::kResume;
		}
		else if (cursor_ == 1) {
			// タイトルへ
			isPaused_ = false;
			return PauseResult::kGoTitle;
		}
	}

	

	return PauseResult::kNone;

}

void PauseMenu::Draw() {
	// ポーズ中じゃなければ描画しない
	if (!isPaused_) return;

	//// 背景（半透明の黒）
	//if (overlay_) {
	//	overlay_->Draw();
	//}

	// 文字描画
	// ※座標は適宜調整してください
	
	ImGuiManager::GetInstance()->Draw();

}