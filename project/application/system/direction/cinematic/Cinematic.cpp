#include "Cinematic.h"
#include <BlackBoard.h>

//アプリケーション
#include "application/system/direction/builder/CinematicBuilder.h"

Cinematic::Cinematic() {
}

Cinematic::~Cinematic() {
}

void Cinematic::Initialize(const std::string& _fileName, Norm::BlackBoard* _blackBoard) {
	//ビルダーを使ってshots_を初期化
	shots_ = CinematicBuilder::BuildCinematic(_fileName, _blackBoard);

	//変数の初期化
	currentIndex_ = 0u;
	isAllShotsFinished_ = false;

}

void Cinematic::Update() {
	//全てのショットが終了していたら更新しない
	if (isAllShotsFinished_) {
		return;
	}

	//最初の初期化処理（一度きり）
	if (shots_[currentIndex_]->GetShotResult() == ShotResult::Idle) {
		shots_[currentIndex_]->Initialize();
	}

	//現在のショットを更新
	shots_[currentIndex_]->Update();

	//ショットの結果を取得
	ShotResult result = shots_[currentIndex_]->GetShotResult();

	//結果が終了なら
	if (result == ShotResult::Finish) {
		//現在のショットの終了処理
		shots_[currentIndex_]->Finalize();
		//次のショットへ
		currentIndex_++;

		//次のショットが存在すれば
		if (currentIndex_ < shots_.size()) {
			//次のショットを初期化
			shots_[currentIndex_]->Initialize();
		}
		else {
			//全てのショットの終了通知を出す
			isAllShotsFinished_ = true;
		}

	}

}

void Cinematic::Debug() {
#ifdef _DEBUG
	//ショットのデバッグ処理
	shots_[currentIndex_]->Debug();
#endif // _DEBUG

}