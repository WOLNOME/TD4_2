#include "GimmickManager.h"
#include "ExplosionGimmick.h"
void GimmickManager::Initialize()
{
	gimmicks_.clear();

}

void GimmickManager::Update()
{
    for (auto& gimmick : gimmicks_) {
        if (!gimmick) {
            continue;
        }

        gimmick->Update();
    }

}

void GimmickManager::Debug()
{
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
    const Vector3& position)
{
    std::unique_ptr<GimmickBase> gimmick = nullptr;

    switch (type) {
    case GimmickType::Explosion:
    {
        auto explosion = std::make_unique<ExplosionGimmick>();

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

    // Manager内のlightInfo_を全ギミックに参照させる
    gimmick->SetLightInfo(&lightInfo_);

    gimmick->Initialize();

    gimmicks_.push_back(std::move(gimmick));
}