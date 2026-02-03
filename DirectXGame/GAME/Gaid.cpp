#include "Gaid.h"

void Gaid::Initialize() {
	gaidTextureHandle_ = TextureManager::Load("gaid/Gaid.png");
	GaidSprite_ = Sprite::Create(gaidTextureHandle_, Vector2{});
	GaidSprite_->SetSize(Vector2(1280,720));


	GaidSprite_->SetAnchorPoint(Vector2(0.0f, 0.0f));
}
void Gaid::Update() {}
void Gaid::Draw() {
	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	GaidSprite_->Draw();
	Sprite::PostDraw();
}