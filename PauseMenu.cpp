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
	if (!overlay_) {
		// テクスチャ0番（通常は白など）を使ってスプライト生成
		overlay_ = Sprite::Create(0, Vector2{0.0f, 0.0f});

		// 画面全体を覆うサイズに設定
		overlay_->SetSize(Vector2(WinApp::kWindowWidth, WinApp::kWindowHeight));

		// 半透明の黒に設定 (R, G, B, A)
		// 0.5f で50%の透け感になります
		overlay_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.5f));
	}
}

PauseResult PauseMenu::Update() {
	Input::GetInstance()->GetJoystickStatePrevious(0, prevState_);
	Input::GetInstance()->GetJoystickState(0, state_);
	ImGuiManager::GetInstance()->Begin();
	// #ifdef DEBUG

	ImGui::Begin("Pause Menu");
	ImGui::Text("== PAUSE ==");
	if (cursor_ == 0) {
		ImGui::Text("> Resume");
		ImGui::Text("  Go to Title");
	} else {
		ImGui::Text("  Resume");
		ImGui::Text("> Go to Title");
	}

	ImGui::End();
	// #endif // DEBUG

	ImGuiManager::GetInstance()->End();

	// PキーでポーズのON/OFF切り替え
	if (Input::GetInstance()->TriggerKey(DIK_P) || ((state_.Gamepad.wButtons & XINPUT_GAMEPAD_START) && !(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_START))) {
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
	if (Input::GetInstance()->TriggerKey(DIK_UP) || (state_.Gamepad.sThumbLY > 20000 && prevState_.Gamepad.sThumbLY <= 20000)) {
		cursor_--;
		if (cursor_ < 0)
			cursor_ = 1;
	}

	// 下入力: キーボード↓ OR スティック下倒し
	if (Input::GetInstance()->TriggerKey(DIK_DOWN) || (state_.Gamepad.sThumbLY < -20000 && prevState_.Gamepad.sThumbLY >= -20000)) {
		cursor_++;
		if (cursor_ > 1)
			cursor_ = 0;
	}

	// 決定 (スペースキー)
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || ((state_.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_A))) {
		if (cursor_ == 0) {
			// ゲームに戻る
			isPaused_ = false;
			return PauseResult::kResume;
		} else if (cursor_ == 1) {
			// タイトルへ
			isPaused_ = false;
			return PauseResult::kGoTitle;
		}
	}

	return PauseResult::kNone;
}

void PauseMenu::Draw() {
	// ポーズ中じゃなければ描画しない
	if (!isPaused_)
		return;

	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	//// 背景（半透明の黒）
	if (overlay_) {
		overlay_->Draw();
	}
	Sprite::PostDraw();

	// 文字描画
	// ※座標は適宜調整してください

	ImGuiManager::GetInstance()->Draw();
}