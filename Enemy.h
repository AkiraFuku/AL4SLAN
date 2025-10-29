#pragma once
#include "KamataEngine.h"
#include "MassFunction.h"
#include <numbers>
#include <algorithm>


using namespace KamataEngine;

class Player;
class MapChipField;
class GameScene;
enum class LRDirection { kRight, kLeft,  };
struct CollisionMapInfo {
		bool isCeiling = false; ///< 天井に衝突しているか
		bool isFloor = false;   ///< 床に衝突しているか
		bool isWall = false;    ///< 壁に衝突しているか
		Vector3 move; ///< 移動パラメータ
	};
enum Corner {
		kRightBottom, ///< 右下
		kLeftBottom,  ///< 左下
		kRightTop,    ///< 右上
		kLeftTop,     ///< 左上
		kNumCorner   ///< コーナーの数
	
};
class Enemy {
public:
	enum class Behavior {
		
		kUnknown, ///< 未知の行動
		kWalk,    ///< 生存行動
		kDead,    ///< 死亡行動

	};
	void Initialize(Model* model,Camera* camera,Vector3& position);
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

	void setGameScene(GameScene*gameScene){ gameScene_=gameScene;}

	// マップチップフィールドの設定
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	void MapCollisionCheck(CollisionMapInfo& collisionMapInfo);
	void CheckMapCollisionUp(CollisionMapInfo &info);
	void CheckMapCollisionDown(CollisionMapInfo &info);
	void CheckMapCollisionRight(CollisionMapInfo &info);
	void CheckMapCollisionLeft(CollisionMapInfo &info);
	Vector3 CornerPosition(const Vector3& center,Corner corner);
	void   ResultCollisionMapInfo(const CollisionMapInfo& info);
	void hitCeiling(const CollisionMapInfo& info);
	void UpdatOnGround(const CollisionMapInfo& info);
	void HitWall(const CollisionMapInfo& info);



private:
	WorldTransform worldTransform_; ///< ワールドトランスフォーム
	Model* model_ = nullptr;    
	Camera* camera_=nullptr;
	static inline const float kWalkSpeed=0.02f ;
	Vector3 velocity_={};
	Vector3 accel_={};
	static inline const float kAcceleration = 0.01f; ///< 移動速度

	static inline const float kWalkMotionAngleStart=0.0f;
	static inline const float kWalkMotionAngleEnd=30.0f;
	static inline const float kWalkMotionTime=1.0f;
	float walkTimer_=0.0f;
	static inline const float kWidth = 0.8f; ///< キャラクターの幅
	static inline const float kHeight = 0.8f; ///< キャラクターの高さ
	static inline const float kBlank = 0.04f; ///< キャラクターの余白
	bool isDead_ = false;  ///< 死亡フラグ
	Behavior behavior_ = Behavior::kWalk;  ///< 行動状態
	Behavior behaviorRequest_ = Behavior::kUnknown; ///< 要求された行動状態
 
	static inline const float kDeadTime = 1.0f; ///< 死亡時間
	static inline const float kDeadMotionAngleStart = 0.0f; ///< 死亡モーション開始角度
	static inline const float kDeadMotionAngleEnd = -60.0f; ///< 死亡時間
	float counter_ = 0.0f;///< カウンター
	
	bool isCollisionDisabled_ = false; ///< 衝突無効フラグ
	GameScene*gameScene_=nullptr;

	MapChipField* mapChipField_ = nullptr; ///< マップチップフィールドへのポインタ

	LRDirection lrDirection_ = LRDirection::kLeft; ///< キャラクターの向き
	        
	bool onGround_ = true;
	bool tachWall_ = false;
	static inline const float kAttenuationLanding = 0.2f; ///< 着地時の減速
	static inline const float kGroundSearchHeight = 0.06f;
	static inline const float kAttenuationWall = 0.2f;

};
 