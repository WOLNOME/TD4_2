#include "ModelManager.h"
#include "DirectXCommon.h"

namespace Norm {

	std::unique_ptr<ModelManager> ModelManager::instance_ = nullptr;

	ModelManager* ModelManager::GetInstance() {
		if (!instance_) {
			instance_ = std::unique_ptr<ModelManager>(new ModelManager());
		}
		return instance_.get();
	}

	void ModelManager::Initialize() {
	}

	void ModelManager::Finalize() {
		//インスタンスを削除
		instance_.reset();
	}

	void ModelManager::LoadModel(const std::string& filePath, ModelFormat format) {
		//重複防止処理！
		if (models_.contains(filePath)) {
			//読み込み済みなら早期return
			return;
		}
		//モデルの生成トファイル読み込み、初期化
		std::unique_ptr<Model> model = std::make_unique<Model>();
		model->Initialize(filePath, format);
		//モデルをmapコンテナに格納する
		models_.insert(std::make_pair(filePath, std::move(model)));

	}

	Model* ModelManager::FindModel(const std::string& filePath) {
		//読み込み済みモデルを検索
		if (models_.contains(filePath)) {
			//読み込みモデルを戻り値としてreturn
			return models_[filePath].get();
		}
		//ファイル名一致なし
		return nullptr;
	}

}