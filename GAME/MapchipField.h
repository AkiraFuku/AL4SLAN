#pragma once
#include "KamataEngine.h"
#include "MassFunction.h"
#include <vector>
#include <math.h>
#include <StageManager.h>

enum class MapChipType {
	kBlank,
	kBlock,
	kEnemy,
	kPlayer,
	kGoal,

};
struct MapChipDataUnit {

	MapChipType type;
	uint8_t subID;
};
struct MapChipData {
	std::vector<std::vector<MapChipDataUnit>> data; ///< マップチップデータ
	                       
};

class MapChipField {

	private:
	static inline const float kBlockWidth = 1.0f; ///< ブロックの横幅
	    static inline const float kBlockHeight =1.0f; ///< ブロックの縦幅
	          // ブロックの行数
	static inline const uint32_t kNumBlockVertical = 13;
	    // ブロックの列数
	static inline const uint32_t kNumBlockHorizontal = 100;
	MapChipData mapChipData_; ///< マップチップデータ
public:
	struct IndexSet {
		uint32_t xIndex; ///< Xインデックス
		uint32_t yIndex; ///< Yインデックス
	};
	struct Rect {
		float left; ///< 左端
		float right; ///< 右端
		float top;   ///< 上端
		float bottom; ///< 下端
	};
	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex,uint32_t yIndex) ;
	uint8_t GetMapChipSubIDByIndex(uint32_t xIndex,uint32_t yIndex);
	Vector3 GetBlockPositionByIndex(uint32_t xIndex, uint32_t yIndex) ;
	// ブロックの行数
	uint32_t GetNumBlockVertical() { return kNumBlockVertical; }
	// ブロックの列数
	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }
	// マップチップのデータを取得 
	Vector3 GetmapChipPositionIndex(uint32_t xIndex, uint32_t yIndex) { return Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVertical - 1 - yIndex), 0); }
	// マップチップのデータを取得
	IndexSet GetMapChipIndexSetByPosition(const Vector3& Position);
	// マップチップのデータを取得
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	enum MapChipCherIndex {
		kChipType=0,
		kChipSubID=1,

	};
};

