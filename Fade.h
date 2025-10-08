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

		void Initialize();
		void Update();
		void Draw();
		void Start(Status status, float duration) ;
		void Stop();
		bool IsFinished() const ;

	private:
	    Sprite* sprite_ = nullptr;
	    Status status_ = Status::None;
	    // フェードの持続時間
	    float duration_ = 0.0f;
	    // フェードの経過時間
	    float counter_ = 0.0f;
};
