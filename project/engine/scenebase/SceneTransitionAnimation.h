#pragma once
#pragma once
#include "MyMath.h"
#include <Sprite.h>
#include <wrl.h>
#include <cstdint>
#include <memory>

namespace Norm {

	/// <summary>
	/// シーン遷移アニメーションの処理を行うクラス
	/// </summary>
	class SceneTransitionAnimation {
	public:
		/// ============================== ///
		///		列挙体
		/// ============================== ///

		/// <summary>
		/// 遷移の状態
		/// </summary>
		enum class State {
			NONE,
			UPDATE_IN,
			END_IN,
			UPDATE_OUT,
			END_OUT,
			END_ALL,
		};

		/// <summary>
		/// 遷移の種類
		/// </summary>
		enum class Type {
			NONE,
			FADE,
			SLIDEUP,
			SLIDEDOWN,
		};

		/// <summary>
		/// 遷移の設定
		/// </summary>
		enum class Option {
			NONE,
			SHAKE,
		};

		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// コンストラクタ
		/// </summary>
		SceneTransitionAnimation();
		/// <summary>
		/// デストラクタ
		/// </summary>
		~SceneTransitionAnimation();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		/// <summary>
		/// 遷移開始時処理
		/// </summary>
		void StartTransition();
		/// <summary>
		/// イン遷移更新処理
		/// </summary>
		void UpdateIn();
		/// <summary>
		/// イン遷移終了処理
		/// </summary>
		void EndIn();
		/// <summary>
		/// アウト遷移更新処理
		/// </summary>
		void UpdateOut();
		/// <summary>
		/// アウト遷移終了処理
		/// </summary>
		void EndOut();
		/// <summary>
		/// 全遷移終了時処理
		/// </summary>
		void EndAll();

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// 状態の取得
		/// </summary>
		/// <returns>状態</returns>
		State GetState() const { return state_; }
		/// <summary>
		/// 遷移中フラグの取得
		/// </summary>
		/// <returns>遷移中フラグ</returns>
		bool GetIsTransitioning() const { return isTransitioning_; }

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// インとアウトの遷移種類のセット
		/// </summary>
		/// <param name="_in">セットするイン遷移種類</param>
		/// <param name="_out">セットするアウト遷移種類</param>
		void SetType(Type _in, Type _out) { inType_ = _in; outType_ = _out; }
		/// <summary>
		/// 遷移設定のセット
		/// </summary>
		/// <param name="_option">セットする遷移設定</param>
		void SetOption(Option _option) { option_ = _option; }
		/// <summary>
		/// 遷移時間(半周)のセット
		/// </summary>
		/// <param name="_time">セットする遷移時間</param>
		void SetTime(float _time) { time_ = _time; }
		/// <summary>
		/// 遷移アニメーションに使用するテクスチャのセット
		/// </summary>
		/// <param name="_textureHandle">セットするテクスチャのハンドル</param>
		void SetTexture(uint32_t _textureHandle);

	private:
		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//スプライト
		std::unique_ptr<Sprite> sprite_ = nullptr;

		//遷移の状態
		State state_;
		//遷移の種類
		Type inType_;
		Type outType_;
		//遷移のオプション
		Option option_;
		//時間
		float time_;
		float timer_;
		//遷移中フラグ
		bool isTransitioning_ = false;

	};

}