#include "StageManager.h"
#include <fstream>
#include <cassert>
#include <sstream>

void StageManager::RoadStage() {
	const std::string filePath="Resources/Stage/stageDatas.csv";
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open() );
	std::stringstream ss;
	ss << file.rdbuf(); 
	file.close();


}
