#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Quaternion.h"
#include <vector>
#include <cstdint>
#include <numbers>

namespace Norm {

	///------------------------------------///
	///             列挙体
	///------------------------------------///

	//δタイム
	static const float kDeltaTime = 1.0f / 60.0f;
	//π
	static const float pi = std::numbers::pi_v<float>;
	//許容数(限りなく0.0fに近い値)
	static const float epsilon = 5e-4f;

	///------------------------------------///
	///             列挙体
	///------------------------------------///

	//ライトの種類
	enum LightKind {
		HalfLambert,
		Lambert,
		NoneLighting,
	};

	///------------------------------------///
	///             構造体
	///------------------------------------///

	/// <summary>
	/// トランスフォーム(オイラー角)
	/// </summary>
	struct TransformEuler {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};
	/// <summary>
	/// トランスフォーム(クォータニオン)
	/// </summary>
	struct TransformQuaternion {
		Vector3 scale;
		Quaternion rotate;
		Vector3 translate;
	};
	/// <summary>
	/// 球
	/// </summary>
	struct Sphere {
		Vector3 center;
		float radius;
	};
	/// <summary>
	/// AABB
	/// </summary>
	struct AABB {
		Vector3 min;
		Vector3 max;
	};
	/// <summary>
	/// OBB
	/// </summary>
	struct OBB {
		Vector3 center;				//!< 中心点
		Vector3 orientations[3];	//!< 座標軸。正規化・直交必須
		Vector3 size;				//!< 座標軸方向の長さの半分。中心から面までの距離
	};
	/// <summary>
	/// 直線
	/// </summary>
	struct Line {
		Vector3 origin;//始点
		Vector3 diff;//終点への差分ベクトル
	};
	/// <summary>
	/// 半直線
	/// </summary>
	struct Ray {
		Vector3 origin;//始点
		Vector3 diff;//終点への差分ベクトル
	};
	/// <summary>
	/// 線分
	/// </summary>
	struct Segment {
		Vector3 origin;//始点
		Vector3 diff;//終点への差分ベクトル
	};
	/// <summary>
	/// 平面
	/// </summary>
	struct Plane {
		Vector3 normal;//法線
		float distance;//距離
	};
	/// <summary>
	/// 三角形
	/// </summary>
	struct Triangle {
		Vector3 vertices[3];
	};
	/// <summary>
	/// バネ
	/// </summary>
	struct Spring {
		Vector3 anchor;
		float naturalLength;
		float stiffness;
		float dampingCoefficient;
	};
	/// <summary>
	/// ボール(物理)
	/// </summary>
	struct Ball {
		Vector3 position;
		Vector3 velocity;
		Vector3 acceleration;
		float mass;
		float radius;
		unsigned int color;
	};
	/// <summary>
	/// 2D矩形(物理)
	/// </summary>
	struct Box {
		Vector2 pos;			//位置
		Vector2 size;			//サイズ
		Vector2 velocity;		//速度
		Vector2 accleration;	//加速度
		float mass;				//質量
		unsigned int color;		//色
	};
	/// <summary>
	/// 振り子
	/// </summary>
	struct Pendulum {
		Vector3 anchor;
		float length;
		float angle;
		float angularVelocity;
		float angularAcceleration;
	};
	/// <summary>
	/// 円錐振り子
	/// </summary>
	struct ConicalPendulum {
		Vector3 anchor;
		float length;
		float halfApexAngle;
		float angle;
		float angularVelocity;
	};
	/// <summary>
	/// カプセル(トンネリング対策)
	/// </summary>
	struct Capsule {
		Segment segment;
		float radius;
	};

	/// <summary>
	/// 数学関数を管理するクラス
	/// </summary>
	class MyMath {
	public:
		///------------------------------------///
		///              Vector2
		///------------------------------------///

		/// <summary>
		/// 加算
		/// </summary>
		/// <param name="v1">ベクトル1</param>
		/// <param name="v2">ベクトル2</param>
		/// <returns>2つのベクトルの加算値</returns>
		static Vector2 Add(const Vector2& v1, const Vector2& v2);
		/// <summary>
		/// 減算
		/// </summary>
		/// <param name="v1">ベクトル1</param>
		/// <param name="v2">ベクトル2</param>
		/// <returns>2つのベクトルの減算値</returns>
		static Vector2 Subtract(const Vector2& v1, const Vector2& v2);
		/// <summary>
		/// 乗算
		/// </summary>
		/// <param name="s">スカラー</param>
		/// <param name="v">ベクトル</param>
		/// <returns>乗算値</returns>
		static Vector2 Multiply(float s, const Vector2& v);
		/// <summary>
		/// 外積
		/// </summary>
		/// <param name="a">ベクトル1</param>
		/// <param name="b">ベクトル2</param>
		/// <returns>外積</returns>
		static float Cross(const Vector2& a, const Vector2& b);
		/// <summary>
		/// 線形補完
		/// </summary>
		/// <param name="v1">ベクトル1(0)</param>
		/// <param name="v2">ベクトル2(1)</param>
		/// <param name="t">割合</param>
		/// <returns>線形補完された値</returns>
		static Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t);

		///------------------------------------///
		///              Vector3
		///------------------------------------///

		/// <summary>
		/// 加算
		/// </summary>
		/// <param name="v1">ベクトル1</param>
		/// <param name="v2">ベクトル2</param>
		/// <returns>2つのベクトルの加算値</returns>
		static Vector3 Add(const Vector3& v1, const Vector3& v2);
		/// <summary>
		/// 減算
		/// </summary>
		/// <param name="v1">ベクトル1</param>
		/// <param name="v2">ベクトル2</param>
		/// <returns>2つのベクトルの減算値</returns>
		static Vector3 Subtract(const Vector3& v1, const Vector3& v2);
		/// <summary>
		/// 乗算
		/// </summary>
		/// <param name="s">スカラー</param>
		/// <param name="v">ベクトル</param>
		/// <returns>乗算値</returns>
		static Vector3 Multiply(float s, const Vector3& v);
		/// <summary>
		/// 外積
		/// </summary>
		/// <param name="a">ベクトル1</param>
		/// <param name="b">ベクトル2</param>
		/// <returns>外積</returns>
		static Vector3 Cross(const Vector3& a, const Vector3& b);
		/// <summary>
		/// 正規化
		/// </summary>
		/// <param name="v">対象のベクトル</param>
		/// <returns>正規化された値</returns>
		static Vector3 Normalize(const Vector3& v);
		/// <summary>
		/// 線形補完
		/// </summary>
		/// <param name="v1">ベクトル1(0)</param>
		/// <param name="v2">ベクトル2(1)</param>
		/// <param name="t">割合</param>
		/// <returns>線形補完された値</returns>
		static Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
		/// <summary>
		/// 球面線形補完
		/// </summary>
		/// <param name="v1">ベクトル1(0)</param>
		/// <param name="v2">ベクトル2(1)</param>
		/// <param name="t">割合</param>
		/// <returns>球面線形補完された値</returns>
		static Vector3 Slerp(const Vector3& vector1, const Vector3& vector2, float t);
		/// <summary>
		/// 座標変換
		/// </summary>
		/// <param name="vector">変換したい座標</param>
		/// <param name="matrix">変換行列</param>
		/// <returns>変換後の座標</returns>
		static Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
		/// <summary>
		/// ローカル座標の座標変換
		/// </summary>
		/// <param name="vector">ローカル座標</param>
		/// <param name="matrix">変換行列</param>
		/// <returns>変換後の座標</returns>
		static Vector3 TransformNormal(const Vector3& vector, const Matrix4x4& matrix);
		/// <summary>
		/// 正射影ベクトル
		/// </summary>
		/// <param name="v1">ベクトル1</param>
		/// <param name="v2">ベクトル2</param>
		/// <returns>正射影ベクトル</returns>
		static Vector3 Project(const Vector3& v1, const Vector3& v2);
		/// <summary>
		/// 平面の法線から矩形を構成する4頂点をもとめる
		/// </summary>
		/// <param name="vector">法線</param>
		/// <returns>頂点の座標</returns>
		static Vector3 Perpendicular(const Vector3& vector);
		/// <summary>
		/// 平面と直線の衝突点をもとめる
		/// </summary>
		/// <param name="l">直線</param>
		/// <param name="p">平面</param>
		/// <returns>衝突点の座標</returns>
		static Vector3 CollisionPoint(const Line& l, const Plane& p);
		/// <summary>
		/// 平面と半直線の衝突点をもとめる
		/// </summary>
		/// <param name="r">半直線</param>
		/// <param name="p">平面</param>
		/// <returns>衝突点の座標</returns>
		static Vector3 CollisionPoint(const Ray& r, const Plane& p);
		/// <summary>
		/// 平面と線分の衝突点をもとめる
		/// </summary>
		/// <param name="s">線分</param>
		/// <param name="p">平面</param>
		/// <returns>衝突点の座標</returns>
		static Vector3 CollisionPoint(const Segment& s, const Plane& p);
		/// <summary>
		/// 反射ベクトルをもとめる
		/// </summary>
		/// <param name="input">入射ベクトル</param>
		/// <param name="normal">法線</param>
		/// <returns>反射ベクトル</returns>
		static Vector3 Reflect(const Vector3& input, const Vector3& normal);
		/// <summary>
		/// 2つのベクトルのなす角を求める関数
		/// </summary>
		/// <param name="v1">ベクトル1</param>
		/// <param name="v2">ベクトル2</param>
		/// <returns>なす角</returns>
		static float AngleOf2VectorY(const Vector3& v1, const Vector3& v2);
		/// <summary>
		/// 向きベクトルから回転を求める関数(Z回転は考慮しない)
		/// 範囲は(-90°~90°)
		/// </summary>
		/// <param name="dir">向きベクトル</param>
		/// <returns>回転</returns>
		static Vector3 DirectionToRotation(const Vector3& dir);
		/// <summary>
		/// 回転から向きベクトルを求める関数(Z回転は考慮しない)
		/// </summary>
		/// <param name="rot">回転</param>
		/// <returns>向きベクトル</returns>
		static Vector3 RotationToDirection(const Vector3& rot);

		///------------------------------------///
		///            Vector4
		///------------------------------------///

		/// <summary>
		/// 線形補完
		/// </summary>
		/// <param name="v1">ベクトル1(0)</param>
		/// <param name="v2">ベクトル2(1)</param>
		/// <param name="t">割合</param>
		/// <returns>線形補完した値</returns>
		static Vector4 Lerp(const Vector4& v1, const Vector4& v2, float t);

		///------------------------------------///
		///              Matrix4x4
		///------------------------------------///

		/// <summary>
		/// 加算
		/// </summary>
		/// <param name="m1">行列1</param>
		/// <param name="m2">行列2</param>
		/// <returns>加算値</returns>
		static Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);
		/// <summary>
		/// 減算
		/// </summary>
		/// <param name="m1">行列1</param>
		/// <param name="m2">行列2</param>
		/// <returns>減算値</returns>
		static Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);
		/// <summary>
		/// 乗算
		/// </summary>
		/// <param name="m1">行列1</param>
		/// <param name="m2">行列2</param>
		/// <returns>乗算値</returns>
		static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
		/// <summary>
		/// 逆行列を求める
		/// </summary>
		/// <param name="m">対象の行列</param>
		/// <returns>対象の逆行列</returns>
		static Matrix4x4 Inverse(const Matrix4x4& m);
		/// <summary>
		/// 行列を転置する
		/// </summary>
		/// <param name="m">対象の行列</param>
		/// <returns>対象の転置行列</returns>
		static Matrix4x4 Transpose(const Matrix4x4& m);
		/// <summary>
		/// 単位行列を作成する
		/// </summary>
		/// <returns>単位行列</returns>
		static Matrix4x4 MakeIdentity4x4();
		/// <summary>
		/// 平行移動行列を作成する
		/// </summary>
		/// <param name="translate">平行移動量ベクトル</param>
		/// <returns>平行移動行列</returns>
		static Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
		/// <summary>
		/// スケーリング行列を作成する
		/// </summary>
		/// <param name="scale">スケール値ベクトル</param>
		/// <returns>スケーリング行列</returns>
		static Matrix4x4 MakeScaleMatrix(const Vector3& scale);
		/// <summary>
		/// オイラー角を用いた回転行列を作成する
		/// </summary>
		/// <param name="rotate">回転角（ラジアン単位）</param>
		/// <returns>回転行列</returns>
		static Matrix4x4 MakeRotateMatrix(const Vector3& rotate);
		/// <summary>
		/// X軸回転行列を作成する
		/// </summary>
		/// <param name="radian">回転角度（ラジアン）</param>
		/// <returns>X軸回転行列</returns>
		static Matrix4x4 MakeRotateXMatrix(float radian);
		/// <summary>
		/// Y軸回転行列を作成する
		/// </summary>
		/// <param name="radian">回転角度（ラジアン）</param>
		/// <returns>Y軸回転行列</returns>
		static Matrix4x4 MakeRotateYMatrix(float radian);
		/// <summary>
		/// Z軸回転行列を作成する
		/// </summary>
		/// <param name="radian">回転角度（ラジアン）</param>
		/// <returns>Z軸回転行列</returns>
		static Matrix4x4 MakeRotateZMatrix(float radian);
		/// <summary>
		/// オイラー角を用いたアフィン変換行列を作成する
		/// </summary>
		/// <param name="scale">スケール</param>
		/// <param name="rotate">回転（ラジアン単位）</param>
		/// <param name="translate">平行移動</param>
		/// <returns>アフィン変換行列</returns>
		static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
		/// <summary>
		/// クォータニオンを用いたアフィン変換行列を作成する
		/// </summary>
		/// <param name="scale">スケール</param>
		/// <param name="rotate">回転クォータニオン</param>
		/// <param name="translate">平行移動</param>
		/// <returns>アフィン変換行列</returns>
		static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);
		/// <summary>
		/// 射影変換行列（透視投影）を作成する
		/// </summary>
		/// <param name="fovY">Y方向の視野角（ラジアン）</param>
		/// <param name="aspectRatio">アスペクト比（横/縦）</param>
		/// <param name="nearClip">ニアクリップ面の距離</param>
		/// <param name="farClip">ファークリップ面の距離</param>
		/// <returns>透視射影行列</returns>
		static Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
		/// <summary>
		/// 平行投影行列を作成する
		/// </summary>
		/// <param name="left">左端の座標</param>
		/// <param name="top">上端の座標</param>
		/// <param name="right">右端の座標</param>
		/// <param name="bottom">下端の座標</param>
		/// <param name="nearClip">ニアクリップ面の距離</param>
		/// <param name="farClip">ファークリップ面の距離</param>
		/// <returns>平行投影行列</returns>
		static Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
		/// <summary>
		/// ビューポート変換行列を作成する
		/// </summary>
		/// <param name="left">ビューポート左位置</param>
		/// <param name="top">ビューポート上位置</param>
		/// <param name="width">ビューポートの幅</param>
		/// <param name="height">ビューポートの高さ</param>
		/// <param name="minDepth">最小深度値</param>
		/// <param name="maxDepth">最大深度値</param>
		/// <returns>ビューポート行列</returns>
		static Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
		/// <summary>
		/// オイラー角から回転行列を作成する
		/// </summary>
		/// <param name="rotate">回転（オイラー）</param>
		/// <returns>回転行列</returns>
		static Matrix4x4 CreateRotationFromEulerAngles(const Vector3& rotate);


		///------------------------------------///
		///           Quaternion
		///------------------------------------///

		/// <summary>
		/// 四元数の加算
		/// </summary>
		/// <param name="q1">加算元の四元数</param>
		/// <param name="q2">加算する四元数</param>
		/// <returns>加算結果の四元数</returns>
		static Quaternion Add(const Quaternion& q1, const Quaternion& q2);
		/// <summary>
		/// 四元数の減算
		/// </summary>
		/// <param name="q1">減算元の四元数</param>
		/// <param name="q2">減算する四元数</param>
		/// <returns>減算結果の四元数</returns>
		static Quaternion Subtract(const Quaternion& q1, const Quaternion& q2);
		/// <summary>
		/// 四元数の乗算（合成回転）
		/// </summary>
		/// <param name="q1">左項の四元数</param>
		/// <param name="q2">右項の四元数</param>
		/// <returns>乗算結果の四元数（q1 の回転の後に q2 の回転を適用）</returns>
		static Quaternion Multiply(const Quaternion& q1, const Quaternion& q2);
		/// <summary>
		/// 四元数とスカラーの乗算
		/// </summary>
		/// <param name="scalar">乗算するスカラー値</param>
		/// <param name="q">対象の四元数</param>
		/// <returns>スカラー倍された四元数</returns>
		static Quaternion Multiply(float scalar, const Quaternion& q);
		/// <summary>
		/// 四元数の内積
		/// </summary>
		/// <param name="q1">一方の四元数</param>
		/// <param name="q2">もう一方の四元数</param>
		/// <returns>内積値（類似度の指標）</returns>
		static float Dot(const Quaternion& q1, const Quaternion& q2);
		/// <summary>
		/// 四元数のノルム（大きさ）
		/// </summary>
		/// <param name="q">対象の四元数</param>
		/// <returns>ノルム（スカラー値）</returns>
		static float Norm(const Quaternion& q);
		/// <summary>
		/// 四元数の正規化
		/// </summary>
		/// <param name="q">対象の四元数</param>
		/// <returns>正規化された単位四元数</returns>
		static Quaternion Normalize(const Quaternion& q);
		/// <summary>
		/// 四元数の共役を求める
		/// </summary>
		/// <param name="q">対象の四元数</param>
		/// <returns>共役四元数</returns>
		static Quaternion Conjugate(const Quaternion& q);
		/// <summary>
		/// 四元数の逆元を求める
		/// </summary>
		/// <param name="q">対象の四元数</param>
		/// <returns>逆四元数（共役／ノルムを用いた逆）</returns>
		static Quaternion Inverse(const Quaternion& q);
		/// <summary>
		/// 回転軸と角度から四元数を生成
		/// </summary>
		/// <param name="axis">回転軸ベクトル</param>
		/// <param name="angle">回転角（ラジアン）</param>
		/// <returns>生成された回転四元数</returns>
		static Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);
		/// <summary>
		/// ベクトルを四元数で回転させた結果を求める
		/// </summary>
		/// <param name="vector">回転させるベクトル</param>
		/// <param name="quaternion">回転を表す四元数</param>
		/// <returns>回転後のベクトル</returns>
		static Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);
		/// <summary>
		/// 四元数から回転行列を生成
		/// </summary>
		/// <param name="q">対象の四元数</param>
		/// <returns>対応する回転行列</returns>
		static Matrix4x4 MakeRotateMatrix(const Quaternion& q);
		/// <summary>
		/// オイラー角から四元数を生成
		/// </summary>
		/// <param name="euler">オイラー角（ピッチ、ヨー、ロール）</param>
		/// <returns>生成された四元数</returns>
		static Quaternion FromEulerAngles(Vector3 euler);
		/// <summary>
		/// 四元数をオイラー角に変換
		/// </summary>
		/// <param name="q">対象の四元数</param>
		/// <returns>オイラー角（ピッチ、ヨー、ロール）</returns>
		static Vector3 ToEulerAngles(const Quaternion& q);
		/// <summary>
		/// 四元数の球面線形補間（Slerp）
		/// </summary>
		/// <param name="q0">始点の四元数</param>
		/// <param name="q1">終点の四元数</param>
		/// <param name="t">補間係数（0～1）</param>
		/// <returns>補間結果の四元数</returns>
		static Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

		///------------------------------------///
		///              float
		///------------------------------------///

		/// <summary>
		/// 角度（ラジアン）から余接（cot）を求める
		/// </summary>
		/// <param name="rad">角度（ラジアン）</param>
		/// <returns>余接（cot(rad)）の値</returns>
		static float Cot(float rad);
		/// <summary>
		/// 3次元ベクトルの長さ（距離）を求める
		/// </summary>
		/// <param name="v">対象のベクトル</param>
		/// <returns>ベクトルの長さ（ノルム）</returns>
		static float Length(const Vector3& v);
		/// <summary>
		/// 2次元ベクトルの内積を求める
		/// </summary>
		/// <param name="v1">一方のベクトル</param>
		/// <param name="v2">もう一方のベクトル</param>
		/// <returns>内積値</returns>
		static float Dot(const Vector2& v1, const Vector2& v2);
		/// <summary>
		/// 3次元ベクトルの内積を求める
		/// </summary>
		/// <param name="v1">一方のベクトル</param>
		/// <param name="v2">もう一方のベクトル</param>
		/// <returns>内積値</returns>
		static float Dot(const Vector3& v1, const Vector3& v2);
		/// <summary>
		/// 2つのスカラー値の線形補間を行う
		/// </summary>
		/// <param name="s1">始点のスカラー値</param>
		/// <param name="s2">終点のスカラー値</param>
		/// <param name="t">補間係数（0～1）</param>
		/// <returns>線形補間後の値</returns>
		static float Lerp(float s1, float s2, float t);
		/// <summary>
		/// 角度を-π~πの間に収める
		/// </summary>
		/// <param name="angle"></param>
		/// <returns></returns>
		static float NormalizeAngle(float angle);


		///------------------------------------///
		///              Transform
		///------------------------------------///

		/// <summary>
		/// トランスフォームの合成(ワールドトランスフォーム)
		/// </summary>
		/// <param name="parent">親トランスフォーム</param>
		/// <param name="child">ローカルトランスフォーム</param>
		/// <returns>合成結果</returns>
		static TransformEuler Combine(const TransformEuler& parent, const TransformEuler& local);

		///------------------------------------///
		///            補助関数
		///------------------------------------///

		/// <summary>
		/// 複数の頂点を指定した軸ベクトルに射影し、射影された範囲（最小値と最大値）を計算する
		/// </summary>
		/// <param name="vertices">射影対象となる頂点配列の先頭ポインタ</param>
		/// <param name="count">頂点配列の要素数</param>
		/// <param name="axis">射影方向となる軸ベクトル（正規化されていることが望ましい）</param>
		/// <returns>
		/// 射影結果の最小値と最大値を表すペア（first = 最小値、second = 最大値）
		/// </returns>
		static std::pair<float, float> ProjectOntoAxis(const Vector3* vertices, int count, const Vector3& axis);

		///------------------------------------///
		///				イージング
		///------------------------------------///

		/// <summary>イージング：Sine（正弦）によるEaseIn</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInSine(float ratio);
		/// <summary>イージング：Sine（正弦）によるEaseOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseOutSine(float ratio);
		/// <summary>イージング：Sine（正弦）によるEaseInOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInOutSine(float ratio);
		/// <summary>イージング：Cubic（三次曲線）によるEaseIn</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInCubic(float ratio);
		/// <summary>イージング：Cubic（三次曲線）によるEaseOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseOutCubic(float ratio);
		/// <summary>イージング：Cubic（三次曲線）によるEaseInOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInOutCubic(float ratio);
		/// <summary>イージング：Quint（五次曲線）によるEaseIn</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInQuint(float ratio);
		/// <summary>イージング：Quint（五次曲線）によるEaseOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseOutQuint(float ratio);
		/// <summary>イージング：Quint（五次曲線）によるEaseInOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInOutQuint(float ratio);
		/// <summary>イージング：Circ（円弧）によるEaseIn</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInCirc(float ratio);
		/// <summary>イージング：Circ（円弧）によるEaseOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseOutCirc(float ratio);
		/// <summary>イージング：Circ（円弧）によるEaseInOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInOutCirc(float ratio);
		/// <summary>イージング：Elastic（弾性）によるEaseIn</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInElastic(float ratio);
		/// <summary>イージング：Elastic（弾性）によるEaseOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseOutElastic(float ratio);
		/// <summary>イージング：Elastic（弾性）によるEaseInOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInOutElastic(float ratio);
		/// <summary>イージング：Quad（二次曲線）によるEaseIn</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInQuad(float ratio);
		/// <summary>イージング：Quad（二次曲線）によるEaseOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseOutQuad(float ratio);
		/// <summary>イージング：Quad（二次曲線）によるEaseInOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInOutQuad(float ratio);
		/// <summary>イージング：Quart（四次曲線）によるEaseIn</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInQuart(float ratio);
		/// <summary>イージング：Quart（四次曲線）によるEaseOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseOutQuart(float ratio);
		/// <summary>イージング：Quart（四次曲線）によるEaseInOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInOutQuart(float ratio);
		/// <summary>イージング：Expo（指数関数）によるEaseIn</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInExpo(float ratio);
		/// <summary>イージング：Expo（指数関数）によるEaseOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseOutExpo(float ratio);
		/// <summary>イージング：Expo（指数関数）によるEaseInOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInOutExpo(float ratio);
		/// <summary>イージング：Back（オーバーシュート）によるEaseIn</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInBack(float ratio);
		/// <summary>イージング：Back（オーバーシュート）によるEaseOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseOutBack(float ratio);
		/// <summary>イージング：Back（オーバーシュート）によるEaseInOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInOutBack(float ratio);
		/// <summary>イージング：Bounce（バウンド）によるEaseIn</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInBounce(float ratio);
		/// <summary>イージング：Bounce（バウンド）によるEaseOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseOutBounce(float ratio);
		/// <summary>イージング：Bounce（バウンド）によるEaseInOut</summary>
		/// <param name="ratio">進行度（0～1）</param>
		/// <returns>補間後の値</returns>
		static float EaseInOutBounce(float ratio);

		///------------------------------------///
		///              当たり判定
		///------------------------------------///

		//球同士の当たり判定
		static bool IsCollision(const Sphere& s1, const Sphere& s2);
		//平面と球の当たり判定
		static bool IsCollision(const Plane& plane, const Sphere& sphere);
		static bool IsCollision(const Sphere& sphere, const Plane& plane);
		//カプセルと球の当たり判定
		static bool IsCollision(const Capsule& capsule, const Sphere& sphere);
		static bool IsCollision(const Sphere& sphere, const Capsule& capsule);
		//直線と平面の当たり判定
		static bool IsCollision(const Line& line, const Plane& plane);
		static bool IsCollision(const Plane& plane, const Line& line);
		//半直線と平面の当たり判定
		static bool IsCollision(const Ray& ray, const Plane& plane);
		static bool IsCollision(const Plane& plane, const Ray& ray);
		//線分と平面の当たり判定
		static bool IsCollision(const Segment& segment, const Plane& plane);
		static bool IsCollision(const Plane& plane, const Segment& segment);
		//カプセルと平面の当たり判定
		static bool IsCollision(const Capsule& capsule, const Plane& plane);
		static bool IsCollision(const Plane& plane, const Capsule& capsule);
		//カプセルと三角形の辺り判定
		static bool IsCollision(const Capsule& capsule, const Triangle& tri);
		static bool IsCollision(const Triangle& tri, const Capsule& capsule);
		//線分と三角形の当たり判定
		static bool IsCollision(const Segment& segment, const Triangle& triangle);
		static bool IsCollision(const Triangle& triangle, const Segment& segment);
		//AABB同士の当たり判定
		static bool IsCollision(const AABB& a, const AABB& b);
		//AABBと点の当たり判定
		static bool IsCollision(const AABB& aabb, const Vector3& point);
		static bool IsCollision(const Vector3& point, const AABB& aabb);
		//AABBと球の当たり判定
		static bool IsCollision(const AABB& aabb, const Sphere& sphere);
		static bool IsCollision(const Sphere& sphere, const AABB& aabb);
		//AABBと直線の当たり判定
		static bool IsCollision(const AABB& aabb, const Line& line);
		static bool IsCollision(const Line& line, const AABB& aabb);
		//AABBと半直線の当たり判定
		static bool IsCollision(const AABB& aabb, const Ray& ray);
		static bool IsCollision(const Ray& ray, const AABB& aabb);
		//AABBと線分の当たり判定
		static bool IsCollision(const AABB& aabb, const Segment& segment);
		static bool IsCollision(const Segment& segment, const AABB& aabb);
		//AABBとOBBの当たり判定
		static bool IsCollision(const AABB& aabb, const OBB& obb);
		static bool IsCollision(const OBB& obb, const AABB& aabb);
		//AABBとカプセルの当たり判定
		static bool IsCollision(const AABB& aabb, const Capsule& capsule);
		static bool IsCollision(const Capsule& capsule, const AABB& aabb);
		//OBB同士の当たり判定
		static bool IsCollision(const OBB& obb1, const OBB& obb2);
		//OBBと球の当たり判定
		static bool IsCollision(const OBB& obb, const Sphere& sphere);
		static bool IsCollision(const Sphere& sphere, const OBB& obb);
		//OBBと直線の当たり判定
		static bool IsCollision(const OBB& obb, const Line& line);
		static bool IsCollision(const Line& line, const OBB& obb);
		//OBBと半直線の当たり判定
		static bool IsCollision(const OBB& obb, const Ray& ray);
		static bool IsCollision(const Ray& ray, const OBB& obb);
		//OBBと線分の当たり判定
		static bool IsCollision(const OBB& obb, const Segment& segment);
		static bool IsCollision(const Segment& segment, const OBB& obb);
		//OBBとカプセルの当たり判定
		static bool IsCollision(const OBB& obb, const Capsule& capsule);
		static bool IsCollision(const Capsule& capsule, const OBB& obb);
		//カプセル同士の当たり判定
		static bool IsCollision(const Capsule& capsule1, const Capsule& capsule2);

		///------------------------------------///
		///             図形の線描画
		///------------------------------------///

		/// <summary>
		/// 球体の線描画を作成する
		/// </summary>
		/// <param name="sphere">球体</param>
		/// <param name="color">線の色</param>
		/// <param name="subdivision">分割数</param>
		static void CreateLineSphere(const Sphere& sphere, Vector4 color, uint32_t subdivision = 15);
		/// <summary>
		/// AABBの線描画を作成する
		/// </summary>
		/// <param name="aabb">AABB</param>
		/// <param name="color">線の色</param>
		static void CreateLineAABB(const AABB& aabb, Vector4 color);
		/// <summary>
		/// OBBの線描画を作成する
		/// </summary>
		/// <param name="obb">OBB</param>
		/// <param name="color">線の色</param>
		static void CreateLineOBB(const OBB& obb, Vector4 color);

	private:
		///------------------------------------///
		///          補助関数
		///------------------------------------///

		/// <summary>
		/// 点と平面の最短距離を求める
		/// </summary>
		/// <param name="_point">点</param>
		/// <param name="_plane">平面</param>
		/// <returns>点と平面の最短距離</returns>
		static float DistancePointToPlane(const Vector3& point, const Plane& plane);
		/// <summary>
		/// 点と線分の最短距離を求める
		/// </summary>
		/// <param name="point">点</param>
		/// <param name="segment">線分</param>
		/// <returns>点と線分の最短距離</returns>
		static float DistancePointToSegment(const Vector3& point, const Segment& segment);
		/// <summary>
		/// 線分同士の最短距離を求める
		/// </summary>
		/// <param name="s1">線分1</param>
		/// <param name="s2">線分2</param>
		/// <returns>線分同士の最短距離</returns>
		static float DistanceSegmentToSegment(const Segment& s1, const Segment& s2);
		/// <summary>
		/// 点と平面の最近接点を求める
		/// </summary>
		/// <param name="point">点</param>
		/// <param name="plane">平面</param>
		/// <returns>最近接点</returns>
		static Vector3 ClosestPoint(const Vector3& point, const Plane& plane);
		/// <summary>
		/// 点と三角形の最近接点を求める
		/// </summary>
		/// <param name="point">点</param>
		/// <param name="tri">三角形</param>
		/// <returns>最近接点</returns>
		static Vector3 ClosestPoint(const Vector3& point, const Triangle& tri);
		/// <summary>
		/// 点と線分の最近接点を求める
		/// </summary>
		/// <param name="point">座標</param>
		/// <param name="segment">対象の線分</param>
		/// <returns>最近接点</returns>
		static Vector3 ClosestPoint(const Vector3& point, const Segment& segment);
		/// <summary>
		/// 点とAABBの最近接点をもとめる
		/// </summary>
		/// <param name="point">座標</param>
		/// <param name="aabb">AABB</param>
		/// <returns>最近接点の座標</returns>
		static Vector3 ClosestPoint(const Vector3& point, const AABB& aabb);
		/// <summary>
		/// 三角形を含んでいる平面を作る
		/// </summary>
		/// <param name="tri">三角形</param>
		/// <returns>三角形を含んでいる平面</returns>
		static Plane MakePlane(const Triangle& tri);
		/// <summary>
		/// 平行の判定
		/// </summary>
		/// <param name="segment">線分</param>
		/// <param name="plane">平面</param>
		/// <returns>判定結果</returns>
		static bool CheckParallel(const Segment& segment, const Plane& plane);
		/// <summary>
		/// 線分を平面に射影したときにできる線分を求める
		/// </summary>
		/// <param name="segment">線分</param>
		/// <param name="plane">平面</param>
		/// <returns>求まった線分</returns>
		static Segment ProjectSegmentOntoPlane(const Segment& segment, const Plane& plane);
		/// <summary>
		/// 点が三角形内部にあるかの判定
		/// </summary>
		/// <param name="point">点</param>
		/// <param name="tri">三角形</param>
		/// <returns>内部にあるか</returns>
		static bool IsPointInTriangle(const Vector3& point, const Triangle& tri);
		/// <summary>
		/// 線分同士の交点を求める(最短距離がEpsolon未満である線分同士に限る)
		/// </summary>
		/// <param name="s1">線分1</param>
		/// <param name="s2">線分2</param>
		/// <returns>交点の座標</returns>
		static Vector3 IntersectionSegmentToSegment(const Segment& s1, const Segment& s2);


	};

	///------------------------------------///
	///      演算子のオーバーロード
	///------------------------------------///

	//二項演算子
	Vector2 operator+(const Vector2& v1, const Vector2& v2);
	Vector2 operator-(const Vector2& v1, const Vector2& v2);
	Vector2 operator*(float s, const Vector2& v);
	Vector2 operator*(const Vector2& v, float s);
	Vector2 operator/(const Vector2& v, float s);
	Vector3 operator+(const Vector3& v1, const Vector3& v2);
	Vector3 operator-(const Vector3& v1, const Vector3& v2);
	Vector3 operator*(float s, const Vector3& v);
	Vector3 operator*(const Vector3& v, float s);
	Vector3 operator/(const Vector3& v, float s);
	Vector3 operator*(const Matrix4x4& mat, const Vector3& vec);
	Vector3 operator*(const Quaternion& q, const Vector3& v);
	Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2);
	Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2);
	Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);
	Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
	Quaternion operator-(const Quaternion& q1, const Quaternion& q2);
	Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
	Quaternion operator*(float s, const Quaternion& q);
	Quaternion operator*(const Quaternion& q, float s);

	//単項演算子
	Vector3 operator-(const Vector3& v);
	Vector3 operator+(const Vector3& v);
	Quaternion operator-(const Quaternion& q);
	Quaternion operator+(const Quaternion& q);



}