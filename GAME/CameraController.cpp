#include <algorithm>  
#include "CameraController.h"  
#include "Player.h"  
#include "MassFunction.h"  

using namespace KamataEngine;  
using namespace std;
void CameraController::Initialize(Camera *camera){  
	camera_ = camera;  
}  

void CameraController::Update(){  
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();  
	
	desetination_ =targetWorldTransform.translation_+targetOffset_+target_->getVelocity()*kVelocityBias;
	camera_->translation_ = Lerp(camera_->translation_, desetination_, 0.1f); // 緩やかに追従するように補間

	camera_->translation_.x=max(camera_->translation_.x,
		desetination_.x+targetMargin_.left);
	camera_->translation_.x=min(camera_->translation_.x,
		desetination_.x+targetMargin_.right);
	camera_->translation_.y=max(camera_->translation_.y,
		desetination_.y+targetMargin_.bottom);
	camera_->translation_.y=min(camera_->translation_.y,
		desetination_.y+targetMargin_.top);


	// 修正: std::max と std::min を使用するために std:: を明示的に指定  
	camera_->translation_.x = max(camera_->translation_.x, moveArea_.left); // x座標を0以上に制限  
	camera_->translation_.x = min(camera_->translation_.x, moveArea_.right); // x座標をmoveAreaの右端以下に制限  
	camera_->translation_.y = min(camera_->translation_.y, moveArea_.bottom); // y座標を0以上に制限  
	camera_->translation_.y = max(camera_->translation_.y, moveArea_.top); // y座標をmoveAreaの下端以下に制限  
	camera_->UpdateMatrix();  
}  

void CameraController::Reset() {  
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();  
	// 必要に応じて targetWorldTransform を使用して処理を追加  
	camera_->translation_ = Add(targetWorldTransform.translation_, targetOffset_);  
	
}
