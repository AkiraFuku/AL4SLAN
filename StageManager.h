#pragma once
#include <cassert>
#include <string>
#include <vector>

struct StageData {
	std::string name; // ファイル名 (例: field1.csv)
	int32_t timeLimit;

};

class StageManager {
public:
	// 1. シングルトンインスタンス取得
	static StageManager* GetInstance();

	// データを読み込む関数 (LoadStage のタイポ修正も含め RoadStage のままにします)
	void RoadStage();

	// 指定したインデックスのステージデータを取得
	const StageData& GetStageData(int32_t index) const {
		// 配列外参照を防ぐアサート
		assert(index >= 0 && index < static_cast<int32_t>(stageDatas_.size()));
		return stageDatas_[index];
	}

	// ステージ総数を取得
	uint32_t GetStageNum() const {
		return static_cast<uint32_t>(stageDatas_.size());
	}

private:
	// シングルトン用コンストラクタ隠蔽
	StageManager() = default;
	~StageManager() = default;
	StageManager(const StageManager&) = delete;
	StageManager& operator=(const StageManager&) = delete;

	std::vector<StageData> stageDatas_;
};