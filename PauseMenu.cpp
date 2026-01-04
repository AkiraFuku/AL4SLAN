#include "PauseMenu.h"
PauseMenu::PauseMenu() {}

PauseMenu::~PauseMenu() {
	if (overlay_) {
		delete overlay_;
	}
	if (spriteResume_) delete spriteResume_;
	if (spriteSelect_) delete spriteSelect_;
	if (spriteTitle_)  delete spriteTitle_;
	if (spriteCursor_) delete spriteCursor_;
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

	texResume_ = TextureManager::Load("Resume.png");
	texSelect_ = TextureManager::Load("Select.png");
	texTitle_  = TextureManager::Load("Title.png");
	texCursor_ = TextureManager::Load("Cursor.png");

	// スプライト生成
	spriteResume_ = Sprite::Create(texResume_, { 0, 0 });
	spriteSelect_ = Sprite::Create(texSelect_, { 0, 0 });
	spriteTitle_  = Sprite::Create(texTitle_,  { 0, 0 });
	spriteCursor_ = Sprite::Create(texCursor_, { 0, 0 });



	// スプライトの配置（画面中央付近に縦並び）
	float centerX = WinApp::kWindowWidth / 2.0f;
	float startY = 200.0f; // 最初の項目のY座標
	float gapY = 100.0f;    // 項目間の間隔

	// 位置設定 (画像を中央揃えにする計算)
	// ※KamataEngineにSetAnchorPointがある場合は {0.5, 0.5} にして centerX, startY を直接セットしてください
	// ここでは左上基準と仮定して簡易的に配置します
	spriteResume_->SetPosition({ centerX - 100, startY });
	spriteSelect_->SetPosition({ centerX - 100, startY + gapY });
	spriteTitle_->SetPosition( { centerX - 100, startY + gapY * 2 });
}

PauseResult PauseMenu::Update() {
	Input::GetInstance()->GetJoystickStatePrevious(0, prevState_);
	Input::GetInstance()->GetJoystickState(0, state_);
	ImGuiManager::GetInstance()->Begin();
	// #ifdef DEBUG

	//ImGui::Begin("Pause Menu");
	//ImGui::Text("== PAUSE ==");
	//switch (cursor_) {
	//case 0: // Resume選択中
	//	ImGui::Text("> Resume");
	//	ImGui::Text("  Go to Select");
	//	ImGui::Text("  Go to Title");
	//	break;
	//case 1: // Select選択中
	//	ImGui::Text("  Resume");
	//	ImGui::Text("> Go to Select");
	//	ImGui::Text("  Go to Title");
	//	break;
	//case 2: // Title選択中
	//	ImGui::Text("  Resume");
	//	ImGui::Text("  Go to Select");
	//	ImGui::Text("> Go to Title");
	//	break;
	//}

	//ImGui::End();
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

	// カーソル移動 (0 ～ 2 の範囲でループ)
	if (Input::GetInstance()->TriggerKey(DIK_UP) || (state_.Gamepad.sThumbLY > 20000 && prevState_.Gamepad.sThumbLY <= 20000)) {
		cursor_--;
		if (cursor_ < 0) {
			cursor_ = 2; // 2にループ
		}
	}
	if (Input::GetInstance()->TriggerKey(DIK_DOWN) || (state_.Gamepad.sThumbLY < -20000 && prevState_.Gamepad.sThumbLY >= -20000)) {
		cursor_++;
		if (cursor_ > 2) {
			cursor_ = 0; // 0にループ
		}
	}

	// 決定 (スペースキー)
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || ((state_.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_A))) {
		switch (cursor_) {
		case 0: // Resume
			isPaused_ = false;
			return PauseResult::kResume;
		case 1: // Select
			isPaused_ = false;
			return PauseResult::kSelect;
		case 2: // Title
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

	// === 追加: メニュー項目の描画 ===
	// 全項目を描画（選択されていないものは少し暗くするなどの演出も可能）
	if (spriteResume_) {
		// 選択中は白(通常)、非選択はグレーにする例
		spriteResume_->SetColor(cursor_ == 0 ? Vector4(1, 1, 1, 1) : Vector4(0.5f, 0.5f, 0.5f, 1));
		spriteResume_->Draw();
	}
	if (spriteSelect_) {
		spriteSelect_->SetColor(cursor_ == 1 ? Vector4(1, 1, 1, 1) : Vector4(0.5f, 0.5f, 0.5f, 1));
		spriteSelect_->Draw();
	}
	if (spriteTitle_) {
		spriteTitle_->SetColor(cursor_ == 2 ? Vector4(1, 1, 1, 1) : Vector4(0.5f, 0.5f, 0.5f, 1));
		spriteTitle_->Draw();
	}

	// カーソルの描画
	if (spriteCursor_) {
		// 現在のカーソル位置に合わせてカーソル画像の座標を更新
		float cursorX = (WinApp::kWindowWidth / 2.0f) - 160.0f; // 項目の左側に表示
		float startY = 200.0f;
		float gapY = 100.0f;
		
		// Y座標を cursor_ に応じて計算
		float currentY = startY + (gapY * cursor_);
		
		spriteCursor_->SetPosition({ cursorX, currentY });
		spriteCursor_->Draw();
	}
	Sprite::PostDraw();

	// 文字描画
	// ※座標は適宜調整してください

	ImGuiManager::GetInstance()->Draw();
}