#include "StageManager.h"
#include <fstream>
#include <sstream>

// シングルトンの実装
StageManager* StageManager::GetInstance() {
	static StageManager instance;
	return &instance;
}

void StageManager::RoadStage() {
	// 既に読み込み済みなら再読み込みしない（必要に応じてクリア処理を入れても良い）
	if (!stageDatas_.empty()) {
		return;
	}

	const std::string filePath = "Resources/Stage/stageDatas.csv";
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());
	
	std::stringstream ss;
	ss << file.rdbuf();
	file.close();

	std::string line;
	while (std::getline(ss, line)) {
		std::stringstream line_stream(line);
		StageData stageData;
		std::string segment;

		// 1. ファイル名
		std::getline(line_stream, segment, ',');
		stageData.name = segment;

		// 2. 制限時間
		std::getline(line_stream, segment, ',');
		stageData.timeLimit = std::stoi(segment);

		stageDatas_.push_back(stageData);
	}
}