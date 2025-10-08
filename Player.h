#pragma once
#define NOMINMAX
#include "KamataEngine.h"
#include "MassFunction.h"

using namespace KamataEngine;
class MapChipField;
class Enemy;


/// <summary>
/// 自キャラ
/// </summary>
class Player {
public:

	enum class LRDirection { kRight, kLeft,  };
	enum class Behavior {
		kUnknown, ///< 未定義
		kRoot, ///< 通常行動
		kAttack, ///< 攻撃行動
		kDash,    ///< ダッシュ行動
	};

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
	enum class AttackPhase {
	kUnknown, ///< 未定義
	kCharge, ///< 溜め
	kAttack, ///< 攻撃
	kAfter,  ///< 攻撃後

	};
	enum class DashPhase {
	kUnknown, ///< 未定義
	kCharge, ///< 溜め
	kDash, ///< ダッシュ
	kAfter,  ///< 攻撃後

	};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void Initialize(Model* model,Model* modelAttack,uint32_t textureHandle,Camera* camera,const Vector3& position );
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	
    /// <summary>
    /// ワールドトランスフォームの参照を取得
    /// </summary>
    /// <returns>ワールドトランスフォームの参照</returns>
    const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const Vector3& getVelocity() const { return velocity_; }
	
	void SetMapchipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	void MapCollisionCheck(CollisionMapInfo& collisionMapInfo);
	void CheckMapCollisionUp(CollisionMapInfo &info);
	void CheckMapCollisionDown(CollisionMapInfo &info);
	void CheckMapCollisionRight(CollisionMapInfo &info);
	void CheckMapCollisionLeft(CollisionMapInfo &info);
	void inputMove();
	Vector3 CornerPosition(const Vector3& center,Corner corner);
	void   ResultCollisionMapInfo(const CollisionMapInfo& info);
	void hitCeiling(const CollisionMapInfo& info);
	void UpdatOnGround(const CollisionMapInfo& info);
	void HitWall(const CollisionMapInfo& info);

	Vector3 GetWorldPosition();

	AABB GetAABB();
	void OnCollision(const Enemy*enemy);
	// 死亡判定
	bool IsDead() const { return isDead_; }

	//通常行動の更新
	void BehaviorRootUpdate();
	// 　攻撃行動の更新
	void BehaviorAttackUpdate();
	// ダッシュ行動の更新
	void BehaviorDashUpdate();

	//
	void BehaviorRootInitialize();
	void BehaviorAttackInitialize();
	void BehaviorDashInitialize();

	// 行動状態の設定
	bool isAttack()const;

private:
	WorldTransform worldTransform_; ///< ワールドトランスフォーム
	Model* model_ = nullptr;        ///< モデル
	uint32_t textureHandle_ = 0u;    ///< テクスチャハンドル
	Camera* camera_=nullptr;      ///< カメラ
	Vector3 velocity_ = {}; ///< 速度
	static inline const float kAcceleration = 0.01f; ///< 移動速度
	static inline const float kAttenution = 0.05f;   ///< 減速
	static inline const float kLimitRunSpeed = 0.3f; ///< 最大速度
	LRDirection lrDirection_ = LRDirection::kRight; ///< キャラクターの向き
	// 旋回開始時角度
	float turnFirstRotationY_ = 0.0f;
	//旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間<秒>
	static inline const float kTimeTurn =0.3f;
	bool onGround_=true;
	//重力加速度
	static inline const float kGravityAcceleration=0.98f;
	//落下速度制限
	static inline const float  kLimitFallSpeed = 0.5f;
	//跳躍加速度
	static inline const float kJumpAcceleration=20.0f;
	MapChipField* mapChipField_ = nullptr; ///< マップチップフィールドへのポインタ
	static inline const float kWidth = 0.8f; ///< キャラクターの幅
	static inline const float kHeight = 0.8f; ///< キャラクターの高さ
	static inline const float kBlank = 0.04f; ///< キャラクターの余白
	static inline const float kAttenuationLanding = 0.2f; ///< 着地時の減速
	static inline const float kGroundSearchHeight = 0.06f;
	static inline const float kAttenuationWall = 0.2f;
	//死亡フラグ
	 bool isDead_ = false;

	 // 行動状態
	 Behavior behavior_ = Behavior::kRoot; ///< 行動状態
	 Behavior behaviorRequest_ = Behavior::kUnknown; ///< 次の行動状態
	 // 攻撃の経過時間カウンター
	 uint32_t attackParameter_ = 0; ///< 攻撃の経過時間カウンター
	 AttackPhase attackPhase_=AttackPhase::kUnknown; ///< 攻撃フェーズ
	static inline const uint32_t  kChageTime= 8;
	static inline const uint32_t  kAttackTime= 5;
	static inline const uint32_t  kAfterTime= 12;
	// ダッシュの経過時間カウンター
	uint32_t dashParameter_ = 0; ///< ダッシュの経過時間カウンター
	DashPhase dashPhase_ = DashPhase::kUnknown; ///< ダッシュフェーズ
	static inline const uint32_t kDashChageTime = 8;
	static inline const uint32_t kDashAttackTime = 5;
	static inline const uint32_t kDashAfterTime = 12;
	
	Model* modelAttack_ = nullptr; ///< 攻撃用モデル
	WorldTransform worldTransformAttack_; ///< 攻撃用ワールドトランスフォーム

	WorldTransform worldTransformMove_; ///< 1フレーム前のワールドトランスフォーム

	//コントローラー
	XINPUT_STATE state_;

	XINPUT_STATE prevState_;

	uint32_t jumpSEHandle_;

	uint32_t attackSEHandle_;

	uint32_t DeathSEHandle_;

	// ジャンプ回数
	uint32_t jumpCount_ = 0;
	// ジャンプ回数制限
	static inline const uint32_t kLimitJumpCount = 2;
	
};
