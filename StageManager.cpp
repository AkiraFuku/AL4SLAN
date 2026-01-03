#include "StageManager.h"
#include <fstream>
#include <sstream>

void StageManager::RoadStage() {
	const std::string filePath = "Resources/Stage/stageDatas.csv";
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());
	std::stringstream ss;
	ss << file.rdbuf();
	file.close();
	std::string line;

	// 2. ssから改行文字に出会うまで読み込み、lineに格納するループ
	while (std::getline(ss, line)) {
		std::stringstream line_stream(line);
		StageData stageData;
		std::string segment;
		// --- 1つ目の要素（ファイルパス/名前）を取得 ---
		// 第3引数に ',' を指定すると、カンマまで読み込みます
		std::getline(line_stream, segment, ',');
		stageData.name = segment;

		// --- 2つ目の要素（制限時間）を取得 ---
		std::getline(line_stream, segment, ',');
		// 数値として使う場合は std::stoi で string を int に変換
		stageData.timeLimit = std::stoi(segment);

		stageDatas_.push_back(stageData);
	}
}
