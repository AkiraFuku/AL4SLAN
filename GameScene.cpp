#include "GameScene.h"

using namespace KamataEngine;

// GameScene::GameScene() {}
//



GameScene::~GameScene() {
	delete player_;
	delete model_;
	delete AttackModel_;
	///
	delete blockM_;
	//
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* WorldTransformBlock : worldTransformBlockLine) {
			delete WorldTransformBlock;
		}
	}

	worldTransformBlocks_.clear();
	// デバッグカメラの解放
	delete debugCamera_;
	// スカイドームの解放
	delete skydome_;
	// スカイドームのモデルの解放
	delete modelSkydome_;
	// マップチップフィールドの解放
	delete mapchipField_;
	// delete enemy_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	

	delete deathParticles_;
	delete deathParticlesModel_;

	delete fade_;

	delete hitEffectModel_;
	for (HitEffect* hitEffect:hitEffects_){
	delete hitEffect;
	}
	if (goal_) {
		delete goal_;
	}
	
	delete goalModel_;
	delete gaid_;

	
	
	
}
// ゲームシーンのブロック生成
void GameScene::GenerateBlock() {
	/// ブロック要素数
	uint32_t numBlockVertical = mapchipField_->GetNumBlockVertical();
	uint32_t numBlockHorizontal = mapchipField_->GetNumBlockHorizontal();

	// ブロック1個横幅

	worldTransformBlocks_.resize(numBlockVertical);
	for (uint32_t i = 0; i < numBlockVertical; i++) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}
	// キューブの生成
	for (uint32_t i = 0; i < numBlockVertical; i++) {
		for (uint32_t j = 0; j < numBlockHorizontal; j++) {
			if (mapchipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapchipField_->GetBlockPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::GenerateEnemy() {
	uint32_t numBlockVertical = mapchipField_->GetNumBlockVertical();
	uint32_t numBlockHorizontal = mapchipField_->GetNumBlockHorizontal();

	
	for (uint32_t i = 0; i < numBlockVertical; i++) {
		for (uint32_t j = 0; j < numBlockHorizontal; j++) {
			if (mapchipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kEnemy) {
				Enemy* newEnemy = new Enemy();
				Vector3 enemyPosition = mapchipField_->GetmapChipPositionIndex(j, i);
				newEnemy->Initialize(enemy_model_, &camera_, enemyPosition);
				newEnemy->setGameScene(this);
				newEnemy->SetMapChipField(mapchipField_);
				enemies_.push_back(newEnemy);
			}
		}
	}


}

void GameScene::GenerateGoal() {
	uint32_t numBlockVertical = mapchipField_->GetNumBlockVertical();
	uint32_t numBlockHorizontal = mapchipField_->GetNumBlockHorizontal();

	for (uint32_t i = 0; i < numBlockVertical; i++) {
		for (uint32_t j = 0; j < numBlockHorizontal; j++) {
			if (mapchipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kGoal) {
				goal_ = new Goal();
				Vector3 Position = mapchipField_->GetmapChipPositionIndex(j, i);
				goal_->Initialize(goalModel_, &camera_,Position);
			}
		}
	}

	


}

Vector3 GameScene::PlayerStartPosition() {
	
	uint32_t numBlockVertical = mapchipField_->GetNumBlockVertical();
	uint32_t numBlockHorizontal = mapchipField_->GetNumBlockHorizontal();
	Vector3 Position ={};
	for (uint32_t i = 0; i < numBlockVertical; i++) {
		for (uint32_t j = 0; j < numBlockHorizontal; j++) {
			if (mapchipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kPlayer) {
				
				 Position = mapchipField_->GetmapChipPositionIndex(j, i);

			
			}
		}
	}
	return Position;

}

void GameScene::CheckAllCollisions() {

#pragma region
	// 座標１,２
	AABB aabb1, aabb2;
	AABB attackAABB = player_->GetAttackAABB();
	// 自キャラ
	aabb1 = player_->GetAABB();
	// 敵キャラ
	for (Enemy* enemy : enemies_) {
		if (enemy->IsCollisionDisabled()) {
			continue; // 衝突判定を無効にしている敵はスキップ
		}
		aabb2 = enemy->GetAABB();
		if (IsCollision(aabb1, aabb2)) {

			
			player_->OnCollision(enemy);
			enemy->OnCollision(player_);

		}
		// 攻撃判定
		if (IsCollision(attackAABB, aabb2)) {
			enemy->HitAttack(player_);
		}

		

	}
	//ゴール
	if (goal_) {
	aabb2 = goal_->GetAABB();
		if (IsCollision(aabb1, aabb2)) {
		goal_->OnCollision(player_);
			clear_ = true;
		}
	}
	
	

#pragma endregion
}


//  ゲームシーンの初期化
void GameScene::Initialize() {
	teXtureHandle_ = TextureManager::Load("img_thumb_08_01.png");
	// モデルの生成
	model_ = Model::CreateFromOBJ("player", true);

	camera_.Initialize();

	mapchipField_ = new MapChipField();
	mapchipField_->LoadMapChipCsv("Resources/Stage/blocks.csv");

	// 自キャラ生成
	player_ = new Player();
	AttackModel_ = Model::CreateFromOBJ("block", true);

	// 自キャラの初期化
	Vector3 playerPosition = PlayerStartPosition();
	player_->Initialize(model_,AttackModel_, teXtureHandle_, &camera_, playerPosition);
	player_->SetMapchipField(mapchipField_);
	// 修正: player_->SetMapchipField(mapchipField_); に変更
	//	//ブロックモデル生成
	blockM_ = Model::CreateFromOBJ("block", true);

	///
	GenerateBlock();

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);
	// スカイドームのモデル生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);
	/// カメラコントローラーの生成
	cameraControlle_ = new CameraController();
	cameraControlle_->Initialize(&camera_);
	cameraControlle_->SetTarget(player_);
	cameraControlle_->Reset();
	CameraController::Rect cameraArea(12.0f, 100.0f - 12.0f, 6.0f, 6.0f);
	cameraControlle_->SetMoveArea(cameraArea);

	gaid_ = new Gaid();
	gaid_->Initialize();
	// エネミー

	enemy_model_ = Model::CreateFromOBJ("enemy");


	GenerateEnemy();

	for (Enemy* enemy : enemies_) {
		enemy->setGameScene(this);
		enemy->SetMapChipField(mapchipField_);
	}

	
	// デスパーティクル
	deathParticlesModel_ = Model::CreateFromOBJ("deathParticle");
	// フェーズ
	phase_ = Phase::kFadeIn;
	//フェード
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// 　ヒットエフェクトの初期化
	hitEffectModel_ = Model::CreateFromOBJ("particle", true);
	HitEffect::SetModel(hitEffectModel_);
	HitEffect::SetCamera(&camera_);

	// ゴールの初期化
	goalModel_ = Model::CreateFromOBJ("goal", true);

	crearModel_ = Model::CreateFromOBJ("clear", true);


	worldTransformClear_.Initialize();
	
	worldTransformClear_.scale_ = {0.5f, 0.5f, 0.5f};

	RetryModel_ = Model::CreateFromOBJ("Retry", true);
	worldTransformRetry_.Initialize();
	worldTransformRetry_.scale_ = {0.5f, 0.5f, 0.5f};
	GenerateGoal();


	
	// BGM再生
	
	

	
	

}
void GameScene::ChangePhase() {
	switch (phase_) {

		case Phase::kStart:	
				if (Input::GetInstance()->TriggerKey(DIK_SPACE)||((state_.Gamepad.wButtons & XINPUT_GAMEPAD_A))&&!(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
			phase_ = Phase::kPlay; // ポーズ解除でプレイに戻る
		}
		
		    break;
	case Phase::kPlay:
		if (player_->IsDead()) {
			

			phase_ = Phase::kDeath;
		}
		else if (goal_->isGoal()) {
			phase_ = Phase::kClear;
		}
		else if (Input::GetInstance()->TriggerKey(DIK_P)||((state_.Gamepad.wButtons & XINPUT_GAMEPAD_START))&&!(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_START)) {
			phase_ = Phase::kPause; // ポーズに遷移
		}
		const Vector3 deathParticlesPosition = player_->GetWorldTransform().translation_;

		deathParticles_ = new DeathParticles;
		deathParticles_->Initialze(deathParticlesModel_, &camera_, deathParticlesPosition);

		break;


	case Phase::kDeath:
		/*if (deathParticles_&&deathParticles_->IsFinished()) {
			finished_ = true;
		}*/
	
		break;

	case Phase::kClear:
		// クリア処理
		// ここでは何もしないが、必要に応じてクリア処理を追加する
		break;

	case Phase::kPause:

		// ポーズ中の処理
		if (Input::GetInstance()->TriggerKey(DIK_P)||((state_.Gamepad.wButtons & XINPUT_GAMEPAD_START))&&!(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_START)) {
			phase_ = Phase::kPlay; // ポーズ解除でプレイに戻る
		}
		break;
	}
	

	

}

// ゲームシーンの更新
void GameScene::Update() {
	Input::GetInstance()->GetJoystickState(0, state_);
	Input::GetInstance()->GetJoystickStatePrevious(0, prevState_);


		hitEffects_.remove_if([](HitEffect *hitEffect) {
		if (hitEffect->IsDead()) {
			delete hitEffect;

			return true;
		}
		return false;
	});

	enemies_.remove_if([](Enemy* enemy) {

		if (enemy->IsDead()) {
			delete enemy;
			return true; // 削除する場合はtrueを返す
		}
		return false;
		
		
		
	});

	gaid_->Update();
	

	ChangePhase();
	
	switch (phase_) {

		case GameScene::Phase::kFadeIn:
		// フェードの更新
		fade_->Update();
		if (fade_->IsFinished()) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = GameScene::Phase::kStart;
		}
		// スカイドームの更新
		skydome_->Update();
		cameraControlle_->Update();
		//player_->Update();
		//// エネミー
		//for (Enemy* enemy : enemies_) {
		//	enemy->Update();	
		//}
		//for (HitEffect* hitEffect:hitEffects_){
		//	hitEffect->Update();
		//}
		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* WorldTransformBlock : worldTransformBlockLine) {
				if (!WorldTransformBlock) {
					continue;
				}
				WorldTransformUpdate(WorldTransformBlock);
			}
		}
		goal_->Update();
		break;

		case GameScene::Phase::kStart:

		//phase_ = GameScene::Phase::kPlay;
		    // スカイドームの更新
		    skydome_->Update();
		    // カメラの更新
		    cameraControlle_->Update();
		    // player_の更新
		  //  player_->Update();
		    // エネミー
				
			break;

	case GameScene::Phase::kPlay:
		
		// スカイドームの更新
		skydome_->Update();
		// カメラの更新
		cameraControlle_->Update();
		// player_の更新
		player_->Update();
		// エネミー
		for (Enemy* enemy : enemies_) {
			enemy->Update();	
		}
		for (HitEffect* hitEffect:hitEffects_){
			hitEffect->Update();
		}
		
		///// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* WorldTransformBlock : worldTransformBlockLine) {
				if (!WorldTransformBlock) {
					continue;
				}

				WorldTransformUpdate(WorldTransformBlock);
			}
		}
		if (goal_) {
			goal_->Update();
		}
		// ゴールの更新
	
		CheckAllCollisions();

		

		break;
	case GameScene::Phase::kDeath:
		worldTransformRetry_.translation_ = {camera_.translation_.x, camera_.translation_.y, -2.5f};
		WorldTransformUpdate(&worldTransformRetry_);
		if (deathParticles_&&deathParticles_->IsFinished()) {

			//fade_->Start(Fade::Status::FadeOut, 1.0f);
			        printf("Phase: kFadeOut に遷移\n");
			if (Input::GetInstance()->TriggerKey(DIK_SPACE)||((state_.Gamepad.wButtons & XINPUT_GAMEPAD_A))&&!(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)){
			phase_ = GameScene::Phase::kFadeOut;
			}
		}
		// スカイドームの更新
		skydome_->Update();
		// カメラの更新
		cameraControlle_->Update();
		// エネミー
		for (Enemy* enemy : enemies_) {
			enemy->Update();	
		}
		// デスパーティクル
		if (deathParticles_) {
			deathParticles_->Update();
		}
		for (HitEffect* hitEffect:hitEffects_){
			hitEffect->Update();
		}
		///// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* WorldTransformBlock : worldTransformBlockLine) {
				if (!WorldTransformBlock) {
					continue;
				}

				WorldTransformUpdate(WorldTransformBlock);
			}
		}
		// ゴールの更新
		if (goal_) {
			goal_->Update();
		}
		break;

		case GameScene::Phase::kFadeOut:
		// フェードの更新
		    fade_->Update();
		if (fade_->IsFinished()) {
		    if (clear_) {
			Gameend_ = true;
			} else {
			  finished_ = true;
			}
		  
	
		}

		
		skydome_->Update();
		cameraControlle_->Update();
		for (Enemy* enemy: enemies_) {
			enemy->Update();	
		}
		for (HitEffect* hitEffect:hitEffects_){
			hitEffect->Update();
		}
		break;
	    case GameScene::Phase::kClear:
		    // クリア処理
		    // ここでは何もしないが、必要に応じてクリア処理を追加する
			worldTransformClear_.translation_ = {camera_.translation_.x, camera_.translation_.y, -2.5f};

			WorldTransformUpdate(&worldTransformClear_);
			if (Input::GetInstance()->TriggerKey(DIK_SPACE)||((state_.Gamepad.wButtons & XINPUT_GAMEPAD_A))&&!(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
				phase_ = GameScene::Phase::kFadeOut;
		    }
		
		    skydome_->Update();
		    cameraControlle_->Update();
			for (Enemy* enemy: enemies_) {
				enemy->Update();	
			}
			for (HitEffect* hitEffect:hitEffects_){
				hitEffect->Update();
			}
			// ブロックの更新
			for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
				for (WorldTransform* WorldTransformBlock : worldTransformBlockLine) {
					if (!WorldTransformBlock) {
						continue;
					}
					WorldTransformUpdate(WorldTransformBlock);
				}
			}
		    break;

		case GameScene::Phase::kPause:

			/*if () {
		    }
			*/

		    break;
	}
}
// ゲームシーンの描画
void GameScene::Draw() {
	///
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	switch (phase_) {
	case GameScene::Phase::kClear:
		
		
		crearModel_->Draw(worldTransformClear_, camera_);
		break;
	case GameScene::Phase::kDeath:
		RetryModel_->Draw(worldTransformRetry_, camera_);
		break;
	}
	// カメラの描画
	if (!player_->IsDead()) {
		player_->Draw();
	}
	
	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {

		for (WorldTransform* WorldTransformBlock : worldTransformBlockLine) {
			if (!WorldTransformBlock) {
				continue;
			}
			blockM_->Draw(*WorldTransformBlock, camera_);
		}
	}
	// スカイドームの描画
	skydome_->Draw();
	///
	// エネミー
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}
	// デスパーティクル
	if (deathParticles_) {
		deathParticles_->Draw();
	}
	for (HitEffect* hitEffect:hitEffects_){
	hitEffect->Draw();
	}

	// ゴールの描画
	if (goal_) {
		goal_->Draw();
	}
	Model::PostDraw();

	Sprite::PreDraw(dxCommon->GetCommandList());

	fade_->Draw();
	
switch (phase_) {
	
	case GameScene::Phase::kFadeIn:
	case GameScene::Phase::kStart:
	case GameScene::Phase::kPause:
		gaid_->Draw();

		break;


	}
	
	Sprite::PostDraw();

	

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_TAB)) {
		if (!isDebugCameraActive_) {
			isDebugCameraActive_ = true;
		} else {
			isDebugCameraActive_ = false;
		}
	}
	// enemyのデバッグ描画

	
	int enemyIndex = 0;
	enemyIndex =static_cast<int> (enemies_.size());
	DebugText::GetInstance()->ConsolePrintf("Enemy Count: %d\n", enemyIndex);

#endif // _DEBUG
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		//
		camera_.TransferMatrix();
	} else {
		camera_.TransferMatrix();
	}
}
void GameScene::CreateHitEffect(const Vector3& position) {
	HitEffect* newHiteFFect= HitEffect::Create(position);
	hitEffects_.push_back(newHiteFFect);

}