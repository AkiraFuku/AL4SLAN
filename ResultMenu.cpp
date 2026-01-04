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
	memset(&state_, 0, sizeof(XINPUT_STATE));
	memset(&prevState_, 0, sizeof(XINPUT_STATE));
	// 背景がまだなければ生成（PauseMenuと同じ設定）
	if (!overlay_) {
		overlay_ = Sprite::Create(0, Vector2{0.0f, 0.0f});
		overlay_->SetSize(Vector2(WinApp::kWindowWidth, WinApp::kWindowHeight));
		overlay_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.5f)); // 半透明の黒
	}
}

ResultMenu::ResultSelection ResultMenu::Update() {

	prevState_ = state_;

	// (2) 新しい入力を state_ に取得する
	Input::GetInstance()->GetJoystickState(0, state_);
	// ImGuiの描画

	ImGuiManager::GetInstance()->Begin();
	ImGui::Begin("Result Menu");

	if (isClear_) {
		ImGui::Text("== STAGE CLEAR ==");
		// クリア時の表示分岐
		switch (cursor_) {
		case 0: // Next
			ImGui::Text("> Next Stage");
			ImGui::Text("  Go to Select");
			ImGui::Text("  Go to Title");
			break;
		case 1: // Select
			ImGui::Text("  Next Stage");
			ImGui::Text("> Go to Select");
			ImGui::Text("  Go to Title");
			break;
		case 2: // Title
			ImGui::Text("  Next Stage");
			ImGui::Text("  Go to Select");
			ImGui::Text("> Go to Title");
			break;
		}
	} else {
		ImGui::Text("== GAME OVER ==");
		// ゲームオーバー時の表示分岐
		switch (cursor_) {
		case 0: // Retry
			ImGui::Text("> Retry");
			ImGui::Text("  Go to Select");
			ImGui::Text("  Go to Title");
			break;
		case 1: // Select
			ImGui::Text("  Retry");
			ImGui::Text("> Go to Select");
			ImGui::Text("  Go to Title");
			break;
		case 2: // Title
			ImGui::Text("  Retry");
			ImGui::Text("  Go to Select");
			ImGui::Text("> Go to Title");
			break;
		}
	}

	ImGui::End();
	ImGuiManager::GetInstance()->End();
	// カーソル移動 (上下キー)
	if (Input::GetInstance()->TriggerKey(DIK_UP) || (state_.Gamepad.sThumbLY > 20000 && prevState_.Gamepad.sThumbLY <= 20000)) {
		cursor_--;
		if (cursor_ < 0) cursor_ = 2;
	}
	if (Input::GetInstance()->TriggerKey(DIK_DOWN) || (state_.Gamepad.sThumbLY < -20000 && prevState_.Gamepad.sThumbLY >= -20000)) {
		cursor_++;
		if (cursor_ > 2) cursor_ = 0;
	}

	// 決定 (スペースキー または Aボタン)
	// ※パッド対応も含める場合はここにパッド入力判定も追加してください
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || ((state_.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_A))) {
		if (isClear_) {
			switch (cursor_) {
			case 0: return ResultSelection::kNext;
			case 1: return ResultSelection::kSelect; // 追加
			case 2: return ResultSelection::kTitle;
			}
		} else {
			switch (cursor_) {
			case 0: return ResultSelection::kRetry;
			case 1: return ResultSelection::kSelect; // 追加
			case 2: return ResultSelection::kTitle;
			}
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