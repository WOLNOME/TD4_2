#include "BaseCamera.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include <vector>

namespace Norm {

	BaseCamera::BaseCamera()
		: fovY(0.45f)
		, aspectRatio(float(WinApp::kClientWidth) / float(WinApp::kClientHeight))
		, nearClip(0.1f)
		, farClip(100.0f) {
		worldTransform.Initialize();
		viewMatrix = MyMath::Inverse(worldTransform.GetWorldMatrix());
		projectionMatrix = MyMath::MakePerspectiveFovMatrix(fovY, aspectRatio, nearClip, farClip);
		viewProjectionMatrix = MyMath::Multiply(viewMatrix, projectionMatrix);
	}

	void BaseCamera::Initialize() {
		//座標変換用リソース
		viewProjectionResource_ = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(ViewProjectionTransformationMatrixForVS));
		viewProjectionResource_->Map(0, nullptr, reinterpret_cast<void**>(&viewProjectionData_));
		viewProjectionData_->matWorld = worldTransform.GetWorldMatrix();
		viewProjectionData_->matView = viewMatrix;
		viewProjectionData_->matProjection = projectionMatrix;
		//カメラ座標リソース
		cameraPositionResource_ = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(ViewProjectionTransformationMatrixForVS));
		cameraPositionResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraPositionData_));
		cameraPositionData_->worldPosition = worldTransform.GetWorldTranslate();
	}

	void BaseCamera::Update() {
		//カメラの更新
		UpdateMatrix();
	}

	void BaseCamera::UpdateMatrix() {
		//ワールドトランスフォームの更新
		worldTransform.UpdateMatrix();

		viewMatrix = MyMath::Inverse(worldTransform.GetWorldMatrix());
		projectionMatrix = MyMath::MakePerspectiveFovMatrix(fovY, aspectRatio, nearClip, farClip);
		viewProjectionMatrix = MyMath::Multiply(viewMatrix, projectionMatrix);

		viewProjectionData_->matWorld = worldTransform.GetWorldMatrix();
		viewProjectionData_->matView = viewMatrix;
		viewProjectionData_->matProjection = projectionMatrix;

		cameraPositionData_->worldPosition = worldTransform.GetWorldTranslate();
	}

	void BaseCamera::DebugWithImGui() {
#ifdef _DEBUG

		ImGui::Begin("カメラ");
		ImGui::Text("座標： (%.2f, %.2f, %.2f)", worldTransform.GetWorldTranslate().x, worldTransform.GetWorldTranslate().y, worldTransform.GetWorldTranslate().z);
		ImGui::Text("回転： (%.2f, %.2f, %.2f)", worldTransform.GetRotate().x, worldTransform.GetRotate().y, worldTransform.GetRotate().z);
		ImGui::End();

#endif // _DEBUG
	}

}