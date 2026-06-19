#include "CombinedParticleManager.h"
#include "CombinedParticle.h"
#include "ParticleManager.h"
#include "RandomStringUtil.h"

namespace Norm {

	std::unique_ptr<CombinedParticleManager> CombinedParticleManager::instance_ = nullptr;

	CombinedParticleManager* CombinedParticleManager::GetInstance() {
		if (!instance_) {
			instance_ = std::unique_ptr<CombinedParticleManager>(new CombinedParticleManager());
		}
		return instance_.get();
	}

	void CombinedParticleManager::Initialize() {
	}

	void CombinedParticleManager::Update() {
		//複合パーティクルが一つもセットされていなかったら抜ける
		if (particles.empty()) return;

		//各パーティクルの更新
		for (const auto& particle : particles) {
			//更新処理
			particle.second->Update();
		}
	}

	void CombinedParticleManager::Finalize() {
		//インスタンスを削除
		instance_.reset();
	}

	void CombinedParticleManager::Regist(const std::string& name, CombinedParticle* particle) {
		//重複チェック
		if (particles.find(name) != particles.end()) {
			return;
		}
		//登録
		particles[name] = particle;
	}

	void CombinedParticleManager::Delete(const std::string& name) {
		// 名前がコンテナ内に存在するかチェック
		auto it = particles.find(name);
		if (it != particles.end()) {
			particles.erase(it);  // コンテナから削除
		}
	}

	std::string CombinedParticleManager::GenerateName(const std::string& name) {
		// 出力する名前
		std::string outputName = name + "_" + RandomStringUtil::GenerateRandomString(3);

		// 重複チェック用のラムダ式
		std::function<void(const std::string&)> checkDuplicate = [&](const std::string& name) {
			// 重複しているかチェック
			if (particles.find(name) != particles.end()) {
				// 重複しているので名前を変更
				outputName = name + "_" + RandomStringUtil::GenerateRandomString(3);
				checkDuplicate(outputName);
			}
			};

		// 重複チェック
		checkDuplicate(outputName);

		// 最終的に出力
		return outputName;
	}

}

