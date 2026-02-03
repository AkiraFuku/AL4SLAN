#pragma once
#include <KamataEngine.h>
using namespace KamataEngine;
class Fade {
	public:
	enum class Status {
		None,
		FadeIn,
		FadeOut

	};
	// ★追加: シングルトンインスタンス取得
	static Fade* GetInstance();

	// ★追加: コピーコンストラクタと代入演算子を無効化（複製防止）
	Fade(const Fade&) = delete;
	Fade& operator=(const Fade&) = delete;

		void Initialize();
		void Update();
		void Draw();
		void Start(Status status, float duration) ;
		void Stop();
		bool IsFinished() const ;

	private:

		// ★変更: コンストラクタとデストラクタをprivateにする
	Fade() = default;
	~Fade();

	    Sprite* sprite_ = nullptr;
	    Status status_ = Status::None;
	    // フェードの持続時間
	    float duration_ = 0.0f;
	    // フェードの経過時間
	    float counter_ = 0.0f;
};
