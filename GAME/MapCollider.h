#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;

// 前方宣言
class MapChipField;

/// <summary>
/// マップチップとの当たり判定判定結果
/// </summary>
struct CollisionMapInfo {
	bool isCeiling = false; ///< 天井に衝突しているか
	bool isFloor = false;   ///< 床に衝突しているか
	bool isWall = false;    ///< 壁に衝突しているか
	Vector3 move;           ///< 移動量（補正後）
};

/// <summary>
/// マップ衝突判定クラス
/// </summary>
class MapCollider {
public:
	// 四隅の定義
	enum Corner {
		kRightBottom, ///< 右下
		kLeftBottom,  ///< 左下
		kRightTop,    ///< 右上
		kLeftTop,     ///< 左上
		kNumCorner    ///< コーナーの数
	};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="mapChipField">マップチップフィールドのポインタ</param>
	void Initialize(MapChipField* mapChipField);

	/// <summary>
	/// 衝突判定と補正を行うメイン関数
	/// </summary>
	/// <param name="position">現在のワールド座標</param>
	/// <param name="width">判定幅</param>
	/// <param name="height">判定高さ</param>
	/// <param name="info">移動情報（moveを更新して返す）</param>
	void CheckCollision(const Vector3& position, float width, float height, CollisionMapInfo& info);

private:
	// 内部判定関数
	void CheckMapCollisionUp(const Vector3& position, float width, float height, CollisionMapInfo& info);
	void CheckMapCollisionDown(const Vector3& position, float width, float height, CollisionMapInfo& info);
	void CheckMapCollisionRight(const Vector3& position, float width, float height, CollisionMapInfo& info);
	void CheckMapCollisionLeft(const Vector3& position, float width, float height, CollisionMapInfo& info);

	// 四隅の座標計算ヘルパー
	Vector3 CornerPosition(const Vector3& center, float width, float height, Corner corner);

private:
	MapChipField* mapChipField_ = nullptr;
	static inline const float kBlank = 0.04f; // 余白
};