#include "Goal.h"
#include "Player.h"
#include "assert.h"
void Goal::Initialize(Model* model, Camera* camera, Vector3& position) {
	assert(model);
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}
void Goal::Update() {
#ifdef DEBUG
if (Input::GetInstance()->PushKey(DIK_G)) {
		isGoal_ = true; // デバッグ用にゴールフラグを立てる
	}
#endif // DEBUG

	if (isAnimating_) {
       worldTransform_.translation_.y = Lerp(worldTransform_.translation_.y, targetY_, 0.1f);
    }

	WorldTransformUpdate(&worldTransform_);
}
void Goal::Draw() {
	model_->Draw(worldTransform_, *camera_); }

AABB Goal::GetAABB() {

	Vector3 worldPos = worldTransform_.translation_;
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}
void Goal::OnCollision(const Player* player) {
	
	(void)player;   // 引数のPlayerは使用しないのでキャストして警告を抑制
	isGoal_ = true; // ゴールフラグを立てる
	
	

}

void Goal::StartClearAnimation(float playerY) {

	
    if (!isAnimating_) {
        isAnimating_ = true;
        // プレイヤーの頭上を指定距離分プラスした場所を目標にする
        targetY_ = playerY + kHeightAbovePlayer;
    }
}