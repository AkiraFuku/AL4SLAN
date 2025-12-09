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
	for (HitEffect* hitEffect : hitEffects_) {
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
				WorldTransformUpdate(worldTransform);
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
				goal_->Initialize(goalModel_, &camera_, Position);
			}
		}
	}
}

Vector3 GameScene::PlayerStartPosition() {

	uint32_t numBlockVertical = mapchipField_->GetNumBlockVertical();
	uint32_t numBlockHorizontal = mapchipField_->GetNumBlockHorizontal();
	Vector3 Position = {};
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

		if (enemy->IsCollisionDisabled() || enemy->InCamera()) {
			continue; // 衝突判定を無効にしている敵はスキップ
		}
		aabb2 = enemy->GetAABB();
		if (IsCollision(aabb1, aabb2)) {

			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
		// 攻撃判定
		if (player_->isAttack()) {
			if (IsCollision(attackAABB, aabb2)) {
				enemy->HitAttack(player_);
			}
		}
	}
	// ゴール
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
	AttackModel_ = Model::CreateFromOBJ("attack_effect", true);

	// 自キャラの初期化
	Vector3 playerPosition = PlayerStartPosition();
	player_->Initialize(model_, AttackModel_, teXtureHandle_, &camera_, playerPosition);
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

	// デスパーティクル
	deathParticlesModel_ = Model::CreateFromOBJ("deathParticle");
	// フェーズ
	phase_ = Phase::kFadeIn;
	// フェード
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
		if (Input::GetInstance()->TriggerKey(DIK_SPACE) || ((state_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) && !(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
			phase_ = Phase::kPlay; // ポーズ解除でプレイに戻る
		}

		break;
	case Phase::kPlay:
		if (player_->IsDead()) {

			phase_ = Phase::kDeath;
		} else if (goal_->isGoal()) {
			phase_ = Phase::kClear;
		} else if (Input::GetInstance()->TriggerKey(DIK_P) || ((state_.Gamepad.wButtons & XINPUT_GAMEPAD_START)) && !(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_START)) {
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
		if (Input::GetInstance()->TriggerKey(DIK_P) || ((state_.Gamepad.wButtons & XINPUT_GAMEPAD_START)) && !(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_START)) {
			phase_ = Phase::kPlay; // ポーズ解除でプレイに戻る
		}
		break;
	}
}

// ゲームシーンの更新
void GameScene::Update() {
	Input::GetInstance()->GetJoystickState(0, state_);
	Input::GetInstance()->GetJoystickStatePrevious(0, prevState_);

	hitEffects_.remove_if([](HitEffect* hitEffect) {
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

		goal_->Update();
		break;

	case GameScene::Phase::kStart:

		// phase_ = GameScene::Phase::kPlay;
		//  スカイドームの更新
		skydome_->Update();
		// カメラの更新
		cameraControlle_->Update();

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
		EnemyCollision();
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
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
		if (deathParticles_ && deathParticles_->IsFinished()) {

			// fade_->Start(Fade::Status::FadeOut, 1.0f);
			printf("Phase: kFadeOut に遷移\n");
			if (Input::GetInstance()->TriggerKey(DIK_SPACE) || ((state_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) && !(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
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
		EnemyCollision();
		// デスパーティクル
		if (deathParticles_) {
			deathParticles_->Update();
		}
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
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
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
		}
		break;
	case GameScene::Phase::kClear:
		// クリア処理
		// ここでは何もしないが、必要に応じてクリア処理を追加する
		worldTransformClear_.translation_ = {camera_.translation_.x, camera_.translation_.y, -2.5f};

		WorldTransformUpdate(&worldTransformClear_);
		if (Input::GetInstance()->TriggerKey(DIK_SPACE) || ((state_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) && !(prevState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
			phase_ = GameScene::Phase::kFadeOut;
		}

		skydome_->Update();
		cameraControlle_->Update();
		/*for (Enemy* enemy : enemies_) {
		    enemy->Update();
		}*/
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
		}
		// ブロックの更新

		break;

	case GameScene::Phase::kPause:

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

	DrawBlock();

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
	for (HitEffect* hitEffect : hitEffects_) {
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
	enemyIndex = static_cast<int>(enemies_.size());
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
void GameScene::DrawBlock() {

	// 1. カメラの位置を取得 (WorldTransformではない方のCameraクラスのtranslation_を使用)
	Vector3 cameraPos = camera_.translation_;

	// 2. カメラの視野範囲（ワールド座標）を決定
	// ここではブロック1つを1.0fとし、画面外に余裕を持たせるため、
	// 視野範囲をカメラの中心からX軸±20、Y軸±15と仮定します。
	// ※この値は画面サイズやカメラ設定に合わせて調整が必要です。
	const float kViewRangeX = 20.0f;
	const float kViewRangeY = 15.0f;

	// 3. 描画するブロックのワールド座標範囲を計算
	float minX_world = cameraPos.x - kViewRangeX;
	float maxX_world = cameraPos.x + kViewRangeX;
	float minY_world = cameraPos.y - kViewRangeY;
	float maxY_world = cameraPos.y + kViewRangeY;

	// 4. ワールド座標をマップインデックスに変換（ブロックサイズ1.0fと仮定）
	int minX_index = (int)std::floor(minX_world);
	int maxX_index = (int)std::ceil(maxX_world);
	int minY_index = (int)std::floor(minY_world);
	int maxY_index = (int)std::ceil(maxY_world);

	// 5. マップの境界内にインデックスをクランプ（範囲を制限）
	// worldTransformBlocks_のサイズを取得
	int max_row = (int)worldTransformBlocks_.size();
	// 1行目のサイズを列数と仮定（マップが空でなければ）
	int max_col = (max_row > 0) ? (int)worldTransformBlocks_[0].size() : 0;

	// Y軸（行）の開始と終了インデックス
	int startY = max(0, minY_index);
	int endY = min(max_row, maxY_index);

	// X軸（列）の開始と終了インデックス
	int startX = max(0, minX_index);
	int endX = min(max_col, maxX_index);

	// 6. 描画ループを修正し、計算した範囲内だけを処理
	// ブロック
	for (int y = startY; y < endY; ++y) {
		for (int x = startX; x < endX; ++x) {
			// worldTransformBlocks_は [y][x] の順にアクセス
			WorldTransform* WorldTransformBlock = worldTransformBlocks_[y][x];

			// nullptrチェックは残します
			if (!WorldTransformBlock) {
				continue;
			}
			blockM_->Draw(*WorldTransformBlock, camera_);
		}
	}
}
void GameScene::CreateHitEffect(const Vector3& position) {
	HitEffect* newHiteFFect = HitEffect::Create(position);
	hitEffects_.push_back(newHiteFFect);
}

void GameScene::EnemyCollision() {
	// リストの先頭からチェック
	auto itA = enemies_.begin();
	for (; itA != enemies_.end(); ++itA) {
		Enemy* enemyA = *itA;

		// 次の要素からチェックを開始することで、(A, B) の重複判定と (A, A) の自己判定を防ぐ
		auto itB = itA;
		itB++;

		for (; itB != enemies_.end(); ++itB) {
			Enemy* enemyB = *itB;

			// 死亡している、またはカメラ外のエネミーはスキップ
			if (enemyA->IsDead() || enemyB->IsDead())
				continue;
			if (enemyA->InCamera() && enemyB->InCamera())
				continue;

			// AABB（当たり判定ボックス）を取得
			AABB aabb1 = enemyA->GetAABB();
			AABB aabb2 = enemyB->GetAABB();

			// AABB同士が当たっているか判定
			if (aabb1.min.x < aabb2.max.x && aabb1.max.x > aabb2.min.x && aabb1.min.y < aabb2.max.y && aabb1.max.y > aabb2.min.y && aabb1.min.z < aabb2.max.z && aabb1.max.z > aabb2.min.z) {

				// 重なり量を計算
				float overlapX = min(aabb1.max.x, aabb2.max.x) - max(aabb1.min.x, aabb2.min.x);
				float overlapY = min(aabb1.max.y, aabb2.max.y) - max(aabb1.min.y, aabb2.min.y);

				// 上下の位置関係を判定
				Enemy* upperEnemy = (aabb1.min.y > aabb2.min.y) ? enemyA : enemyB;
				Enemy* lowerEnemy = (upperEnemy == enemyA) ? enemyB : enemyA;

				// --- 縦方向の衝突（踏みつけ判定） ---
				// Yの重なりの方が小さい、かつ 上にいるキャラが落下中
				if (overlapY < overlapX) {
					if (upperEnemy->GetVelocity().y <= 0.0f) {
						// 下のエネミーの頭頂部のY座標を計算
						// (注: Enemy::kHeightなどはprivateなので、GetWorldPosition等から計算するか、Getterを追加推奨)
						float lowerTopY = lowerEnemy->GetWorldPosition().y + (0.8f / 2.0f);
						upperEnemy->OnLandOnEnemy(lowerTopY);
					}
				}
				// --- 横方向の衝突（押し合い） ---
				else {
					Vector3 posA = enemyA->GetWorldPosition();
					Vector3 posB = enemyB->GetWorldPosition();

					// X軸の押し出し量を計算（重なっている分を半分ずつ押し戻す）
					float pushBackX = overlapX / 2.0f;

					// 位置関係によって左右に押し出す + 速度反転
					if (posA.x < posB.x) {
						// Aが左、Bが右
						enemyA->AddPosition({-pushBackX, 0, 0}); // 左へ押し戻す
						enemyB->AddPosition({pushBackX, 0, 0});  // 右へ押し戻す

						// 互いに相手の方を向いていたら反転させる
						if (enemyA->GetVelocity().x > 0)
							enemyA->OnCollisionWithEnemy();
						if (enemyB->GetVelocity().x < 0)
							enemyB->OnCollisionWithEnemy();
					} else {
						// Aが右、Bが左
						enemyA->AddPosition({pushBackX, 0, 0});  // 右へ押し戻す
						enemyB->AddPosition({-pushBackX, 0, 0}); // 左へ押し戻す

						if (enemyA->GetVelocity().x < 0)
							enemyA->OnCollisionWithEnemy();
						if (enemyB->GetVelocity().x > 0)
							enemyB->OnCollisionWithEnemy();
					}
				}
			}
		}
	}
}