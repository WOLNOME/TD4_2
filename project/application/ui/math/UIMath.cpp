#include "UIMath.h"

#include "engine/base/windows/WinApp.h"
#include "engine/math/MyMath.h"

using namespace Norm;

Norm::Vector2 WorldToScreen(const Vector3 _pos, const Matrix4x4 _viewProjectionMatrix) {

	Matrix4x4 viewport = MyMath::MakeViewportMatrix(0, 0, static_cast<float>(WinApp::GetInstance()->kClientWidth), static_cast<float>(WinApp::GetInstance()->kClientHeight), 0, 1);

	Matrix4x4 viewProjectionViewport = _viewProjectionMatrix * viewport;

	//3Dオブジェクトの座標をスクリーン座標に変換する
	Vector3 screenPos = MyMath::Transform(_pos, viewProjectionViewport);

	return Vector2(screenPos.x, screenPos.y);
}

Norm::Vector3 ScreenToWorld(const Vector2 _pos, const Vector3 _cameraPos, const Matrix4x4 _viewProjection) {

	Vector3 screenPos = { _pos.x,_pos.y,0.0f };

	Matrix4x4 invViewProjection = MyMath::Inverse(_viewProjection);

	//マウス座標を正規化デバイス座標に変換
	float ndcX = (2.0f * screenPos.x / WinApp::GetInstance()->kClientWidth) - 1.0f;
	float ndcY = 1.0f - (2.0f * screenPos.y / WinApp::GetInstance()->kClientHeight);

	Vector3 pointNDC = { ndcX,ndcY,1.0f };

	//正規化デバイス座標をワールド座標に変換
	Vector3 mouseWorldPos = MyMath::Transform(pointNDC, invViewProjection);

	//カメラ座標からマウス座標への直線を生成
	Line line;
	line.diff = Vector3(mouseWorldPos - _cameraPos).Normalized();
	line.origin = _cameraPos;

	//XY平面を作成
	Plane YZPlane;
	YZPlane.normal = { 0,0,1 };
	YZPlane.distance = 0.0f;

	//直線と平面の交点を求める
	Vector3 closestPoint = MyMath::CollisionPoint(line, YZPlane);

	return closestPoint;
}