#pragma once
#include "Particle.h"
#include "JsonUtil.h"
#include <MyMath.h>
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>
#include <queue>

namespace Norm {

	/// <summary>
	/// 複合パーティクルを管理するクラス
	/// </summary>
	class CombinedParticle {
		//パーティクルマネージャーに公開
		friend class ParticleManager;
		//複合パーティクルマネージャーに公開
		friend class CombinedParticleManager;
		//パーティクルエディターシーンに公開
		friend class ParticleEditorScene;
	public:
		/// ============================== ///
		///		構造体(public)
		/// ============================== ///

		/// <summary>
		/// 単パーティクル情報(これをつなぎ合わせて複合にする)
		/// </summary>
		struct SingleParticleInfo {
			std::unique_ptr<Particle> particle; // パーティクル本体
			TransformEuler localTransform;		// ローカルトランスフォーム
			float startTime = 0.0f;				// 発生開始時間
			float endTime = 0.0f;				// 発生終了時間
		};

	private:
		/// ============================== ///
		///		構造体(private)
		/// ============================== ///

		/// <summary>
		/// 再生情報
		/// </summary>
		struct PlayInfo {
			bool isPlay = false;		//再生フラグ
			bool isRepeat = false;		//連続再生フラグ
			float elapsedTimer = 0.0f;	//経過タイマー
			float duration = 0.0f;		//尺
		};

	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// デストラクタ
		/// </summary>
		~CombinedParticle();
		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="_name">名前</param>
		/// <param name="_comParticleFileName">複合パーティクルのファイル名</param>
		/// <param name="_isEditor">エディター用かどうか</param>
		void Initialize(const std::string& _name, const std::string& _comParticleFileName, bool _isEditor = false);
		/// <summary>
		/// デバッグ表示
		/// </summary>
		void Debug();

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// 基準のトランスフォームを取得する
		/// </summary>
		/// <returns>基準のトランスフォーム</returns>
		const TransformEuler& GetBaseTransform() { return baseTransform_; }
		/// <summary>
		/// 全てのハンドル名をコンテナとして渡す
		/// </summary>
		/// <returns>全てのハンドル名</returns>
		std::vector<std::string> GetAllHandleName();
		/// <summary>
		/// 再生フラグを取得する
		/// </summary>
		/// <returns>再生フラグ</returns>
		bool GetIsPlay() { return playInfo_.isPlay; }
		/// <summary>
		/// 連続再生フラグを取得する
		/// </summary>
		/// <returns>連続再生フラグ</returns>
		bool GetIsRepeat() { return playInfo_.isRepeat; }
		/// <summary>
		/// 全体尺を取得する
		/// </summary>
		/// <returns>全体尺</returns>
		float GetDuration() { return playInfo_.duration; }
		/// <summary>
		/// 経過タイマーを取得する
		/// </summary>
		/// <returns>経過時間</returns>
		float GetElapsedTimer() { return playInfo_.elapsedTimer; }

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// 基準のトランスフォームをセットする
		/// </summary>
		/// <param name="transform">基準トランスフォーム</param>
		void SetBaseTransform(const TransformEuler& transform) { baseTransform_ = transform; }
		/// <summary>
		/// 再生フラグをセットする
		/// </summary>
		/// <param name="isPlay">再生フラグ</param>
		void SetIsPlay(bool isPlay) { playInfo_.isPlay = isPlay; }
		/// <summary>
		/// 連続再生フラグをセットする
		/// </summary>
		/// <param name="isRepeat">連続再生フラグ</param>
		void SetIsRepeat(bool isRepeat);

	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// 更新処理(複合パーティクルマネージャー用)
		/// </summary>
		void Update();

		/// <summary>
		/// 全体尺の再計算
		/// </summary>
		void ReculculateDuration();

		/// <summary>
		/// パーティクルを追加(パーティクルエディター用)
		/// </summary>
		/// <param name="_fileName">ファイル名</param>
		/// <param name="_startTime">開始時間</param>
		/// <param name="_endTime">終了時間</param>
		/// <returns>成功したか</returns>
		bool AddParticle(const std::string& _fileName, float _startTime, float _endTime);
		/// <summary>
		/// パーティクルを削除(パーティクルエディター用)
		/// </summary>
		/// <param name="_id">ハンドル名</param>
		void RemoveParticle(const std::string& _handleName);

		/// ============================== ///
		///		getter(エディター専用)
		/// ============================== ///

		/// <summary>
		/// パラメーターの取得
		/// </summary>
		/// <returns>パラメーター</returns>
		const std::unordered_map<std::string, json> GetParams();

		/// ============================== ///
		///		setter(エディター専用)
		/// ============================== ///

		/// <summary>
		/// パラメーターのセット
		/// </summary>
		/// <param name="_params">パラメーター</param>
		void SetParams(const std::unordered_map<std::string, json>& _params);

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//名前
		std::string name_;

		//パーティクルのコンテナ
		std::vector<SingleParticleInfo> particles_;

		//再生情報
		PlayInfo playInfo_;

		//基準のトランスフォーム
		TransformEuler baseTransform_;

		//複合最大数
		const uint32_t kMaxCombinedParticles = 8;
	};

}