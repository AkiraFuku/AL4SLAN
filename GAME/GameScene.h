#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include <vector>
#include "Skydome.h"
#include "MapchipField.h"
#include "CameraController.h"
#include "Enemy.h"
#include "DeathParticles.h"
#include "HitEffect.h"
#include "Goal.h"
#include "Gaid.h"
#include "IScene.h"
#include "PauseMenu.h"
#include "StageManager.h"
#include "ResultMenu.h"
using namespace KamataEngine ;


class GameScene : public IScene{
private:
	//3Dモデル
	Model* model_ = nullptr;
	Model* AttackModel_ = nullptr;
	
	Model* blockM_ = nullptr;


	// texture
	uint32_t teXtureHandle_ = 0;
	//
	
	// カメラ
	KamataEngine::Camera camera_;
	// 自キャラ
	Player* player_=nullptr;
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	bool isDebugCameraActive_ = false;
	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	// スカイドーム
	Skydome* skydome_ = nullptr;
	// スカイドームのモデル
	Model* modelSkydome_ = nullptr;
	// mapchipField
	 MapChipField* mapchipField_ ;
	// カメラクラス
	 CameraController* cameraControlle_;

	 //エネミー
	 std::vector<Enemy*> enemies_;
	 Model* enemy_model_=nullptr ;

	 //デスパーティクル
	 DeathParticles* deathParticles_;
	 Model* deathParticlesModel_=nullptr;

	 //フェーズ
	 enum class Phase{

		 kFadeIn, // フェードイン
		 kStart,   // ゲーム開始
		 kPlay, // プレイ中
		 kPause,   // pouse
		 kDeath, // 死亡
		 kClear,//クリア
		 kFadeOut, // フェードアウト
	 };
	 Phase phase_ = Phase::kPlay;

	 bool finished_ = false; // ゲーム終了フラグ	
	 bool Gameend_ = false;  // ゲーム終了フラグ
	 bool clear_ = false;    // クリアフラグ

	 // ヒットエフェクト
	 
	 Model* hitEffectModel_ = nullptr;
	 std::vector<HitEffect*> hitEffects_;
	 // ゴール
	 Goal* goal_ = nullptr;
	 // ゴールのモデル
	 Model* goalModel_ = nullptr;

	 Model* crearModel_ = nullptr;
	 WorldTransform worldTransformClear_;
	 Model* RetryModel_ = nullptr;
	 WorldTransform worldTransformRetry_;

	 XINPUT_STATE state_;
	 XINPUT_STATE prevState_;

	 // ゲーム説明
	 Gaid* gaid_ = nullptr;
	
	 //// BGM
	 //uint32_t bgmHandle_ = 0;

	 //uint32_t PlayHandle_ = 0;

	 int stageNo_ =-1;
	 
	 PauseMenu* pauseMenu_ = nullptr; // 変数を追加
	 ResultMenu* resultMenu_ = nullptr;

	 int nextSceneRequest_=0;

	 // === カウントダウン用変数 ===
    // カウントダウンタイマー (秒数計測用)
    float countdownTimer_ = 0.0f;

    // スプライト
    KamataEngine::Sprite* spriteCount_ = nullptr;
   
    // テクスチャハンドル
    uint32_t texHandle3_ = 0;
    uint32_t texHandle2_ = 0;
    uint32_t texHandle1_ = 0;
    uint32_t texHandleGo_ = 0;
public:
	
	~GameScene() override;
	GameScene(int stageNo);
    void Initialize() override;
    void Update() override;
    void Draw() override;

	void DrawBlock();
	//
	void GenerateBlock();

	void GenerateEnemy();	
	void GenerateGoal();
	Vector3 PlayerStartPosition() ;

	//当たり判定
	void CheckAllCollisions();
	// フェーズの切り替え
	void ChangePhase();

	// ゲーム終了
	bool IsFinished() const { return finished_; }
	bool IsGameEnd() const { return Gameend_; }
	 void CreateHitEffect(const Vector3 & position);

	 void EnemyCollision();
};
