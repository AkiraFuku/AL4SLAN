#pragma once
#include <KamataEngine.h>
using namespace KamataEngine;

class Gaid {

	public:
void Initialize();
void Update();
void Draw();

private:
	uint32_t gaidTextureHandle_ = 0;
	Sprite* GaidSprite_ = nullptr;
};
