#include "Fade.h"  
#include <algorithm>  

// ★追加: インスタンス取得関数の実装
Fade* Fade::GetInstance() {
	static Fade instance;
	return &instance;
	// 既に作成済みなら再作成しないようにガード
	
}

// ★追加: デストラクタ（終了時にスプライトを解放）
Fade::~Fade() {
	if (sprite_) {
		delete sprite_;
		sprite_ = nullptr;
	}

}
void Fade::Initialize(){  
if (!sprite_) {
		sprite_ = Sprite::Create(0, Vector2{});
		sprite_->SetSize(Vector2(WinApp::kWindowWidth, WinApp::kWindowHeight));
		sprite_->SetColor(Vector4(0, 0, 0, 1.0f));
	}
};  
void Fade::Update(){  
	switch (status_) {  
	case Fade::Status::None:  
		break;  
	case Fade::Status::FadeIn:  
		// カウンターを進める  
		counter_ += 1.0f / 60.0f;  
		// カウンターがdurationを超えた打ち止め  
		if (counter_>= duration_) {  
			counter_ = duration_;  
		}  

		// アルファ値を計算
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(1.0f - counter_ / duration_, 0.0f, 1.0f)));

		break;  
	case Fade::Status::FadeOut:  
		// FadeOutの処理  
		// カウンターを進める  
		counter_ += 1.0f / 60.0f;  
		// カウンターがdurationを超えた打ち止め  
		if (counter_>= duration_) {  
			counter_ = duration_;  
		}  
		// アルファ値を計算  
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));  
		break;  
	}  
};  
void Fade::Draw() {  
	if (status_== Status::None) {
		return;  
	}
	if (!sprite_) return;
	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());  
	sprite_->Draw();  
	Sprite::PostDraw();  
}  
void Fade::Start(Status status, float duration) {  
	status_= status;  
	duration_ = duration;  
	counter_ = 0.0f;  
}
void Fade::Stop() {
	status_= Status::None;

}
bool Fade::IsFinished() const {
	switch (status_) {

	case Fade::Status::FadeIn:
	case Fade::Status::FadeOut:
		if (counter_>= duration_) {
			return true;  
		} else {  
			return false;  
		}	
	}
	return true;
};
