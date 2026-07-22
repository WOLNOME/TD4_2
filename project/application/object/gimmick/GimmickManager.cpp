#include "GimmickManager.h"
#include "ExplosionGimmick.h"
#include <cassert>

#include "application/system/LightManager.h"

void GimmickManager::Initialize(Norm::BaseCamera* _camera) {
	camera_ = _camera;
	gimmicks_.clear();
}

void GimmickManager::Update()
{
    for (auto& gimmick : gimmicks_) {

        gimmick->UpdateUI();

        if (!gimmick) {
            continue;
        }

		gimmick->Update();
	}

}

void GimmickManager::Debug() {
#ifdef _DEBUG

	for (auto& gimmick : gimmicks_) {
		if (!gimmick) {
			continue;
		}

		// 現状はExplosionGimmickだけ個別のDebugImGuiを持っている
		if (auto* explosion =
			dynamic_cast<ExplosionGimmick*>(gimmick.get())) {

			explosion->DebugImGui();
		}
	}

#endif
}

void GimmickManager::CreateGimmick(
	GimmickType type,
	const Vector3& position) {
	assert(lightManager_ && "ライトマネージャーが設定されていません");

	std::unique_ptr<GimmickBase> gimmick = nullptr;

	switch (type) {
	case GimmickType::Explosion:
	{
		auto explosion = std::make_unique<ExplosionGimmick>();
		explosion->SetLightManager(lightManager_);
		explosion->SetPosition(position);

		gimmick = std::move(explosion);
		break;
	}

	case GimmickType::HomingLauncher:
		// 後で追加
		break;

	case GimmickType::StunTrap:
		// 後で追加
		break;

	case GimmickType::None:
	default:
		return;
	}

	if (!gimmick) {
		return;
	}

	gimmick->Initialize(camera_);

	gimmicks_.push_back(std::move(gimmick));
}