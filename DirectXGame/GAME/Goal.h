#pragma once
#include "KamataEngine.h"
#include "MassFunction.h"

using namespace KamataEngine;

class Player;
class GameScene;

class Goal {

public:
	void Initialize(Model* model,Camera* camera,Vector3& position);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	AABB GetAABB();
	void OnCollision(const Player* player);
	void setGameScene(GameScene*gameScene){ gameScene_=gameScene;}

	const WorldTransform& GetWorldTransform() const { return worldTransform_; } ///< ワールドトランスフォームの取得
	void SetTranslation(const Vector3& translation) { worldTransform_.translation_ = translation; } ///< 座標の設定
	void SetRotation(const Vector3& rotation) { worldTransform_.rotation_ = rotation; }             ///< 回転角の設定
	
	void StartClearAnimation(float playerY);

	bool isGoal() const { return isGoal_; } ///< ゴールフラグの取得


private:



	Model* model_ = nullptr; ///< モデル
	Camera* camera_ = nullptr; ///< カメラ
	WorldTransform worldTransform_; ///< ワールドトランスフォーム
	ObjectColor objectColor_; ///< オブジェクトカラー
	Vector4 color_;           ///< 色
	bool isGoal_ = false;     ///< ゴールフラグ

	static inline const float kWidth = 0.8f; ///< ゴール幅
	static inline const float kHeight = 0.8f; ///< ゴール高さ
	AABB aabb_; ///< AABB（Axis-Aligned Bounding Box）

	GameScene* gameScene_ = nullptr; ///< ゲームシーンへの参照
	bool isAnimating_ = false;
  // bool isAnimating_ = false;
    float targetY_ = 0.0f; // 停止する目標の高さ
    static inline const float kRiseSpeed = 0.15f; // 上昇速度
    static inline const float kHeightAbovePlayer = 1.5f; // プレイヤーからどれくらい上に浮かせるか
};
