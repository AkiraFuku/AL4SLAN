#pragma once
#include "KamataEngine.h"

#include "MassFunction.h"
#include <algorithm>
#include <numbers>
#include "MapCollider.h"

using namespace KamataEngine;

class Player;
class MapChipField;
class GameScene;
enum class LRDirection {
	kRight,
	kLeft,
};

class Enemy {
public:
	enum class Behavior {

		kUnknown, ///< 未知の行動
		kWalk,    ///< 生存行動
		kDead,    ///< 死亡行動

	};
	
	

	//// 角の定義（Playerと同じ）
	//enum Corner {
	//	kRightBottom, ///< 右下
	//	kLeftBottom,  ///< 左下
	//	kRightTop,    ///< 右上
	//	kLeftTop,     ///< 左上
	//	kNumCorner    ///< コーナーの数
	//};
	void Initialize(Model* model, Camera* camera, Vector3& position);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	Vector3 GetWorldPosition();
	AABB GetAABB();
	void OnCollision(const Player* player);
	// 　被弾判定
	void HitAttack(const Player* player);

	bool IsDead() const { return isDead_; } ///< 死亡フラグの取得

	bool IsCollisionDisabled() const { return isCollisionDisabled_; } ///< 衝突無効フラグの取得

	void setGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	// カメラに映っているか
	bool InCamera();

	void SetMapChipField(MapChipField* mapChipField);

	void OnCollisionWithEnemy();

    const Vector3& GetVelocity() const { return velocity_; }

    // targetTopY: 下にいるエネミーの頭頂部のY座標
    void OnLandOnEnemy(float targetTopY);

	//めり込まない
	void AddPosition(const Vector3& offset);
private:
	// ★追加: 衝突判定関連のメンバ関数
   /* void MapCollisionCheck(CollisionMapInfo& info);
    void CheckMapCollisionUp(CollisionMapInfo& info);
    void CheckMapCollisionDown(CollisionMapInfo& info);
    void CheckMapCollisionRight(CollisionMapInfo& info);
    void CheckMapCollisionLeft(CollisionMapInfo& info);
  */  void ResultCollisionMapInfo(const CollisionMapInfo& info);
  //  Vector3 CornerPosition(const Vector3& center, Corner corner);
private:
	WorldTransform worldTransform_; ///< ワールドトランスフォーム
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;

	std::unique_ptr<MapCollider> mapCollider_;
	MapChipField* mapChipField_ = nullptr;
	static inline const float kWalkSpeed = 0.02f;
	Vector3 velocity_ = {};
	static inline const float kWalkMotionAngleStart = 0.0f;
	static inline const float kWalkMotionAngleEnd = 30.0f;
	static inline const float kWalkMotionTime = 1.0f;
	float walkTimer_ = 0.0f;
	static inline const float kWidth = 0.8f;        ///< キャラクターの幅
	static inline const float kHeight = 0.8f;       ///< キャラクターの高さ
	static inline const float kBlank = 0.04f;       ///< キャラクターの余白
	bool isDead_ = false;                           ///< 死亡フラグ
	Behavior behavior_ = Behavior::kWalk;           ///< 行動状態
	Behavior behaviorRequest_ = Behavior::kUnknown; ///< 要求された行動状態

	static inline const float kDeadTime = 1.0f;             ///< 死亡時間
	static inline const float kDeadMotionAngleStart = 0.0f; ///< 死亡モーション開始角度
	static inline const float kDeadMotionAngleEnd = -60.0f; ///< 死亡時間
	float counter_ = 0.0f;                                  ///< カウンター

	bool isCollisionDisabled_ = false; ///< 衝突無効フラグ
	GameScene* gameScene_ = nullptr;


	LRDirection lrDirection_ = LRDirection::kLeft; ///< キャラクターの向き
	static inline const float kViewRangeX = 20.0f;
	static inline const float kViewRangeY = 15.0f;
	static inline const float kGravity = -0.1f;
	static inline const float kLimitFallSpeed = -2.0f;

	float collisionCooldown_ = 0.0f;
};
