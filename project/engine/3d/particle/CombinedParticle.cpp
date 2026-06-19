#include "CombinedParticle.h"
#include "ParticleManager.h"
#include "CombinedParticleManager.h"
#include "LineManager.h"
#include <cassert>
#include <filesystem>
#undef min
#undef max

namespace Norm {

	auto makeTransformFromJson = [](const json& t) -> TransformEuler {
		return TransformEuler{
			{ t["Scale"]["x"],     t["Scale"]["y"],     t["Scale"]["z"] },
			{ t["Rotate"]["x"],    t["Rotate"]["y"],    t["Rotate"]["z"] },
			{ t["Translate"]["x"], t["Translate"]["y"], t["Translate"]["z"] }
		};
		};

	CombinedParticle::~CombinedParticle() {
		//マネージャーから登録解除
		CombinedParticleManager::GetInstance()->Delete(name_);
	}

	void CombinedParticle::Initialize(const std::string& _name, const std::string& _comParticleFileName, bool _isEditor) {
		//名前をセット
		name_ = _name;
		//基準のトランスフォームを初期化
		baseTransform_ = {
			.scale = {1,1,1},
			.rotate = {0,0,0},
			.translate = {0,0,0}
		};
		//再生情報を初期化
		playInfo_ = {
			.isPlay = false,
			.isRepeat = false,
			.elapsedTimer = 0.0f,
			.duration = 0.0f
		};

		//フォルダパス
		std::string folderPath = "Resources/particles/" + _comParticleFileName;
		std::vector<std::string> dataFiles;
		//ディレクトリ内のファイルを列挙
		for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
			if (entry.is_regular_file()) {
				//拡張子が .json かを判別
				if (entry.path().extension() == ".json") {
					//データファイルコンテナに格納
					dataFiles.push_back(entry.path().generic_string());
				}
			}
		}
		//データファイルコンテナを使ってパーティクルのコンテナに内部データを移す
		for (const auto& dataFile : dataFiles) {
			//初期化
			std::string fileName = dataFile.substr(
				dataFile.find_last_of("/") + 1,
				dataFile.rfind(".json") - dataFile.find_last_of("/") - 1
			);		//ファイル名(〇〇/△△.json→△△)
			std::string relativePath = _comParticleFileName + "/" + fileName;	//データの相対パス
			//パーティクルを生成
			SingleParticleInfo sParticle;

			sParticle.particle = std::make_unique<Particle>();
			if (_isEditor) {
				sParticle.particle->Initialize(fileName, relativePath);
			}
			else {
				sParticle.particle->Initialize(ParticleManager::GetInstance()->GenerateName(fileName), relativePath);
			}
			//発生開始時間&終了時間のセット
			json data = JsonUtil::GetJsonData(folderPath + "/" + fileName);
			sParticle.startTime = data["StartTime"];
			sParticle.endTime = data["EndTime"];
			sParticle.localTransform = makeTransformFromJson(data["LocalTransform"]);
			//全体尺のうち長ければ更新
			playInfo_.duration = std::max(playInfo_.duration, sParticle.endTime + sParticle.particle->param_["LifeTime"]["Max"]);

			//コンテナに格納
			particles_.push_back(std::move(sParticle));
		}

		//最後にマネージャーに登録
		CombinedParticleManager::GetInstance()->Regist(name_, this);

	}

	void CombinedParticle::Debug() {
#ifdef _DEBUG
		//基準のトランスフォームをデバッグ表示
		Sphere baseSphere;
		baseSphere.center = baseTransform_.translate;
		baseSphere.radius = 0.2f;
		MyMath::CreateLineSphere(baseSphere, { 0,1,0,1 }, 12);

#endif // _DEBUG

	}

	std::vector<std::string> CombinedParticle::GetAllHandleName() {
		std::vector<std::string> result;
		//全てのパーティクルを走査
		for (auto& sParInfo : particles_) {
			//キーをresultに格納
			result.push_back(sParInfo.particle->name_);
		}
		return result;
	}

	void CombinedParticle::SetIsRepeat(bool isRepeat) {
		playInfo_.isRepeat = isRepeat;
		//全体尺の再計算
		ReculculateDuration();

	}

	void CombinedParticle::Update() {
		//particlesが空なら抜ける
		if (particles_.empty()) {
			return;
		}

		//再生フラグがオンなら
		if (playInfo_.isPlay) {
			//タイマーをカウント
			playInfo_.elapsedTimer += kDeltaTime;
			//全てのパーティクルを走査
			for (auto& sParInfo : particles_) {
				//再生フラグがオフの時
				if (!sParInfo.particle->emitter_.isPlay) {
					//oneShotの場合
					if (sParInfo.endTime == 0.0f) {
						//タイマーがstartTime~にある＆現在時間-startTime<=kDeltaTimeの時
						if (playInfo_.elapsedTimer > sParInfo.startTime && playInfo_.elapsedTimer - sParInfo.startTime <= kDeltaTime) {
							//パーティクルをオンにする
							sParInfo.particle->emitter_.isPlay = true;
							//描画フラグをオンにする
							sParInfo.particle->emitter_.isDraw = true;

						}
						continue;
					}

					//タイマーがstartTime~endTimeにある時
					if (playInfo_.elapsedTimer > sParInfo.startTime && playInfo_.elapsedTimer < sParInfo.endTime) {
						//パーティクルをオンにする
						sParInfo.particle->emitter_.isPlay = true;
						//描画フラグをオンにする
						sParInfo.particle->emitter_.isDraw = true;
					}
				}
				//再生フラグがオンの時
				else {
					//タイマーがstartTime~endTimeの外にある時
					if (playInfo_.elapsedTimer < sParInfo.startTime || playInfo_.elapsedTimer > sParInfo.endTime) {
						//パーティクルをオフにする
						sParInfo.particle->emitter_.isPlay = false;
					}
				}
			}
			//タイマーが全体の尺を超過したら
			if (playInfo_.elapsedTimer > playInfo_.duration) {
				//連続再生しない場合
				if (!playInfo_.isRepeat) {
					//再生フラグをオフにする
					playInfo_.isPlay = false;
					//全てのパーティクルを走査
					for (auto& sParInfo : particles_) {
						//全てのパーティクルを停止させる
						sParInfo.particle->emitter_.isPlay = false;
						//全ての描画をオフにする
						sParInfo.particle->emitter_.isDraw = false;

					}
				}
				//タイマーをリセット(共通)
				playInfo_.elapsedTimer = 0.0f;
			}
		}
		else {
			//再生フラグがオフなら全てのパーティクルを停止させる
			for (auto& sParInfo : particles_) {
				sParInfo.particle->emitter_.isPlay = false;
			}
		}

		//全パーティクルの走査
		for (auto& sParInfo : particles_) {
			//エミッターのトランスフォームを更新
			sParInfo.particle->emitter_.worldTransform = MyMath::Combine(baseTransform_, sParInfo.localTransform);
		}
	}

	void CombinedParticle::ReculculateDuration() {
		playInfo_.duration = 0.0f;
		for (auto& sParInfo : particles_) {
			if (playInfo_.isRepeat) {
				playInfo_.duration = std::max(playInfo_.duration, sParInfo.endTime);
			}
			else {
				playInfo_.duration = std::max(playInfo_.duration, sParInfo.endTime + sParInfo.particle->param_["LifeTime"]["Max"]);
			}
		}
	}

	bool CombinedParticle::AddParticle(const std::string& _fileName, float _startTime, float _endTime) {
		//コンテナのサイズが最大数を超えていたらreturn
		if (particles_.size() >= kMaxCombinedParticles) {
			return false;
		}
		//新しいパーティクルを作成
		SingleParticleInfo newParticle;
		newParticle.startTime = _startTime;
		newParticle.endTime = _endTime;
		newParticle.particle = std::make_unique<Particle>();
		//パーティクルの名前を決める(例 : basic.json→basic)
		std::string cutJson = _fileName.substr(0, _fileName.rfind(".json"));
		std::string name = ParticleManager::GetInstance()->GenerateName(cutJson);
		//パーティクルの初期化(名前はnameベースで適当に生成)
		newParticle.particle->Initialize(name, cutJson);
		//トランスフォームを計算
		newParticle.localTransform = makeTransformFromJson(newParticle.particle->param_["LocalTransform"]);
		newParticle.particle->emitter_.worldTransform = MyMath::Combine(baseTransform_, newParticle.localTransform);
		//パーティクルをコンテナに追加
		particles_.push_back(std::move(newParticle));

		return true;
	}

	void CombinedParticle::RemoveParticle(const std::string& _handleName) {
		particles_.erase(
			std::remove_if(
				particles_.begin(),
				particles_.end(),
				[&](const SingleParticleInfo& info) {
					return info.particle->name_ == _handleName;
				}
			),
			particles_.end()
		);
	}

	const std::unordered_map<std::string, json> CombinedParticle::GetParams() {
		std::unordered_map<std::string, json> result;
		//全パーティクルを走査　(keyは.jsonを省いた形を想定)
		for (auto& sParInfo : particles_) {
			//keyを指定してパラメーターを格納
			result[sParInfo.particle->name_] = sParInfo.particle->GetParam();
		}

		return result;
	}

	void CombinedParticle::SetParams(const std::unordered_map<std::string, json>& _params) {
		//取得したパラメーターを走査 (keyは.jsonを省いた形を想定)
		for (auto& [key, param] : _params) {
			//パーティクルを走査
			bool isFind = false;
			for (auto& sParInfo : particles_) {
				//keyとパーティクル名が一致したら
				if (sParInfo.particle->name_ == key) {
					//パラメーターをセット
					sParInfo.particle->param_ = param;
					isFind = true;
					//即時反映
					sParInfo.particle->TraceEmitterForCS();
					sParInfo.particle->TraceJsonInfoForCS();
					//ローカルトランスフォームは個別で反映
					sParInfo.localTransform.translate = {
						sParInfo.particle->jsonInfoForCS_.localTransform.translate.x,
						sParInfo.particle->jsonInfoForCS_.localTransform.translate.y,
						sParInfo.particle->jsonInfoForCS_.localTransform.translate.z,
					};
					sParInfo.localTransform.rotate = {
						sParInfo.particle->jsonInfoForCS_.localTransform.rotate.x,
						sParInfo.particle->jsonInfoForCS_.localTransform.rotate.y,
						sParInfo.particle->jsonInfoForCS_.localTransform.rotate.z,
					};
					sParInfo.localTransform.scale = {
						sParInfo.particle->jsonInfoForCS_.localTransform.scale.x,
						sParInfo.particle->jsonInfoForCS_.localTransform.scale.y,
						sParInfo.particle->jsonInfoForCS_.localTransform.scale.z,
					};

					break;
				}
			}

			if (!isFind) {
				//ここへくる事は想定していない
				assert(false && "単パーティクルの名前変更時に反映されていない可能性があります");
			}
		}
	}

}