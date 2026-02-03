#include "TitleScene.h"
#include "Fade.h"
#include "MapchipField.h"
#include "MassFunction.h"
#include "SceneManager.h"
#include <numbers>
TitleScene::~TitleScene() {
	delete titleModel_;
	delete playerModel_;
	delete skydome_;
	delete modelSkydome_;
	delete gaid_;
}
void TitleScene::Initialize() {
	titleModel_ = Model::CreateFromOBJ("titleFont", true);
	playerModel_ = Model::CreateFromOBJ("player");
	// カメラ初期化
	camera_.Initialize();
	const float kPlayerTitle = 2.0f;

	worldTransformTitle_.Initialize();

	worldTransformTitle_.scale_ = {kPlayerTitle, kPlayerTitle, kPlayerTitle};

	const float kPlayerScale = 10.0f;

	worldTransformPlayer_.Initialize();

	worldTransformPlayer_.scale_ = {kPlayerScale, kPlayerScale, kPlayerScale};

	worldTransformPlayer_.rotation_.y = 0.65f * std::numbers::pi_v<float>;

	worldTransformPlayer_.translation_.x = -2.0f;

	worldTransformPlayer_.translation_.y = -10.0f;

	Fade::GetInstance()->Start(Fade::Status::FadeIn, 1.0f);

	gaid_ = new Gaid();

	gaid_->Initialize();

	// スカイドームの初期化
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	// ゲーム説明スプライト
	bgmHandle_ = Audio::GetInstance()->LoadWave("Sound/BGM/Soldiers.wav");
	playHandle_ = Audio::GetInstance()->PlayWave(bgmHandle_, true);
	
	Audio::GetInstance()->SetVolume(playHandle_, 0.5f);
};
void TitleScene::Update() {

	Input::GetInstance()->GetJoystickState(0, state_);
	Input::GetInstance()->GetJoystickStatePrevious(0, prevState_);
	switch (phase_) {
	case TitleScene::Phase::kFadeIn:
		Fade::GetInstance()->Update();
		if (Fade::GetInstance()->IsFinished()) {
			phase_ = TitleScene::Phase::kMain;
		}
		break;
	case TitleScene::Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE) || (state_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
			Fade::GetInstance()->Start(Fade::Status::FadeOut, 1.0f);

			phase_ = TitleScene::Phase::kFadeOut;
		
		} else if (((state_.Gamepad.wButtons & XINPUT_GAMEPAD_START)) && !(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_START)) {
			phase_ = TitleScene::Phase::kGaid;
		}

		break;
	case TitleScene::Phase::kGaid:
		if (((state_.Gamepad.wButtons & XINPUT_GAMEPAD_START)) && !(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_START)) {
			phase_ = TitleScene::Phase::kMain;
		}
		break;

	case TitleScene::Phase::kFadeOut:
		Fade::GetInstance()->Update();

	if (bgmVolume_ > 0.0f) {
        bgmVolume_ -= kFadeOutSpeed;
        if (bgmVolume_ < 0.0f) bgmVolume_ = 0.0f;
        
        // 音量を反映 (Audioクラスの仕様に合わせてメソッド名は調整してください)
        Audio::GetInstance()->SetVolume(playHandle_, bgmVolume_);
    }
		if (Fade::GetInstance()->IsFinished()) {
			Audio::GetInstance()->StopWave(playHandle_);
			// フェードアウトが終わったら、次のシーンへ
			if (Audio::GetInstance()->IsPlaying(playHandle_)) {
				Audio::GetInstance()->StopWave(playHandle_);
			}
			SceneManager::GetInstance()->ChangeScene(SceneType::kSelect);
		}
		break;
	};
	counter_ += 1.0f / 60.0f;
	counter_ = std::fmod(counter_, kTimeTitleMove);

	float angle = counter_ / kTimeTitleMove * 2.0f * std::numbers::pi_v<float>;

	worldTransformTitle_.translation_.y = std::sin(angle) + 10.0f;

	worldTransformPlayer_.rotation_.y += (0.65f * std::numbers::pi_v<float>) / 60.0f;

	camera_.TransferMatrix();

	Vector3 skydomeRotate = {skydome_->GetRotation().x, skydome_->GetRotation().y - (0.65f * std::numbers::pi_v<float>) / 60.0f, skydome_->GetRotation().z};
	skydome_->SetRotation(skydomeRotate);
	// スカイドームの更新
	skydome_->Update();

	// アフィン変換～DirectXに転送(タイトル座標)
	WorldTransformUpdate(&worldTransformTitle_);

	// アフィン変換～DirectXに転送（プレイヤー座標）
	WorldTransformUpdate(&worldTransformPlayer_);
};
void TitleScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();
	Model::PreDraw(commandList);
	playerModel_->Draw(worldTransformPlayer_, camera_);
	titleModel_->Draw(worldTransformTitle_, camera_);
	skydome_->Draw();

	Model::PostDraw();
	Fade::GetInstance()->Draw();

	if (phase_ == Phase::kGaid) {
		gaid_->Draw();
	}
};
