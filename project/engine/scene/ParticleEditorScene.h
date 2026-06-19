#pragma once
#include "BaseScene.h"
#include "DevelopCamera.h"
#include "json.hpp"
#include "WorldTransform.h"
#include <vector>
#include <string>

using json = nlohmann::json;

namespace Norm {

	/// <summary>
	/// パーティクルエディター専用シーン
	/// </summary>
	class ParticleEditorScene : public BaseScene {
	private:
		/// ============================== ///
		///		列挙体
		///	============================== ///

		/// <summary>
		/// モード
		/// </summary>
		enum class Mode {
			kNone,			//未選択
			kEdit,			//パーティクルの編集
		};
		/// <summary>
		/// オプション
		/// (選択→確認→実行)の選択の部分
		/// </summary>
		enum class Option {
			kNone,						//未選択
			kReset,						//リセット
			kSave,						//セーブ
			kShowParticleFileDialog,	//パーティクルファイルのダイアログの表示
			kShowTextureFileDialog,		//テクスチャファイルのダイアログの表示
		};
		/// <summary>
		/// 確認
		///	(選択→確認→実行)の確認の部分
		/// </summary>
		enum class Check {
			kNone,		//未選択
			kContinue,	//続けるか
			kEditName,	//編集対象名
		};

		/// ============================== ///
		///		構造体
		///	============================== ///

		/// <summary>
		/// 状態管理
		/// </summary>
		struct EditorState {
			Mode mode = Mode::kNone;		//モード
			Option option = Option::kNone;	//オプション
			Check check = Check::kNone;		//確認
		};
		/// <summary>
		/// メンバ変数まとめた構造体
		/// </summary>
		struct Variable {

		};

	public:
		/// ============================== ///
		///		メンバ関数
		///	============================== ///

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize() override;
		/// <summary>
		/// 終了時
		/// </summary>
		void Finalize() override;
		/// <summary>
		/// 更新
		/// </summary>
		void Update() override;
		/// <summary>
		/// デバッグ処理
		/// </summary>
		void DebugWithImGui() override;

	private:
		/// ============================== ///
		///		非公開メンバ関数
		///	============================== ///

		/// <summary>
		/// 最初のメニュー画面時処理
		/// </summary>
		void StartWithImGui();
		/// <summary>
		/// エディター画面時処理
		/// </summary>
		void EditWithImGui();

		/// <summary>
		/// オプション処理
		/// </summary>
		void OptionWithImGui();
		/// <summary>
		/// 確認処理
		/// </summary>
		void CheckWithImGui();

		/// <summary>
		/// エディターのメイン処理
		/// </summary>
		void Editor();

		/// ============================== ///
		///		メンバ変数
		///	============================== ///

		//インプット
		Input* input_ = nullptr;
		//開発用カメラ
		std::unique_ptr<DevelopCamera> camera_ = nullptr;

		//天球と地面
		std::unique_ptr<Object3d> skydome_ = nullptr;
		WorldTransform skydomeWT_;
		uint32_t textureHandleGround_ = EOF;
		std::unique_ptr<Object3d> ground_ = nullptr;
		WorldTransform groundWT_;

		// 編集するパーティクル
		std::unique_ptr<CombinedParticle> cParticle_ = nullptr;
		// 編集するパラメーター
		std::unordered_map<std::string, json> cEditParam_;

		//ImGui操作用変数
		std::vector<std::string> particleFiles_;	//Resources/particles内ファイル一覧
		std::vector<std::string> textureFiles_;		//Resources/textures内ファイル一覧
		std::string particleFileName_;		//編集するパーティクルファイルの名前

		//選択中のパーティクルのハンドル名
		std::string selectedParticleHandle_;

		//状態管理用変数
		EditorState state_;
		//オプション処理で使う変数
		bool isShapeChange_ = false;
		bool isChangeTexture_ = false;
		//チェック処理で使う変数

		//エミッターのライン表示フラグ
		bool displayLineEmitter_ = true;

		//名前変更時に編集用文字列を保存しておく変数
		std::string particleName_;

		bool isLoop_ = true;

	};

}