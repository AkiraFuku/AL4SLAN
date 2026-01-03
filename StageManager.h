#pragma once
#include <cassert>
#include <string>
#include <vector>

struct StageData {
	std::string name;
	int32_t timeLimit;
};

class StageManager {
public:
	// 1. シングルトンインスタンス取得用

	void RoadStage();
	const StageData& GetStageData(int32_t index) const {
		assert(index < static_cast<int32_t>(stageDatas_.size()));
		return stageDatas_[index];
	}
	uint32_t GetStageNum(){
	return stageDatas_.size();
	}

private:
	std::vector<StageData> stageDatas_;
};
