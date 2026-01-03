#pragma once
#include <string>
#include <vector>
struct StageData {
	std::string name;
	int32_t timeLimit;
};

class StageManager {
public:

	void RoadStage();

private:
	std::vector<StageData> stageDatas_;
};
