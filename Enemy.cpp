
#include <cassert>
#include <numbers>
#include "Enemy.h"
#include "Math.h"
#include "Player.h"
#include "HitEffect.h"
#include "GameScene.h"

void Enemy::Initialize(Model* model, Camera* camera,Vector3& position ) {
	// NULLチェック
	assert(model);

	
	model_  = model;
	
	camera_ = camera;
	
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	// 角度調整
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	velocity_={-kWalkSpeed,0.0f,0.0f};
	walkTimer_=0.0f;



}
void Enemy::Update() {

		if (behaviorRequest_ != Behavior::kUnknown) {
		// 振るまいを変更する
		behavior_ = behaviorRequest_;

		// 各振るまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kDead:
		default:
			counter_ = 0.0f;
		break;
		}

		// 振るまいリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	
	case Enemy::Behavior::kWalk:
	default:{
		worldTransform_.translation_+=velocity_;
	walkTimer_+=1.0f/60.0f;

	float param= std::sinf(std::numbers::pi_v<float>*2.0f*walkTimer_/kWalkMotionTime);
	float degree =kWalkMotionAngleStart+kWalkMotionAngleEnd*(param+1.0f)/2.0f;
	worldTransform_.rotation_.x=Radian(degree);
	WorldTransformUpdate(&worldTransform_);
		break;
	}
	case Enemy::Behavior::kDead:{
		// 死亡時の処理
		// 死亡アニメーションの時間を経過させる
		counter_ += 1.0f / 60.0f; // 1フレーム分の時間を引く
		worldTransform_.rotation_.y+=0.3f ; // Y座標を下げる
		worldTransform_.rotation_.x=EaseOut(ToRadian(kDeadMotionAngleStart),ToRadian(kDeadMotionAngleEnd),counter_/kDeadTime);

		WorldTransformUpdate(&worldTransform_);
		if (counter_>=kDeadTime) {
			isDead_ = true; // 死亡アニメーションが終わったら、敵を削除するフラグを立てる

		}

		break;
	}
	}

	


	
};
void Enemy::Draw(){

model_->Draw(worldTransform_,*camera_);
};
Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x =worldTransform_.translation_.x;
	worldPos.y =worldTransform_.translation_.y;
	worldPos.z =worldTransform_.translation_.z;
	
	
	return worldPos;

}

AABB Enemy::GetAABB() { 
	Vector3 worldPos =GetWorldPosition();

AABB aabb;

aabb.min ={worldPos.x-kWidth/2.0f,worldPos.y-kHeight/2.0f,worldPos.z-kWidth/2.0f};
aabb.max ={worldPos.x+kWidth/2.0f,worldPos.y+kHeight/2.0f,worldPos.z+kWidth/2.0f};
return aabb;
}

void Enemy::OnCollision(const Player* player) {
//(void)player;
	// プレイヤーとの衝突時の処理をここに記述
	// 例えば、敵を消す、ダメージを与えるなど
// 今回は何もしない
// ただし、死亡フラグを立てるなどの処理は行う

	///isDead_ = true; // プレイヤーと衝突したら死亡
if (behavior_==Behavior::kDead) {
	return;
}
	if (player->isAttack()) {
		// 敵の振るまいをやられに変更
		behaviorRequest_ = Behavior::kDead;

		Vector3 effectPos=Division(Add(worldTransform_.translation_,player->GetWorldTransform().translation_),2.0f);
		gameScene_->CreateHitEffect(effectPos);
		

		isCollisionDisabled_ = true; // 衝突を無効化

		
	}
}


