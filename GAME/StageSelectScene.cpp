#include "StageSelectScene.h"
#include "SceneManager.h"

void StageSelectScene::Initialize() {
	selectStageNo_ = 1;
	Fade::GetInstance()->Start(Fade::Status::FadeIn, 1.0f);
	phase_ = StageSelectScene::Phase::kFadeIn;
}

void StageSelectScene::Update() {
	// StageManagerから最大ステージ数を取得
	// 配列サイズを取得するが、ステージ番号は1始まりで管理したいのでそのまま使うか調整する
	const uint32_t kMaxStage = StageManager::GetInstance()->GetStageNum();

	// --- ステージ選択処理 ---
	imgui_->Begin();
	ImGui::Begin("Stage Select");
	ImGui::Text("Select Stage: %d", selectStageNo_);
	ImGui::Text("Max Stage: %d", kMaxStage); // デバッグ表示
	ImGui::Text("Use LEFT/RIGHT to change stage.");
	ImGui::Text("Press SPACE to start the game.");
	ImGui::End();

	switch (phase_) {
	case StageSelectScene::Phase::kFadeIn:
		Fade::GetInstance()->Update();
		if (Fade::GetInstance()->IsFinished()) {
			Fade::GetInstance()->Stop();
			phase_ = StageSelectScene::Phase::kMain;
		}
		break;

	case StageSelectScene::Phase::kMain:
		// 右キー
		if (Input::GetInstance()->TriggerKey(DIK_RIGHT)) {
			selectStageNo_++;
			if (selectStageNo_ > kMaxStage) {
				selectStageNo_ = 1;
			}
		}

		// 左キー
		if (Input::GetInstance()->TriggerKey(DIK_LEFT)) {
			selectStageNo_--;
			if (selectStageNo_ < 1) {
				selectStageNo_ = kMaxStage;
			}
		}

		// 決定処理
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			SceneManager::GetInstance()->SetCurrentStage(selectStageNo_);
			Fade::GetInstance()->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = StageSelectScene::Phase::kFadeOut;
		}
		break;

	case StageSelectScene::Phase::kFadeOut:
		Fade::GetInstance()->Update();
		if (Fade::GetInstance()->IsFinished()) {
			SceneManager::GetInstance()->ChangeScene(SceneType::kGame);
		}
		break;
	}
	imgui_->End();
}

void StageSelectScene::Draw() {
	Fade::GetInstance()->Draw();
	imgui_->Draw();
}