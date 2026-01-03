#include "ResultMenu.h"

using namespace KamataEngine;

ResultMenu::ResultMenu() {}

ResultMenu::~ResultMenu() {
	if (overlay_) {
		delete overlay_;
	}
}

void ResultMenu::Initialize(bool isClear) {
	isClear_ = isClear;
	cursor_ = 0;

	// 背景がまだなければ生成（PauseMenuと同じ設定）
	if (!overlay_) {
		overlay_ = Sprite::Create(0, Vector2{ 0.0f, 0.0f });
		overlay_->SetSize(Vector2(WinApp::kWindowWidth, WinApp::kWindowHeight));
		overlay_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.5f)); // 半透明の黒
	}
}

ResultMenu::ResultSelection ResultMenu::Update() {
	// ImGuiの描画
	ImGuiManager::GetInstance()->Begin();
	ImGui::Begin("Result Menu");

	if (isClear_) {
		ImGui::Text("== STAGE CLEAR ==");
		if (cursor_ == 0) {
			ImGui::Text("> Next Stage");
			ImGui::Text("  Go to Title");
		} else {
			ImGui::Text("  Next Stage");
			ImGui::Text("> Go to Title");
		}
	} else {
		ImGui::Text("== GAME OVER ==");
		if (cursor_ == 0) {
			ImGui::Text("> Retry");
			ImGui::Text("  Go to Title");
		} else {
			ImGui::Text("  Retry");
			ImGui::Text("> Go to Title");
		}
	}

	ImGui::End();
	ImGuiManager::GetInstance()->End();

	// カーソル移動 (上下キー)
	if (Input::GetInstance()->TriggerKey(DIK_UP)) {
		cursor_--;
		if (cursor_ < 0) cursor_ = 1;
	}
	if (Input::GetInstance()->TriggerKey(DIK_DOWN)) {
		cursor_++;
		if (cursor_ > 1) cursor_ = 0;
	}

	// 決定 (スペースキー または Aボタン)
	// ※パッド対応も含める場合はここにパッド入力判定も追加してください
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		if (isClear_) {
			// クリア時の分岐
			if (cursor_ == 0) return ResultSelection::kNext;
			if (cursor_ == 1) return ResultSelection::kTitle;
		} else {
			// ゲームオーバー時の分岐
			if (cursor_ == 0) return ResultSelection::kRetry;
			if (cursor_ == 1) return ResultSelection::kTitle;
		}
	}

	return ResultSelection::kNone;
}

void ResultMenu::Draw() {
	// 半透明背景の描画
	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	if (overlay_) {
		overlay_->Draw();
	}
	Sprite::PostDraw();

	// ImGui描画
	ImGuiManager::GetInstance()->Draw();
}