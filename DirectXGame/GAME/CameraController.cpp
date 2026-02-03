#include <algorithm>  
#include "CameraController.h"  
#include "Player.h"  
#include "MassFunction.h"  

#include <iostream>

using namespace KamataEngine;  
using namespace std;
void CameraController::Initialize(Camera *camera){  
	camera_ = camera;  
}  

void CameraController::Update(){  
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();  

	if (isClearPhase_) {
		// --- クリアフェーズ：プレイヤーを中央に捉えてズーム ---
        
        // 1. 目標地点は「プレイヤーの座標 + クリア用オフセット」
        // 先読み（velocity）は入れないことで中央に固定する
        desetination_ = targetWorldTransform.translation_ + clearOffset_;

        // 2. 線形補間(Lerp)で滑らかに移動させる
        // 0.1f は追従速度。お好みで調整してください
        camera_->translation_ = Lerp( camera_->translation_, desetination_, 0.1f);
	} else {
	

	
	desetination_ =targetWorldTransform.translation_+targetOffset_+target_->getVelocity()*kVelocityBias;
	camera_->translation_ = Lerp(camera_->translation_, desetination_, 0.1f); // 緩やかに追従するように補間
	if (shakeTimer_ > 0.0f) {
        // 【変更点】乱数(rand)ではなく、sin波を使ってゆっくり揺らす
        // 係数(20.0fなど)を小さくすると、もっとゆっくりになります
        float frequency = 5.5f; // 揺れの速さ（周波数）
        
        // 時間経過で滑らかに変化する値を作成
        float offsetX = std::sin(shakeTimer_ * frequency) * shakePower_;
        float offsetY = std::cos(shakeTimer_ * frequency) * shakePower_;
        
        // カメラ座標に加算
        camera_->translation_.x += offsetX;
        camera_->translation_.y += offsetY;

        // タイマーを減らす
        shakeTimer_ -= 1.0f / 60.0f; // 120.0fだと減りが遅いので、60fps基準なら60.0fが自然です
    }

	//Move move={{desetination_.x+targetMargin_.left,desetination_.y+targetMargin_.bottom},{desetination_.x+targetMargin_.right,desetination_.y+targetMargin_.top}};

	camera_->translation_.x=max(camera_->translation_.x,
		desetination_.x+targetMargin_.left);
	camera_->translation_.x=min(camera_->translation_.x,
		desetination_.x+targetMargin_.right);
	camera_->translation_.y=max(camera_->translation_.y,
		desetination_.y+targetMargin_.bottom);
	camera_->translation_.y=min(camera_->translation_.y,
		desetination_.y+targetMargin_.top);

//	camera_->translation_.x =clamp(camera_->translation_.x,moveArea_.right, moveArea_.left);
//	camera_->translation_.y =clamp(camera_->translation_.y,moveArea_.bottom, moveArea_.top);
	// 修正: std::max と std::min を使用するために std:: を明示的に指定  
	camera_->translation_.x = max(camera_->translation_.x, moveArea_.left); // x座標を0以上に制限  
	camera_->translation_.x = min(camera_->translation_.x, moveArea_.right); // x座標をmoveAreaの右端以下に制限  
	camera_->translation_.y = min(camera_->translation_.y, moveArea_.bottom); // y座標を0以上に制限  
	camera_->translation_.y = max(camera_->translation_.y, moveArea_.top); // y座標をmoveAreaの下端以下に制限  
	}

	camera_->UpdateMatrix();  
}  

void CameraController::Reset() {  
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();  
	// 必要に応じて targetWorldTransform を使用して処理を追加  
	camera_->translation_ = Add(targetWorldTransform.translation_, targetOffset_);  
	
}

void CameraController::RequestShake(float duration, float power){
    shakeTimer_ = duration;
    shakePower_ = power;
}
