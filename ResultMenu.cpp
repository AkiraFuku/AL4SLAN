#include "ResultMenu.h"

using namespace KamataEngine;

ResultMenu::ResultMenu() {}

ResultMenu::~ResultMenu() {
	if (overlay_) {
		delete overlay_;
	}
	if (spriteNext_)   delete spriteNext_;
	if (spriteRetry_)  delete spriteRetry_;
	if (spriteSelect_) delete spriteSelect_;
	if (spriteTitle_)  delete spriteTitle_;
	if (spriteCursor_) delete spriteCursor_;
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
	// ファイル名は適宜変更してください
	texNext_   = TextureManager::Load("Next.png");
	texRetry_  = TextureManager::Load("Retry.png");
	texSelect_ = TextureManager::Load("Select.png");
	texTitle_  = TextureManager::Load("Title.png");
	texCursor_ = TextureManager::Load("Cursor.png");

	// スプライト生成（未生成の場合のみ作成、または毎回作成して配置リセット）
	// ここでは毎回生成し直すパターン（解放忘れに注意してInitialize内でのnewは慎重に）
	// Initializeが何度も呼ばれる設計なら、nullptrチェックを入れるのが安全です
	if (!spriteNext_)   spriteNext_   = Sprite::Create(texNext_,   { 0, 0 });
	if (!spriteRetry_)  spriteRetry_  = Sprite::Create(texRetry_,  { 0, 0 });
	if (!spriteSelect_) spriteSelect_ = Sprite::Create(texSelect_, { 0, 0 });
	if (!spriteTitle_)  spriteTitle_  = Sprite::Create(texTitle_,  { 0, 0 });
	if (!spriteCursor_) spriteCursor_ = Sprite::Create(texCursor_, { 0, 0 });
}

ResultMenu::ResultSelection ResultMenu::Update() {

	prevState_ = state_;

	// (2) 新しい入力を state_ に取得する
	Input::GetInstance()->GetJoystickState(0, state_);
	// ImGuiの描画

	ImGuiManager::GetInstance()->Begin();
	//ImGui::Begin("Result Menu");

	//if (isClear_) {
	//	ImGui::Text("== STAGE CLEAR ==");
	//	// クリア時の表示分岐
	//	switch (cursor_) {
	//	case 0: // Next
	//		ImGui::Text("> Next Stage");
	//		ImGui::Text("  Go to Select");
	//		ImGui::Text("  Go to Title");
	//		break;
	//	case 1: // Select
	//		ImGui::Text("  Next Stage");
	//		ImGui::Text("> Go to Select");
	//		ImGui::Text("  Go to Title");
	//		break;
	//	case 2: // Title
	//		ImGui::Text("  Next Stage");
	//		ImGui::Text("  Go to Select");
	//		ImGui::Text("> Go to Title");
	//		break;
	//	}
	//} else {
	//	ImGui::Text("== GAME OVER ==");
	//	// ゲームオーバー時の表示分岐
	//	switch (cursor_) {
	//	case 0: // Retry
	//		ImGui::Text("> Retry");
	//		ImGui::Text("  Go to Select");
	//		ImGui::Text("  Go to Title");
	//		break;
	//	case 1: // Select
	//		ImGui::Text("  Retry");
	//		ImGui::Text("> Go to Select");
	//		ImGui::Text("  Go to Title");
	//		break;
	//	case 2: // Title
	//		ImGui::Text("  Retry");
	//		ImGui::Text("  Go to Select");
	//		ImGui::Text("> Go to Title");
	//		break;
	//	}
	//}

	//ImGui::End();
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
	// 配置設定
	float centerX = WinApp::kWindowWidth / 2.0f-100.0f;
	float startY = 200.0f;
	float gapY = 100.0f;

	// スプライトの色や位置を設定して描画
	Sprite* topItem = isClear_ ? spriteNext_ : spriteRetry_;
	
	// 1段目: Next または Retry
	if (topItem) {
		topItem->SetPosition({ centerX - 100, startY });
		topItem->SetColor(cursor_ == 0 ? Vector4(1, 1, 1, 1) : Vector4(0.5f, 0.5f, 0.5f, 1));
		topItem->Draw();
	}

	// 2段目: Select
	if (spriteSelect_) {
		spriteSelect_->SetPosition({ centerX - 100, startY + gapY });
		spriteSelect_->SetColor(cursor_ == 1 ? Vector4(1, 1, 1, 1) : Vector4(0.5f, 0.5f, 0.5f, 1));
		spriteSelect_->Draw();
	}

	// 3段目: Title
	if (spriteTitle_) {
		spriteTitle_->SetPosition({ centerX - 100, startY + gapY * 2 });
		spriteTitle_->SetColor(cursor_ == 2 ? Vector4(1, 1, 1, 1) : Vector4(0.5f, 0.5f, 0.5f, 1));
		spriteTitle_->Draw();
	}

	// カーソル
	if (spriteCursor_) {
		float cursorX = (centerX - 100) - 50.0f; // 項目の左側
		float currentY = startY + (gapY * cursor_);
		spriteCursor_->SetPosition({ cursorX, currentY });
		spriteCursor_->Draw();
	}
	Sprite::PostDraw();

	// ImGui描画
	ImGuiManager::GetInstance()->Draw();
}