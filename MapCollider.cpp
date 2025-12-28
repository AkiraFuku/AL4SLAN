#include "MapCollider.h"
#include "MapChipField.h"
#include <algorithm>
#include <array>

using namespace KamataEngine;

void MapCollider::Initialize(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

void MapCollider::CheckCollision(const Vector3& position, float width, float height, CollisionMapInfo& info) {
	// 初期化
	info.isCeiling = false;
	info.isFloor = false;
	info.isWall = false;

	// 各方向の判定
	CheckMapCollisionUp(position, width, height, info);
	CheckMapCollisionDown(position, width, height, info);
	CheckMapCollisionRight(position, width, height, info);
	CheckMapCollisionLeft(position, width, height, info);
}

void MapCollider::CheckMapCollisionUp(const Vector3& position, float width, float height, CollisionMapInfo& info) {
	if (info.move.y <= 0.0f) {
		return;
	}

	const float kHitBoxOffset = 0.2f;

	// 左上（内側）
	Vector3 topLeft = position + info.move + Vector3(-width / 2.0f + kHitBoxOffset, +height / 2.0f, 0.0f);
	// 右上（内側）
	Vector3 topRight = position + info.move + Vector3(+width / 2.0f - kHitBoxOffset, +height / 2.0f, 0.0f);

	MapChipType mapChipType;
	bool hit = false;

	// 左上の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(topLeft);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右上の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(topRight);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 当たっていたら
	if (hit) {
		// 中心座標でインデックス再取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(position + Vector3(0.0f, +height / 2.0f, 0.0f));
		MapChipField::IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(position);
		if (indexSet.yIndex != indexSetNow.yIndex) {
			// 天井音を鳴らすなどの処理をここに追加可能
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// 上方向の移動量を補正
			info.move.y = max(0.0f, rect.bottom - position.y - (height / 2.0f + kBlank));
			info.isCeiling = true;
		}
	}
}

void MapCollider::CheckMapCollisionDown(const Vector3& position, float width, float height, CollisionMapInfo& info) {
	if (info.move.y >= 0.0f) {
		return;
	}

	const float kHitBoxOffset = 0.2f;

	// 左下（内側）
	Vector3 bottomLeft = position + info.move + Vector3(-width / 2.0f + kHitBoxOffset, -height / 2.0f, 0.0f);
	// 右下（内側）
	Vector3 bottomRight = position + info.move + Vector3(+width / 2.0f - kHitBoxOffset, -height / 2.0f, 0.0f);

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	bool hit = false;

	// 左下の当たり判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(bottomLeft);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右下の当たり判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(bottomRight);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	// 下がブロックで、その上がブロックでない場合（崖の端など）
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(position + info.move + Vector3(0.0f, -height / 2.0f, 0.0f));
		MapChipField::IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(position);
		if (indexSet.yIndex != indexSetNow.yIndex) {
			// 落下音を鳴らすなどの処理をここに追加可能
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = min(0.0f, rect.top - position.y + (height / 2.0f + kBlank));
			info.isFloor = true;
		}
	}
}

void MapCollider::CheckMapCollisionRight(const Vector3& position, float width, float height, CollisionMapInfo& info) {
	if (info.move.x <= 0.0f) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(position + info.move, width, height, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	bool hit = false;

	// 右上
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(position + info.move + Vector3(+width / 2.0f, 0.0f, 0.0f));
		MapChipField::IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(position);
		if (indexSet.xIndex != indexSetNow.xIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.x = max(0.0f, rect.left - position.x - (width / 2.0f + kBlank));
			info.isWall = true;
		}
	}
}

void MapCollider::CheckMapCollisionLeft(const Vector3& position, float width, float height, CollisionMapInfo& info) {
	if (info.move.x >= 0.0f) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(position + info.move, width, height, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;

	// 左上
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 左下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(position + info.move + Vector3(-width / 2.0f, 0.0f, 0.0f));
		MapChipField::IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(position);
		if (indexSet.xIndex != indexSetNow.xIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.x = min(0.0f, rect.right - position.x + (width / 2.0f + kBlank));
			info.isWall = true;
		}
	}
}

Vector3 MapCollider::CornerPosition(const Vector3& center, float width, float height, Corner corner) {
	Vector3 OffsetTable[kNumCorner] = {
	    Vector3{+width / 2.0f, -height / 2.0f, 0.0f}, // kRightBottom
	    Vector3{-width / 2.0f, -height / 2.0f, 0.0f}, // kLeftBottom
	    Vector3{+width / 2.0f, +height / 2.0f, 0.0f}, // kRightTop
	    Vector3{-width / 2.0f, +height / 2.0f, 0.0f}  // kLeftTop
	};
	return center + OffsetTable[static_cast<uint32_t>(corner)];
}