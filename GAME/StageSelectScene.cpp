#include "StageSelectScene.h"
#include "SceneManager.h"

void StageSelectScene::Initialize() {
    selectStageNo_ = 1;
}

void StageSelectScene::Update() {
    // --- ステージ選択処理 ---
    	imgui_->Begin();
    ImGui::Begin("Stage Select");
    ImGui::Text("Select Stage: %d", selectStageNo_);
    ImGui::Text("Use LEFT/RIGHT to change stage.");
    ImGui::Text("Press SPACE to start the game.");
	ImGui::End();
   
    // 右キーで番号を増やす
    if (Input::GetInstance()->TriggerKey(DIK_RIGHT)) {
        selectStageNo_++;
        // 最大値を超えたら1に戻す（ループさせる）
        if (selectStageNo_ > kMaxStage_) {
            selectStageNo_ = 1;
        }
    }
    
    // 左キーで番号を減らす
    if (Input::GetInstance()->TriggerKey(DIK_LEFT)) {
        selectStageNo_--;
        // 1未満になったら最大値にする
        if (selectStageNo_ < 1) {
            selectStageNo_ = kMaxStage_;
        }
    }

    // --- 決定処理 ---
    if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
        // 1. SceneManagerに選んだステージ番号をセットする
        // (SceneManagerに SetCurrentStage 関数を作っておく必要があります)
        SceneManager::GetInstance()->SetCurrentStage(selectStageNo_);

        // 2. ゲームシーンへ切り替え
        SceneManager::GetInstance()->ChangeScene(SceneType::kGame);
    }
    
    imgui_->End();
}

void StageSelectScene::Draw() {
    // 画面に今の選択番号を表示する（仮で文字表示）
    // ※実際には画像やスプライトを使うとかっこよくなります

    imgui_->Draw();
}