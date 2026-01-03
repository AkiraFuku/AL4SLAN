#include "MapchipField.h"
#include <map>
#include <fstream>
#include <sstream>
#include <cassert>
#include <string>
namespace{
    std::map<char, MapChipType> mapChipTable= {
     {'0', MapChipType::kBlank},
     {'B', MapChipType::kBlock},
	 {'E', MapChipType::kEnemy},
	 {'P', MapChipType::kPlayer},
     {'G', MapChipType::kGoal  }
    };
}

void MapChipField::ResetMapChipData() {
    mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVertical);
	for (std::vector<MapChipDataUnit>& mapChipDataLine:mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {
    ResetMapChipData();
    //
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open() );
	///
	std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();
	file.close();
	// CSVの読み込み
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		std::string line;
		getline(mapChipCsv, line);
		std::istringstream line_Stream(line);
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			std::string word;
			getline(line_Stream, word, ',');

			if (word.empty()) {
				continue;
			}
			if (!mapChipTable.contains(word[kChipType])) {
				continue;
			}
			mapChipData_.data[i][j].type=mapChipTable[word[kChipType]];
		
			if (word.size()<=kChipSubID) {
				continue;
			}
			mapChipData_.data[i][j].subID = static_cast<uint8_t>(word[kChipSubID] - '0');
		}
	}
}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) { 
	// 範囲外チェック
	if (xIndex<0||kNumBlockHorizontal-1<xIndex) {
		return MapChipType::kBlank;
	}
	// 範囲外チェック
	if (yIndex < 0 || kNumBlockVertical - 1 < yIndex) {
		return MapChipType::kBlank;
	}
	// マップチップのデータを取得
	return mapChipData_.data[yIndex][xIndex].type;
}

uint8_t MapChipField::GetMapChipSubIDByIndex(uint32_t xIndex, uint32_t yIndex) { 
	// 範囲外チェック
	if (xIndex<0||kNumBlockHorizontal-1<xIndex) {
		return 0;
	}
	// 範囲外チェック
	if (yIndex < 0 || kNumBlockVertical - 1 < yIndex) {
		return 0;
	}
	// マップチップのデータを取得
	return mapChipData_.data[yIndex][xIndex].subID;
}

Vector3 MapChipField::GetBlockPositionByIndex(uint32_t xIndex, uint32_t yIndex) { 
	return Vector3(kBlockWidth*xIndex,kBlockHeight*(kNumBlockVertical-1-yIndex),0); 
}


MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const Vector3& Position) { 
	IndexSet indexSet;
	indexSet.xIndex = static_cast<uint32_t>((Position.x+kBlockWidth /2.0f)/ kBlockWidth);
	indexSet.yIndex = kNumBlockVertical - 1 - static_cast<uint32_t>(Position.y + kBlockHeight / 2.0f/kBlockHeight);
	return indexSet; 

}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) { 
	
	Vector3 center = GetBlockPositionByIndex(xIndex, yIndex);	
	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;
	return rect;

}
