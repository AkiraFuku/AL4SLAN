#include "StageSelectScene.h"
#include "SceneManager.h"

void StageSelectScene::Initialize() {
	selectStageNo_ = 1;
	Fade::GetInstance()->Start(Fade::Status::FadeIn, 1.0f);
	phase_ = StageSelectScene::Phase::kFadeIn;
}

void StageSelectScene::Update() {
	// StageManagerから最大ステージ数を取得
	const int kMaxStage = (int)StageManager::GetInstance()->GetStageNum(); // intにキャスト

	// --- ステージ選択処理 ---
	imgui_->Begin();
	ImGui::Begin("Stage Select");
	
	// 【変更】0番なら「タイトルへ」、それ以外ならステージ番号を表示
	if (selectStageNo_ == 0) {
		ImGui::Text("Selection: > Return to Title <"); // タイトル戻る表示
	} else {
		ImGui::Text("Selection: Stage %d", selectStageNo_);
	}

	ImGui::Text("Max Stage: %d", kMaxStage);
	ImGui::Text("Use LEFT/RIGHT to change.");
	ImGui::Text("Press SPACE to decide.");
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
		// 右キー (ステージ番号を進める)
		if (Input::GetInstance()->TriggerKey(DIK_RIGHT)) {
			selectStageNo_++;
			// 最大数を超えたら0（タイトル戻る）にする
			if (selectStageNo_ > kMaxStage) {
				selectStageNo_ = 0;
			}
		}

		// 左キー (ステージ番号を戻す)
		if (Input::GetInstance()->TriggerKey(DIK_LEFT)) {
			selectStageNo_--;
			// 0未満になったら最大ステージにする
			if (selectStageNo_ < 0) {
				selectStageNo_ = kMaxStage;
			}
		}

		// 決定処理
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			// ステージ番号がセットされるが、0の場合は後で判定する
			SceneManager::GetInstance()->SetCurrentStage(selectStageNo_);
			Fade::GetInstance()->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = StageSelectScene::Phase::kFadeOut;
		}
		break;

	case StageSelectScene::Phase::kFadeOut:
		Fade::GetInstance()->Update();
		if (Fade::GetInstance()->IsFinished()) {
			
			// 【修正】0番ならタイトル、それ以外ならゲームへ
			// elseを使わないと両方実行されるバグを防ぎます
			if (selectStageNo_ == 0) {
				SceneManager::GetInstance()->ChangeScene(SceneType::kTitle);
			} else {
				SceneManager::GetInstance()->ChangeScene(SceneType::kGame);
			}
		}
		break;
	}
	imgui_->End();
}

void StageSelectScene::Draw() {
	Fade::GetInstance()->Draw();
	imgui_->Draw();
}