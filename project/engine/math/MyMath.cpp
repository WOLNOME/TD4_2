#include "MyMath.h"
#include <cassert>
#include <cmath>
#include <algorithm>
#include <numbers>
#include "LineManager.h"

#undef min
#undef max

namespace Norm {


	Vector2 MyMath::Add(const Vector2& v1, const Vector2& v2) {
		Vector2 c;
		c = {
			v1.x + v2.x,
			v1.y + v2.y
		};
		return c;
	}

	Vector2 MyMath::Subtract(const Vector2& v1, const Vector2& v2) {
		Vector2 c;
		c = {
			v1.x - v2.x,
			v1.y - v2.y
		};
		return c;
	}

	Vector2 MyMath::Multiply(float s, const Vector2& v) {
		return Vector2(s * v.x, s * v.y);
	}

	float MyMath::Cross(const Vector2& a, const Vector2& b) {
		float c;

		c = (a.x * b.y) - (a.y * b.x);

		return c;
	}

	Vector2 MyMath::Lerp(const Vector2& v1, const Vector2& v2, float t) {
		Vector2 result;
		result.x = Lerp(v1.x, v2.x, t);
		result.y = Lerp(v1.y, v2.y, t);
		return result;
	}

	Vector3 MyMath::Add(const Vector3& v1, const Vector3& v2) {
		Vector3 c;
		c = {
			v1.x + v2.x,
			v1.y + v2.y,
			v1.z + v2.z
		};
		return c;
	}

	Vector3 MyMath::Subtract(const Vector3& v1, const Vector3& v2) {
		Vector3 c;
		c = {
			v1.x - v2.x,
			v1.y - v2.y,
			v1.z - v2.z
		};
		return c;
	}

	Vector3 MyMath::Multiply(float s, const Vector3& v) {
		return Vector3(s * v.x, s * v.y, s * v.z);
	}

	Vector3 MyMath::Cross(const Vector3& a, const Vector3& b) {
		Vector3 c;

		c.x = (a.y * b.z) - (a.z * b.y);
		c.y = (a.z * b.x) - (a.x * b.z);
		c.z = (a.x * b.y) - (a.y * b.x);

		return c;
	}

	Vector3 MyMath::Normalize(const Vector3& v) {
		Vector3 c;
		//長さを求める
		float length = Length(v);
		//length=0で無ければ正規化
		if (length != 0) {
			c.x = v.x / length;
			c.y = v.y / length;
			c.z = v.z / length;
		}
		else {
			assert("正規化できません");
		}
		return c;
	}

	Vector3 MyMath::Lerp(const Vector3& v1, const Vector3& v2, float t) {
		Vector3 result;
		result.x = Lerp(v1.x, v2.x, t);
		result.y = Lerp(v1.y, v2.y, t);
		result.z = Lerp(v1.z, v2.z, t);
		return result;
	}

	Vector3 MyMath::Slerp(const Vector3& vector1, const Vector3& vector2, float t) {
		//正規化ベクトルを求める
		Vector3 start = Normalize(vector1);
		Vector3 end = Normalize(vector2);

		//内積を求める
		float dot = Dot(start, end);
		//誤差により1.0fを超えるのを防ぐ
		dot = std::clamp(dot, dot, 1.0f);

		//s－九コサインでθの角度を求める
		float theta = std::acosf(dot);

		//θの角度からsinθを求める
		float sinTheta = std::sin(theta);

		//サイン(θ(1-t))を求める
		float sinThetaFrom = std::sin((1 - t) * theta);
		//サインθtを求める
		float sinThetaTo = std::sin(t * theta);

		Vector3 normalizeVector;
		//ゼロ除算を防ぐ
		if (sinTheta < 1.0e-5) {
			normalizeVector = start;
		}
		else {
			//球面線形補間したベクトル(単位ベクトル)
			normalizeVector = Add(Multiply(sinThetaFrom / sinTheta, start), Multiply(sinThetaTo / sinTheta, end));
		}

		//ベクトルの長さはstartとendの長さを線形補間
		float length1 = Length(start);
		float length2 = Length(end);
		//Lerpで補間ベクトルの長さを求める
		float length = Lerp(length1, length2, t);

		//長さを反映
		return Multiply(length, normalizeVector);
	}

	Vector3 MyMath::Transform(const Vector3& vector, const Matrix4x4& matrix) {
		Vector3 result;
		result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
		result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
		result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
		float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
		assert(w != 0.0f);
		result.x /= w;
		result.y /= w;
		result.z /= w;
		return result;
	}

	Vector3 MyMath::TransformNormal(const Vector3& vector, const Matrix4x4& matrix) {
		Vector3 result;

		result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0];
		result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1];
		result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2];

		return result;
	}

	Vector3 MyMath::Project(const Vector3& v1, const Vector3& v2) {
		Vector3 c;
		float n = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
		float l = powf(sqrtf(powf(v2.x, 2) + powf(v2.y, 2) + powf(v2.z, 2)), 2);
		c.x = n / l * v2.x;
		c.y = n / l * v2.y;
		c.z = n / l * v2.z;
		return c;
	}

	Vector3 MyMath::Perpendicular(const Vector3& vector) {
		if (vector.x != 0.0f || vector.y != 0.0f) {
			return { -vector.y,vector.x,0.0f };
		}
		//法線がz成分のみなら
		return{ 0.0f,-vector.z,vector.y };
	}

	Vector3 MyMath::CollisionPoint(const Line& l, const Plane& p) {
		///衝突している前提の関数
		Vector3 result;
		float dot = Dot(l.diff, p.normal);
		//媒介変数
		float t = (p.distance - Dot(l.origin, p.normal)) / dot;
		result = Add(l.origin, Multiply(t, l.diff));
		return result;
	}

	Vector3 MyMath::CollisionPoint(const Ray& r, const Plane& p) {
		///衝突している前提の関数
		Vector3 result;
		float dot = Dot(r.diff, p.normal);
		//媒介変数
		float t = (p.distance - Dot(r.origin, p.normal)) / dot;
		result = Add(r.origin, Multiply(t, r.diff));
		return result;
	}

	Vector3 MyMath::CollisionPoint(const Segment& s, const Plane& p) {
		///衝突している前提の関数
		Vector3 result;
		float dot = Dot(s.diff, p.normal);
		//媒介変数
		float t = (p.distance - Dot(s.origin, p.normal)) / dot;
		result = Add(s.origin, Multiply(t, s.diff));
		return result;
	}

	Vector3 MyMath::Reflect(const Vector3& input, const Vector3& normal) {
		Vector3 r;
		r = input - 2 * (Dot(input, normal)) * normal;
		return r;
	}

	float MyMath::AngleOf2VectorY(const Vector3& v1, const Vector3& v2) {
		//Y軸回転のため、引数のベクトルをxz成分に分解(長さの計算する手間省くため正規化)
		Vector2 longHand = Vector2(v1.x, v1.z).Normalize();
		Vector2 hourHand = Vector2(v2.x, v2.z).Normalize();
		//内積とベクトル長さを使ってcosθを求める
		float cos_theta = MyMath::Dot(longHand, hourHand);
		//cosθからθを求める
		float theta = std::acos(cos_theta);
		//2ベクトルの外積を求め、hourHandが左にあるならマイナス
		if (MyMath::Cross(longHand, hourHand) > 0.0f) {
			theta = -theta;
		}
		//cosθの値で場合分け(NAN回避処理)
		if (cos_theta >= 1.0f) {
			theta = 0.0f;
		}
		else if (cos_theta <= -1.0f) {
			theta = std::numbers::pi_v<float>;
		}
		return theta;
	}

	Vector3 MyMath::DirectionToRotation(const Vector3& dir) {
		// 正規化（必須）
		Vector3 n = dir.Normalized();

		// --- Pitch（上下） ---
		// forward.y から角度を求める：-asin(y)
		float pitch = -std::asin(n.y);

		// --- Yaw（左右） ---
		// x, z を使って atan2
		float yaw = std::atan2(n.x, n.z);

		// Rollは考慮しないので0
		float roll = 0.0f;

		return { pitch, yaw, roll };
	}

	Vector3 MyMath::RotationToDirection(const Vector3& rot) {
		// 各軸の回転角度のsin, cosを求める
		float sinPitch = std::sin(rot.x);
		float cosPitch = std::cos(rot.x);
		float sinYaw = std::sin(rot.y);
		float cosYaw = std::cos(rot.y);
		// 回転行列を使って方向ベクトルを求める
		Vector3 dir;
		dir.x = cosPitch * sinYaw;          // x成分
		dir.y = -sinPitch;                  // y成分
		dir.z = cosPitch * cosYaw;          // z成分
		return dir.Normalized();
	}

	Vector4 MyMath::Lerp(const Vector4& v1, const Vector4& v2, float t) {
		Vector4 result;
		result.x = Lerp(v1.x, v2.x, t);
		result.y = Lerp(v1.y, v2.y, t);
		result.z = Lerp(v1.z, v2.z, t);
		result.w = Lerp(v1.w, v2.w, t);
		return result;
	}

	Matrix4x4 MyMath::Add(const Matrix4x4& m1, const Matrix4x4& m2) {
		Matrix4x4 c{};
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				c.m[i][j] = m1.m[i][j] + m2.m[i][j];
			}
		}
		return c;
	}

	Matrix4x4 MyMath::Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
		Matrix4x4 c{};
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				c.m[i][j] = m1.m[i][j] - m2.m[i][j];
			}
		}
		return c;
	}

	Matrix4x4 MyMath::Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
		Matrix4x4 result{};
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] =
					m1.m[i][0] * m2.m[0][j] +
					m1.m[i][1] * m2.m[1][j] +
					m1.m[i][2] * m2.m[2][j] +
					m1.m[i][3] * m2.m[3][j];
			}
		}
		return result;
	}

	Matrix4x4 MyMath::Inverse(const Matrix4x4& m) {
		Matrix4x4 c;
		float A;
		A = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] -
			m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] -
			m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] +
			m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +
			m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] -
			m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] -
			m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] +
			m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

		c.m[0][0] = (1.0f / A) * (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]);
		c.m[0][1] = (1.0f / A) * (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]);
		c.m[0][2] = (1.0f / A) * (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]);
		c.m[0][3] = (1.0f / A) * (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]);
		c.m[1][0] = (1.0f / A) * (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]);
		c.m[1][1] = (1.0f / A) * (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]);
		c.m[1][2] = (1.0f / A) * (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]);
		c.m[1][3] = (1.0f / A) * (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]);
		c.m[2][0] = (1.0f / A) * (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]);
		c.m[2][1] = (1.0f / A) * (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]);
		c.m[2][2] = (1.0f / A) * (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]);
		c.m[2][3] = (1.0f / A) * (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]);
		c.m[3][0] = (1.0f / A) * (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]);
		c.m[3][1] = (1.0f / A) * (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]);
		c.m[3][2] = (1.0f / A) * (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]);
		c.m[3][3] = (1.0f / A) * (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]);

		return c;
	}

	Matrix4x4 MyMath::Transpose(const Matrix4x4& m) {
		Matrix4x4 c;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				c.m[i][j] = m.m[j][i];
			}
		}
		return c;
	}

	Matrix4x4 MyMath::MakeIdentity4x4() {
		Matrix4x4 c = {};
		for (int i = 0; i < 4; ++i) {
			c.m[i][i] = 1.0f;
		}
		return c;
	}

	Matrix4x4 MyMath::MakeTranslateMatrix(const Vector3& translate) {
		Matrix4x4 c;
		c.m[0][0] = 1;
		c.m[0][1] = 0;
		c.m[0][2] = 0;
		c.m[0][3] = 0;
		c.m[1][0] = 0;
		c.m[1][1] = 1;
		c.m[1][2] = 0;
		c.m[1][3] = 0;
		c.m[2][0] = 0;
		c.m[2][1] = 0;
		c.m[2][2] = 1;
		c.m[2][3] = 0;
		c.m[3][0] = translate.x;
		c.m[3][1] = translate.y;
		c.m[3][2] = translate.z;
		c.m[3][3] = 1;
		return c;
	}

	Matrix4x4 MyMath::MakeScaleMatrix(const Vector3& scale) {
		Matrix4x4 c;
		c.m[0][0] = scale.x;
		c.m[0][1] = 0;
		c.m[0][2] = 0;
		c.m[0][3] = 0;
		c.m[1][0] = 0;
		c.m[1][1] = scale.y;
		c.m[1][2] = 0;
		c.m[1][3] = 0;
		c.m[2][0] = 0;
		c.m[2][1] = 0;
		c.m[2][2] = scale.z;
		c.m[2][3] = 0;
		c.m[3][0] = 0;
		c.m[3][1] = 0;
		c.m[3][2] = 0;
		c.m[3][3] = 1;
		return c;
	}

	Matrix4x4 MyMath::MakeRotateMatrix(const Vector3& rotate) {
		Matrix4x4 result;
		Matrix4x4 matRotateX = MakeRotateXMatrix(rotate.x);
		Matrix4x4 matRotateY = MakeRotateYMatrix(rotate.y);
		Matrix4x4 matRotateZ = MakeRotateZMatrix(rotate.z);
		result = matRotateX * matRotateY * matRotateZ;
		return result;
	}

	Matrix4x4 MyMath::MakeRotateXMatrix(float radian) {
		Matrix4x4 c;
		c.m[0][0] = 1;
		c.m[0][1] = 0;
		c.m[0][2] = 0;
		c.m[0][3] = 0;
		c.m[1][0] = 0;
		c.m[1][1] = std::cos(radian);
		c.m[1][2] = std::sin(radian);
		c.m[1][3] = 0;
		c.m[2][0] = 0;
		c.m[2][1] = -std::sin(radian);
		c.m[2][2] = std::cos(radian);
		c.m[2][3] = 0;
		c.m[3][0] = 0;
		c.m[3][1] = 0;
		c.m[3][2] = 0;
		c.m[3][3] = 1;
		return c;
	}

	Matrix4x4 MyMath::MakeRotateYMatrix(float radian) {
		Matrix4x4 c;
		c.m[0][0] = std::cos(radian);
		c.m[0][1] = 0;
		c.m[0][2] = -std::sin(radian);
		c.m[0][3] = 0;
		c.m[1][0] = 0;
		c.m[1][1] = 1;
		c.m[1][2] = 0;
		c.m[1][3] = 0;
		c.m[2][0] = std::sin(radian);
		c.m[2][1] = 0;
		c.m[2][2] = std::cos(radian);
		c.m[2][3] = 0;
		c.m[3][0] = 0;
		c.m[3][1] = 0;
		c.m[3][2] = 0;
		c.m[3][3] = 1;
		return c;
	}

	Matrix4x4 MyMath::MakeRotateZMatrix(float radian) {
		Matrix4x4 c;
		c.m[0][0] = std::cos(radian);
		c.m[0][1] = std::sin(radian);
		c.m[0][2] = 0;
		c.m[0][3] = 0;
		c.m[1][0] = -std::sin(radian);
		c.m[1][1] = std::cos(radian);
		c.m[1][2] = 0;
		c.m[1][3] = 0;
		c.m[2][0] = 0;
		c.m[2][1] = 0;
		c.m[2][2] = 1;
		c.m[2][3] = 0;
		c.m[3][0] = 0;
		c.m[3][1] = 0;
		c.m[3][2] = 0;
		c.m[3][3] = 1;
		return c;
	}

	Matrix4x4 MyMath::MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
		Matrix4x4 c;
		//回転行列統合
		Matrix4x4 rx = MakeRotateXMatrix(rotate.x);
		Matrix4x4 ry = MakeRotateYMatrix(rotate.y);
		Matrix4x4 rz = MakeRotateZMatrix(rotate.z);
		Matrix4x4 rxyz = Multiply(rx, Multiply(ry, rz));

		c.m[0][0] = scale.x * rxyz.m[0][0];
		c.m[0][1] = scale.x * rxyz.m[0][1];
		c.m[0][2] = scale.x * rxyz.m[0][2];
		c.m[0][3] = 0;
		c.m[1][0] = scale.y * rxyz.m[1][0];
		c.m[1][1] = scale.y * rxyz.m[1][1];
		c.m[1][2] = scale.y * rxyz.m[1][2];
		c.m[1][3] = 0;
		c.m[2][0] = scale.z * rxyz.m[2][0];
		c.m[2][1] = scale.z * rxyz.m[2][1];
		c.m[2][2] = scale.z * rxyz.m[2][2];
		c.m[2][3] = 0;
		c.m[3][0] = translate.x;
		c.m[3][1] = translate.y;
		c.m[3][2] = translate.z;
		c.m[3][3] = 1;

		return c;
	}

	Matrix4x4 MyMath::MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate) {
		Matrix4x4 c;

		//クォータニオンから回転行列を生成
		Matrix4x4 rotationMatrix = MakeRotateMatrix(rotate);

		//スケールの適用
		c.m[0][0] = scale.x * rotationMatrix.m[0][0];
		c.m[0][1] = scale.x * rotationMatrix.m[0][1];
		c.m[0][2] = scale.x * rotationMatrix.m[0][2];
		c.m[0][3] = 0;

		c.m[1][0] = scale.y * rotationMatrix.m[1][0];
		c.m[1][1] = scale.y * rotationMatrix.m[1][1];
		c.m[1][2] = scale.y * rotationMatrix.m[1][2];
		c.m[1][3] = 0;

		c.m[2][0] = scale.z * rotationMatrix.m[2][0];
		c.m[2][1] = scale.z * rotationMatrix.m[2][1];
		c.m[2][2] = scale.z * rotationMatrix.m[2][2];
		c.m[2][3] = 0;

		//平行移動の適用
		c.m[3][0] = translate.x;
		c.m[3][1] = translate.y;
		c.m[3][2] = translate.z;
		c.m[3][3] = 1;

		return c;
	}

	Matrix4x4 MyMath::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
		Matrix4x4 c;
		c.m[0][0] = (1 / aspectRatio) * Cot(fovY / 2);
		c.m[0][1] = 0;
		c.m[0][2] = 0;
		c.m[0][3] = 0;
		c.m[1][0] = 0;
		c.m[1][1] = Cot(fovY / 2);
		c.m[1][2] = 0;
		c.m[1][3] = 0;
		c.m[2][0] = 0;
		c.m[2][1] = 0;
		c.m[2][2] = farClip / (farClip - nearClip);
		c.m[2][3] = 1;
		c.m[3][0] = 0;
		c.m[3][1] = 0;
		c.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
		c.m[3][3] = 0;
		return c;
	}

	Matrix4x4 MyMath::MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
		Matrix4x4 c;
		c.m[0][0] = 2 / (right - left);
		c.m[0][1] = 0;
		c.m[0][2] = 0;
		c.m[0][3] = 0;
		c.m[1][0] = 0;
		c.m[1][1] = 2 / (top - bottom);
		c.m[1][2] = 0;
		c.m[1][3] = 0;
		c.m[2][0] = 0;
		c.m[2][1] = 0;
		c.m[2][2] = 1 / (farClip - nearClip);
		c.m[2][3] = 0;
		c.m[3][0] = (left + right) / (left - right);
		c.m[3][1] = (top + bottom) / (bottom - top);
		c.m[3][2] = nearClip / (nearClip - farClip);
		c.m[3][3] = 1;
		return c;
	}

	Matrix4x4 MyMath::MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
		Matrix4x4 c;
		c.m[0][0] = width / 2;
		c.m[0][1] = 0;
		c.m[0][2] = 0;
		c.m[0][3] = 0;
		c.m[1][0] = 0;
		c.m[1][1] = -height / 2;
		c.m[1][2] = 0;
		c.m[1][3] = 0;
		c.m[2][0] = 0;
		c.m[2][1] = 0;
		c.m[2][2] = maxDepth - minDepth;
		c.m[2][3] = 0;
		c.m[3][0] = left + (width / 2);
		c.m[3][1] = top + (height / 2);
		c.m[3][2] = minDepth;
		c.m[3][3] = 1;
		return c;
	}

	Matrix4x4 MyMath::CreateRotationFromEulerAngles(const Vector3& rotate) {
		//ピッチ（X軸回転）、ヨー（Y軸回転）、ロール（Z軸回転）の角度をラジアンに変換
		float cosPitch = cosf(rotate.x);
		float sinPitch = sinf(rotate.x);
		float cosYaw = cosf(rotate.y);
		float sinYaw = sinf(rotate.y);
		float cosRoll = cosf(rotate.z);
		float sinRoll = sinf(rotate.z);

		//X軸回転行列
		Matrix4x4 rotationX = {
			1, 0, 0, 0,
			0, cosPitch, -sinPitch, 0,
			0, sinPitch, cosPitch, 0,
			0, 0, 0, 1
		};

		//Y軸回転行列
		Matrix4x4 rotationY = {
			cosYaw, 0, sinYaw, 0,
			0, 1, 0, 0,
			-sinYaw, 0, cosYaw, 0,
			0, 0, 0, 1
		};

		//Z軸回転行列
		Matrix4x4 rotationZ = {
			cosRoll, -sinRoll, 0, 0,
			sinRoll, cosRoll, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		//回転行列を合成する（順番重要）
		Matrix4x4 rotationMatrix = rotationZ * rotationY * rotationX;

		return rotationMatrix;
	}

	Quaternion MyMath::Add(const Quaternion& q1, const Quaternion& q2) {
		return Quaternion(q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w);
	}

	Quaternion MyMath::Subtract(const Quaternion& q1, const Quaternion& q2) {
		return Quaternion(q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w);
	}

	Quaternion MyMath::Multiply(const Quaternion& q1, const Quaternion& q2) {
		Quaternion c;
		//各Quaternoinのxyz成分をVector3に直す
		Vector3 q1v = { q1.x,q1.y ,q1.z };
		Vector3 q2v = { q2.x,q2.y ,q2.z };
		//演算
		c.w = q1.w * q2.w - Dot(q1v, q2v);
		c.x = Vector3(Cross(q1v, q2v) + (q2.w * q1v) + (q1.w * q2v)).x;
		c.y = Vector3(Cross(q1v, q2v) + (q2.w * q1v) + (q1.w * q2v)).y;
		c.z = Vector3(Cross(q1v, q2v) + (q2.w * q1v) + (q1.w * q2v)).z;

		return c;
	}

	Quaternion MyMath::Multiply(float scalar, const Quaternion& q) {
		return { q.x * scalar,q.y * scalar, q.z * scalar, q.w * scalar };
	}

	float MyMath::Dot(const Quaternion& q1, const Quaternion& q2) {
		return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
	}

	float MyMath::Norm(const Quaternion& q) {
		return std::sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
	}

	Quaternion MyMath::Normalize(const Quaternion& q) {
		float n = Norm(q);
		if (n == 0.0f) {
			throw std::runtime_error("Zero norm quaternion cannot be normalized");
		}
		return Quaternion(q.x / n, q.y / n, q.z / n, q.w / n);
	}

	Quaternion MyMath::Conjugate(const Quaternion& q) {
		return Quaternion(-q.x, -q.y, -q.z, q.w);
	}

	Quaternion MyMath::Inverse(const Quaternion& q) {
		float normSquared = q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z;
		if (normSquared == 0.0f) {
			throw std::runtime_error("Cannot invert a quaternion with zero norm");
		}
		Quaternion conjugate = Conjugate(q);
		return Quaternion(conjugate.x / normSquared, conjugate.y / normSquared,
			conjugate.z / normSquared, conjugate.w / normSquared);
	}

	Quaternion MyMath::MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
		float halfAngle = angle * 0.5f;
		float sinHalfAngle = sin(halfAngle);
		return Quaternion(
			cos(halfAngle),
			axis.x * sinHalfAngle,
			axis.y * sinHalfAngle,
			axis.z * sinHalfAngle
		);
	}

	Vector3 MyMath::RotateVector(const Vector3& vector, const Quaternion& quaternion) {
		Vector3 c;
		Quaternion vtq = { vector.x,vector.y, vector.z, 0.0f };
		Quaternion result = quaternion * vtq * Inverse(quaternion);
		c.x = result.x;
		c.y = result.y;
		c.z = result.z;
		return c;
	}

	Matrix4x4 MyMath::MakeRotateMatrix(const Quaternion& q) {
		Matrix4x4 c;
		c.m[0][0] = std::powf(q.w, 2) + std::powf(q.x, 2) - std::powf(q.y, 2) - std::powf(q.z, 2);
		c.m[0][1] = 2.0f * (q.x * q.y + q.w * q.z);
		c.m[0][2] = 2.0f * (q.x * q.z - q.w * q.y);
		c.m[0][3] = 0.0f;
		c.m[1][0] = 2.0f * (q.x * q.y - q.w * q.z);
		c.m[1][1] = std::powf(q.w, 2) - std::powf(q.x, 2) + std::powf(q.y, 2) - std::powf(q.z, 2);
		c.m[1][2] = 2.0f * (q.y * q.z + q.w * q.x);
		c.m[1][3] = 0.0f;
		c.m[2][0] = 2.0f * (q.x * q.z + q.w * q.y);
		c.m[2][1] = 2.0f * (q.y * q.z - q.w * q.x);
		c.m[2][2] = std::powf(q.w, 2) - std::powf(q.x, 2) - std::powf(q.y, 2) + std::powf(q.z, 2);
		c.m[2][3] = 0.0f;
		c.m[3][0] = 0.0f;
		c.m[3][1] = 0.0f;
		c.m[3][2] = 0.0f;
		c.m[3][3] = 1.0f;
		return c;
	}

	Quaternion MyMath::FromEulerAngles(Vector3 euler) {
		float cy = std::cos(euler.x * 0.5f);
		float sy = std::sin(euler.x * 0.5f);
		float cp = std::cos(euler.y * 0.5f);
		float sp = std::sin(euler.y * 0.5f);
		float cr = std::cos(euler.z * 0.5f);
		float sr = std::sin(euler.z * 0.5f);

		return Quaternion(
			sr * cp * cy - cr * sp * sy,
			cr * sp * cy + sr * cp * sy,
			cr * cp * sy - sr * sp * cy,
			cr * cp * cy + sr * sp * sy
		);
	}

	Vector3 MyMath::ToEulerAngles(const Quaternion& q) {
		Vector3 euler;

		//ピッチ（x軸回り）
		float sinp = 2.0f * (q.w * q.x + q.y * q.z);
		if (std::abs(sinp) >= 1)
			euler.x = std::copysign(std::numbers::pi_v<float> / 2.0f, sinp); //ピッチを -90 〜 90 度にクランプ
		else
			euler.x = std::asin(sinp);

		//ヨー（y軸回り）
		float siny_cosy = 2.0f * (q.w * q.y - q.z * q.x);
		float cosy_cosy = 1.0f - 2.0f * (q.y * q.y + q.x * q.x);
		euler.y = std::atan2(siny_cosy, cosy_cosy);

		//ロール（z軸回り）
		float sinr_cosr = 2.0f * (q.w * q.z + q.x * q.y);
		float cosr_cosr = 1.0f - 2.0f * (q.z * q.z + q.x * q.x);
		euler.z = std::atan2(sinr_cosr, cosr_cosr);

		return euler;
	}

	Quaternion MyMath::Slerp(const Quaternion& q0, const Quaternion& q1, float t) {
		Quaternion c;
		Quaternion q0c = q0;
		Quaternion q1c = q1;
		//q0とq1の内積
		float dot = Dot(q0, q1);
		if (dot < 0.0f) {
			//もう片方の回転を利用する
			q0c = -q0c;
			//内積も反転
			dot = -dot;
		}
		//内積が1に近い場合、線形補完を使用
		if (dot >= 1.0f - epsilon) {
			c = q0c * (1.0f - t) + q1c * t;
			return c;
		}

		//なす角を求める
		float theta = std::acosf(dot);
		//thetaとsinを使って補間係数scale0,scale1を求める
		float sin_theta = std::sqrt(1.0f - dot * dot);
		float scale0 = std::sin((1.0f - t) * theta) / sin_theta;
		float scale1 = std::sin(t * theta) / sin_theta;
		//補間
		c = scale0 * q0c + scale1 * q1c;

		return c;
	}

	float MyMath::Cot(float rad) {
		float c;

		c = 1 / std::tan(rad);

		return c;
	}

	float MyMath::Length(const Vector3& v) {
		float c;
		c = std::sqrt(static_cast<float>(std::pow(v.x, 2) + std::pow(v.y, 2) + std::pow(v.z, 2)));
		return c;
	}

	float MyMath::Dot(const Vector2& v1, const Vector2& v2) {
		float c;
		c = v1.x * v2.x + v1.y * v2.y;
		return c;
	}

	float MyMath::Dot(const Vector3& v1, const Vector3& v2) {
		float c;
		c = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
		return c;
	}


	float MyMath::Lerp(float s1, float s2, float t) {
		return s1 * (1 - t) + s2 * t;
	}

	float MyMath::NormalizeAngle(float angle) {

		// -2π～2π くらいに収める
		angle = std::fmod(angle, pi * 2.0f);

		// -π～π に収める
		if (angle > pi) {
			angle -= pi * 2.0f;
		}
		else if (angle < -pi) {
			angle += pi * 2.0f;
		}

		return angle;
	}

	TransformEuler MyMath::Combine(const TransformEuler& parent, const TransformEuler& local) {
		TransformEuler world;
		//スケール 
		world.scale.x = parent.scale.x * local.scale.x; world.scale.y = parent.scale.y * local.scale.y; world.scale.z = parent.scale.z * local.scale.z;
		//回転 (Euler → Quaternion → 合成 → Euler) 
		Quaternion qParent = FromEulerAngles(parent.rotate); Quaternion qLocal = FromEulerAngles(local.rotate); Quaternion qWorld = qParent * qLocal; world.rotate = ToEulerAngles(qWorld);
		//平行移動 
		Vector3 scaled{ parent.scale.x * local.translate.x, parent.scale.y * local.translate.y, parent.scale.z * local.translate.z }; world.translate = parent.translate + MyMath::RotateVector(scaled, qParent);

		return world;
	}

	std::pair<float, float> MyMath::ProjectOntoAxis(const Vector3* vertices, int count, const Vector3& axis) {
		float min = Dot(vertices[0], axis);
		float max = min;
		for (int i = 1; i < count; ++i) {
			float projection = Dot(vertices[i], axis);
			min = std::min(min, projection);
			max = std::max(max, projection);
		}
		return { min, max };
	}

	float MyMath::EaseInSine(float ratio) {
		return float(1 - std::cos((ratio * std::numbers::pi_v<float>) / 2));
	}

	float MyMath::EaseOutSine(float ratio) {
		return float(std::sin((ratio * std::numbers::pi_v<float>) / 2));
	}

	float MyMath::EaseInOutSine(float ratio) {
		return float(-(std::cos(std::numbers::pi_v<float> *ratio) - 1) / 2);
	}

	float MyMath::EaseInCubic(float ratio) {
		return float(ratio * ratio * ratio);
	}

	float MyMath::EaseOutCubic(float ratio) {
		return float(1 - std::pow(1 - ratio, 3));
	}

	float MyMath::EaseInOutCubic(float ratio) {
		return float(ratio < 0.5 ? 4 * ratio * ratio * ratio : 1 - std::pow(-2 * ratio + 2, 3) / 2);
	}

	float MyMath::EaseInQuint(float ratio) {
		return float(ratio * ratio * ratio * ratio * ratio);
	}

	float MyMath::EaseOutQuint(float ratio) {
		return float(1 - std::pow(1 - ratio, 5));
	}

	float MyMath::EaseInOutQuint(float ratio) {
		return float(ratio < 0.5 ? 16 * ratio * ratio * ratio * ratio * ratio : 1 - std::pow(-2 * ratio + 2, 5) / 2);
	}

	float MyMath::EaseInCirc(float ratio) {
		return float(1 - std::sqrt(1 - std::pow(ratio, 2)));
	}

	float MyMath::EaseOutCirc(float ratio) {
		return float(std::sqrt(1 - std::pow(ratio - 1, 2)));
	}

	float MyMath::EaseInOutCirc(float ratio) {
		return float(ratio < 0.5
			? (1 - std::sqrt(1 - std::pow(2 * ratio, 2))) / 2
			: (std::sqrt(1 - std::pow(-2 * ratio + 2, 2)) + 1) / 2);
	}

	float MyMath::EaseInElastic(float ratio) {
		const float c4 = (2 * std::numbers::pi_v<float>) / 3;

		return float(ratio == 0
			? 0
			: ratio == 1
			? 1
			: -std::pow(2, 10 * ratio - 10) * std::sin((ratio * 10 - 10.75) * c4));
	}

	float MyMath::EaseOutElastic(float ratio) {
		const float c4 = (2 * std::numbers::pi_v<float>) / 3;

		return float(ratio == 0
			? 0
			: ratio == 1
			? 1
			: std::pow(2, -10 * ratio) * std::sin((ratio * 10 - 0.75) * c4) + 1);
	}

	float MyMath::EaseInOutElastic(float ratio) {
		const float c5 = (2 * std::numbers::pi_v<float>) / 4.5f;

		return float(ratio == 0
			? 0
			: ratio == 1
			? 1
			: ratio < 0.5
			? -(std::pow(2, 20 * ratio - 10) * std::sin((20 * ratio - 11.125) * c5)) / 2
			: (std::pow(2, -20 * ratio + 10) * std::sin((20 * ratio - 11.125) * c5)) / 2 + 1);
	}

	float MyMath::EaseInQuad(float ratio) {
		return float(ratio * ratio);
	}

	float MyMath::EaseOutQuad(float ratio) {
		return float(1 - (1 - ratio) * (1 - ratio));
	}

	float MyMath::EaseInOutQuad(float ratio) {
		return float(ratio < 0.5 ? 2 * ratio * ratio : 1 - std::pow(-2 * ratio + 2, 2) / 2);
	}

	float MyMath::EaseInQuart(float ratio) {
		return float(ratio * ratio * ratio * ratio);
	}

	float MyMath::EaseOutQuart(float ratio) {
		return float(1 - std::pow(1 - ratio, 4));
	}

	float MyMath::EaseInOutQuart(float ratio) {
		return float(ratio < 0.5 ? 8 * ratio * ratio * ratio * ratio : 1 - std::pow(-2 * ratio + 2, 4) / 2);
	}

	float MyMath::EaseInExpo(float ratio) {
		return float(ratio == 0 ? 0 : std::pow(2, 10 * ratio - 10));
	}

	float MyMath::EaseOutExpo(float ratio) {
		return float(ratio == 1 ? 1 : 1 - std::pow(2, -10 * ratio));
	}

	float MyMath::EaseInOutExpo(float ratio) {
		return float(ratio == 0
			? 0
			: ratio == 1
			? 1
			: ratio < 0.5 ? std::pow(2, 20 * ratio - 10) / 2
			: (2 - std::pow(2, -20 * ratio + 10)) / 2);
	}

	float MyMath::EaseInBack(float ratio) {
		const float c1 = 1.70158f;
		const float c3 = c1 + 1;

		return float(c3 * ratio * ratio * ratio - c1 * ratio * ratio);
	}

	float MyMath::EaseOutBack(float ratio) {
		const float c1 = 1.70158f;
		const float c3 = c1 + 1;

		return float(1 + c3 * std::pow(ratio - 1, 3) + c1 * std::pow(ratio - 1, 2));
	}

	float MyMath::EaseInOutBack(float ratio) {
		const float c1 = 1.70158f;
		const float c2 = c1 * 1.525f;

		return float(ratio < 0.5
			? (std::pow(2 * ratio, 2) * ((c2 + 1) * 2 * ratio - c2)) / 2
			: (std::pow(2 * ratio - 2, 2) * ((c2 + 1) * (ratio * 2 - 2) + c2) + 2) / 2);
	}

	float MyMath::EaseInBounce(float ratio) {
		return float(1 - EaseOutBounce(1 - ratio));
	}

	float MyMath::EaseOutBounce(float ratio) {
		const float n1 = 7.5625f;
		const float d1 = 2.75f;

		if (ratio < 1 / d1) {
			return float(n1 * ratio * ratio);
		}
		else if (ratio < 2 / d1) {
			return float(n1 * (ratio -= 1.5f / d1) * ratio + 0.75);
		}
		else if (ratio < 2.5 / d1) {
			return float(n1 * (ratio -= 2.25f / d1) * ratio + 0.9375);
		}
		else {
			return float(n1 * (ratio -= 2.625f / d1) * ratio + 0.984375);
		}
	}

	float MyMath::EaseInOutBounce(float ratio) {
		return float(ratio < 0.5
			? (1 - EaseOutBounce(1 - 2 * ratio)) / 2
			: (1 + EaseOutBounce(2 * ratio - 1)) / 2);
	}

	bool MyMath::IsCollision(const Sphere& s1, const Sphere& s2) {
		//2つの球の中心点間の距離を求める
		float distance = Length(Subtract(s2.center, s1.center));
		//半径の合計よりも短ければ衝突
		if (distance <= s1.radius + s2.radius) {
			return true;
		}
		else {
			return false;
		}
	}

	bool MyMath::IsCollision(const Plane& plane, const Sphere& sphere) {
		//球の中心と平面との距離を計算
		float distance = DistancePointToPlane(sphere.center, plane);
		//衝突判定
		if (distance <= sphere.radius) {
			return true;
		}
		else {
			return false;
		}
	}

	bool MyMath::IsCollision(const Sphere& sphere, const Plane& plane) {
		return IsCollision(plane, sphere);
	}

	bool MyMath::IsCollision(const Capsule& capsule, const Sphere& sphere) {
		//球の中心とカプセル内線分の最近接点を求める
		Vector3 closestPoint;
		closestPoint = ClosestPoint(sphere.center, capsule.segment);
		//最近接点と球の中心との距離を求める
		float distance = Vector3(sphere.center - closestPoint).Length();
		//半径の合計よりも短ければ衝突
		if (distance <= sphere.radius + capsule.radius) {
			return true;
		}
		else {
			return false;
		}
	}

	bool MyMath::IsCollision(const Sphere& sphere, const Capsule& capsule) {
		return IsCollision(capsule, sphere);
	}

	bool MyMath::IsCollision(const Line& line, const Plane& plane) {
		//垂直判定を求める
		float dot = Dot(plane.normal, line.diff);
		//平行の場合衝突していない。
		if (dot == 0.0f) {
			return false;
		}
		//tを求める
		float t = (plane.distance - Dot(line.origin, plane.normal)) / dot;
		//直線の当たり判定
		if (t >= 0 || t < 0) {
			return true;
		}
		else {
			return false;
		}
	}

	bool MyMath::IsCollision(const Plane& plane, const Line& line) {
		return IsCollision(line, plane);
	}

	bool MyMath::IsCollision(const Ray& ray, const Plane& plane) {
		//垂直判定を求める
		float dot = Dot(plane.normal, ray.diff);
		//平行の場合衝突していない。
		if (dot == 0.0f) {
			return false;
		}
		//tを求める
		float t = (plane.distance - Dot(ray.origin, plane.normal)) / dot;
		//半直線の当たり判定
		if (t >= 0) {
			return true;
		}
		else {
			return false;
		}
	}

	bool MyMath::IsCollision(const Plane& plane, const Ray& ray) {
		return IsCollision(ray, plane);
	}

	bool MyMath::IsCollision(const Segment& segment, const Plane& plane) {
		//平行判定
		float dot = Dot(segment.diff, plane.normal);
		//平行なので衝突していない
		if (dot == 0.0f) {
			return false;
		}
		//媒介変数
		float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;
		//当たり判定結果
		if (t >= 0.0f && t <= 1.0f) {
			return true;
		}
		else {
			return false;
		}
	}

	bool MyMath::IsCollision(const Plane& plane, const Segment& segment) {
		return IsCollision(segment, plane);
	}

	bool MyMath::IsCollision(const Capsule& capsule, const Plane& plane) {
		///判定を使う場合ここに処理を記入
		return false;
	}

	bool MyMath::IsCollision(const Plane& plane, const Capsule& capsule) {
		return IsCollision(capsule, plane);
	}

	bool MyMath::IsCollision(const Capsule& capsule, const Triangle& tri) {
		//三角形を線分に分ける処理
		auto makeTriangleSegments = [](const Triangle& tri) -> std::vector<Segment> {
			return {
				{ tri.vertices[0], tri.vertices[1] - tri.vertices[0] },
				{ tri.vertices[1], tri.vertices[2] - tri.vertices[1] },
				{ tri.vertices[2], tri.vertices[0] - tri.vertices[2] }
			};
			};
		//線分同士の距離による判定
		auto judgeSegmentsDistance = [&]() -> bool {
			//三角形を線分に分解する
			std::vector<Segment> s;
			s = makeTriangleSegments(tri);
			//カプセル線分と三角形線分を比較し、一番短いのを選別する
			float d1, d2, d3;
			d1 = DistanceSegmentToSegment(capsule.segment, s[0]);
			d2 = DistanceSegmentToSegment(capsule.segment, s[1]);
			d3 = DistanceSegmentToSegment(capsule.segment, s[2]);
			float distanceOfMin = std::min(d1, std::min(d2, d3));
			//最短距離をカプセルの半径と比較して判定する
			if (distanceOfMin < capsule.radius) {
				//衝突してる
				return true;
			}
			else {
				//衝突してない
				return false;
			}
			};
		//点と線分の距離による判定
		auto judgePointToSegmentDistance = [&](const Vector3& point, const Segment& segment)->bool {
			if (DistancePointToSegment(point, segment) < capsule.radius) {
				//衝突してる
				return true;
			}
			else {
				//衝突してない
				return false;
			}
			};

		//三角形を線分に分解する
		std::vector<Segment> triSegments;
		triSegments = makeTriangleSegments(tri);

		//カプセル内部の線分が三角形平面と平行かどうかの判定を行う
		Plane triPlane = MakePlane(tri);
		if (CheckParallel(capsule.segment, triPlane)) {
			//カプセル内部の線分を平面に射影したときの線分を求める
			Segment projSeg = ProjectSegmentOntoPlane(capsule.segment, triPlane);
			//三角形とカプセル線分の最短距離
			float distanceOfMin = 0.0f;
			//射影線分が三角形に衝突しているかの判定を行う
			if (IsCollision(projSeg, tri)) {
				//カプセルの線分との距離を求めるための三角形内部のある点
				Vector3 point = {};
				//始点終点の三角形の内外判定で調べる
				Vector3 sp = projSeg.origin;
				Vector3 ep = projSeg.origin + projSeg.diff;
				if (IsPointInTriangle(sp, tri)) {
					point = sp;
				}
				else if (IsPointInTriangle(ep, tri)) {
					point = ep;
				}

				//射影線分と各線分の交差点を求める(どこでもいいのでelse if)
				if (DistanceSegmentToSegment(projSeg, triSegments[0]) < epsilon) {
					point = IntersectionSegmentToSegment(projSeg, triSegments[0]);
				}
				else if (DistanceSegmentToSegment(projSeg, triSegments[1]) < epsilon) {
					point = IntersectionSegmentToSegment(projSeg, triSegments[1]);
				}
				else if (DistanceSegmentToSegment(projSeg, triSegments[2]) < epsilon) {
					point = IntersectionSegmentToSegment(projSeg, triSegments[2]);
				}
				//ポイントが求まったのでそのポイントとカプセル線分との距離を求める
				distanceOfMin = DistancePointToSegment(point, capsule.segment);
			}
			else {
				//カプセル線分と三角形線分を比較し、一番短いのを選別する
				float d1, d2, d3;
				d1 = DistanceSegmentToSegment(capsule.segment, triSegments[0]);
				d2 = DistanceSegmentToSegment(capsule.segment, triSegments[1]);
				d3 = DistanceSegmentToSegment(capsule.segment, triSegments[2]);
				distanceOfMin = std::min(d1, std::min(d2, d3));
			}
			//求めた最短距離とカプセルの半径を比較して判定処理を行う
			if (distanceOfMin < capsule.radius) {
				//衝突してる
				return true;
			}
			else {
				//衝突してない
				return false;
			}
		}
		//媒介変数から交差点を求める(線分を伸ばした場合も含む)
		float dot = Dot(capsule.segment.diff, triPlane.normal);
		float t = (triPlane.distance - Dot(capsule.segment.origin, triPlane.normal)) / dot;
		Vector3 crossPoint = capsule.segment.origin + capsule.segment.diff * t;
		//カプセル線分が三角形含む平面を通ってる場合
		if (IsCollision(capsule.segment, triPlane)) {
			//カプセル線分が平面に垂直か調べる
			if (std::fabs(Dot(Vector3(capsule.segment.diff).Normalized(), Vector3(triPlane.normal).Normalized()) > 1.0f - epsilon)) {
				//交差点が三角形の内側にあるかを判定
				if (IsPointInTriangle(crossPoint, tri)) {
					//衝突してる
					return true;
				}
				else {
					//交差点と三角形の(三角形上の)最近接点を求める
					Vector3 cp = ClosestPoint(crossPoint, tri);
					//交差点→最近接点の距離とカプセルの半径を比較して衝突判定をとる
					if (Vector3(cp - crossPoint).Length() < capsule.radius) {
						//衝突してる
						return true;
					}
					else {
						//衝突していない
						return false;
					}
				}
			}
			else {
				//交差点が三角形の内側にあるかを判定
				if (IsPointInTriangle(crossPoint, tri)) {
					//衝突してる
					return true;
				}
				else {
					//線分同士の距離による判定
					return judgeSegmentsDistance();
				}
			}
		}
		//カプセル線分が平面とぶつかっていない場合
		else {
			//交差点の三角形の内外判定を求める
			if (IsPointInTriangle(crossPoint, tri)) {
				//線分の始点と終点の内、交差点と近い方の点の射影点を求める
				Vector3 projPoint;
				float lsp = Vector3(crossPoint - capsule.segment.origin).Length();
				float lep = Vector3(crossPoint - (capsule.segment.origin + capsule.segment.diff)).Length();
				//始点の方が近い場合
				if (lsp < lep) {
					projPoint = ClosestPoint(capsule.segment.origin, triPlane);
				}
				//終点の方が近い場合
				else {
					projPoint = ClosestPoint((capsule.segment.origin + capsule.segment.diff), triPlane);
				}
				//射影点の三角形の内外判定をとる
				if (IsPointInTriangle(projPoint, tri)) {
					//射影点→カプセル線分の最短距離とカプセルの半径を比較して判定
					return judgePointToSegmentDistance(projPoint, capsule.segment);
				}
				else {
					//交差点→射影点の線分を作る
					Segment segment = {
						.origin = crossPoint,
						.diff = projPoint - crossPoint
					};
					//線分同士がぶつかっているペアに限ってその交差点を求める
					Vector3 crossPoint2;
					if (DistanceSegmentToSegment(segment, triSegments[0]) < epsilon) {
						crossPoint2 = IntersectionSegmentToSegment(segment, triSegments[0]);
						//求めた交差点→カプセル線分の最短距離をカプセルと比較して判定
						return judgePointToSegmentDistance(crossPoint2, capsule.segment);
					}
					else if (DistanceSegmentToSegment(segment, triSegments[1]) < epsilon) {
						crossPoint2 = IntersectionSegmentToSegment(segment, triSegments[1]);
						//求めた交差点→カプセル線分の最短距離をカプセルと比較して判定
						return judgePointToSegmentDistance(crossPoint2, capsule.segment);
					}
					else if (DistanceSegmentToSegment(segment, triSegments[2]) < epsilon) {
						crossPoint2 = IntersectionSegmentToSegment(segment, triSegments[2]);
						//求めた交差点→カプセル線分の最短距離をカプセルと比較して判定
						return judgePointToSegmentDistance(crossPoint2, capsule.segment);
					}
					else {
						//本来ここは通らない
						return false;
					}
				}
			}
			//交差点が三角形の外側なら
			else {
				//カプセル線分を平面上に射影する
				Segment projSegment = ProjectSegmentOntoPlane(capsule.segment, triPlane);
				//射影線分と三角形の衝突判定
				if (IsCollision(projSegment, tri)) {
					//射影後の始点と終点の距離を求める
					float lsp = Vector3(projSegment.origin - capsule.segment.origin).Length();
					float lep = Vector3((projSegment.origin + projSegment.diff) - (capsule.segment.origin + capsule.segment.diff)).Length();
					//それぞれの距離の短い方の射影点を求める
					Vector3 projPoint;	//射影点(平面に近い点を参照)
					//始点の方が短い場合
					if (lsp < lep) {
						projPoint = projSegment.origin;
					}
					//終点の方が短い場合
					else {
						projPoint = projSegment.origin + projSegment.diff;
					}
					//求めた射影点で三角形の内外判定
					if (IsPointInTriangle(projPoint, tri)) {
						//射影点→線分の長さとカプセルの半径を比較して判定
						return judgePointToSegmentDistance(projPoint, capsule.segment);
					}
					else {
						//射影線分と三角形線分の衝突判定
						Vector3 crossPoint2;
						bool isCompetition = false;
						if (DistanceSegmentToSegment(projSegment, triSegments[0]) < epsilon) {
							//交点を求める
							crossPoint2 = IntersectionSegmentToSegment(projSegment, triSegments[0]);
							isCompetition = true;
						}
						if (DistanceSegmentToSegment(projSegment, triSegments[1]) < epsilon) {
							//競合していたら旧交点と比較
							if (isCompetition) {
								Vector3 newCrossPoint = IntersectionSegmentToSegment(projSegment, triSegments[1]);
								//新交点→射影点の方が短い場合
								if (Vector3(projPoint - newCrossPoint).Length() < Vector3(projPoint - crossPoint2).Length()) {
									//交点を更新
									crossPoint2 = newCrossPoint;
								}
							}
							else {
								//交点を求める
								crossPoint2 = IntersectionSegmentToSegment(projSegment, triSegments[1]);
							}
							isCompetition = true;
						}
						if (DistanceSegmentToSegment(projSegment, triSegments[2]) < epsilon) {
							//競合していたら旧交点と比較
							if (isCompetition) {
								Vector3 newCrossPoint = IntersectionSegmentToSegment(projSegment, triSegments[1]);
								//新交点→射影点の方が短い場合
								if (Vector3(projPoint - newCrossPoint).Length() < Vector3(projPoint - crossPoint2).Length()) {
									//交点を更新
									crossPoint2 = newCrossPoint;
								}
							}
							else {
								//交点を求める
								crossPoint2 = IntersectionSegmentToSegment(projSegment, triSegments[1]);
							}
						}
						//一番カプセル線分に近い射影線分と三角形の交点が求まったのでカプセル半径と比較して判定
						return judgePointToSegmentDistance(crossPoint2, capsule.segment);
					}
				}
				else {
					//線分同士の距離による判定
					return judgeSegmentsDistance();
				}
			}
		}
	}

	bool MyMath::IsCollision(const Triangle& tri, const Capsule& capsule) {
		return IsCollision(capsule, tri);
	}

	bool MyMath::IsCollision(const Segment& segment, const Triangle& triangle) {
		// 三角形を含んでいる面を作る
		Plane plane = MakePlane(triangle);

		// 面と線分の交差判定
		float dot = Dot(segment.diff, plane.normal);
		if (fabs(dot) < epsilon) {
			//内外判定と最短距離を使って三角形内部かを調べる
			bool isHitting = false;
			//線分の始点と終点
			Vector3 startPoint = segment.origin;
			Vector3 endPoint = segment.origin + segment.diff;
			//始点の内外判定
			if (IsPointInTriangle(startPoint, triangle)) {
				//内側にあるので当たってる
				return true;
			}
			//終点の内外判定
			if (IsPointInTriangle(endPoint, triangle)) {
				//内側にあるので当たってる
				return true;
			}
			//三角形が構成する3つの線分を出す
			Segment s1, s2, s3;
			s1.origin = triangle.vertices[0];
			s1.diff = triangle.vertices[1] - triangle.vertices[0];
			s2.origin = triangle.vertices[1];
			s2.diff = triangle.vertices[2] - triangle.vertices[1];
			s3.origin = triangle.vertices[2];
			s3.diff = triangle.vertices[0] - triangle.vertices[2];
			//射影線分と各線分の交差点を求める
			if (DistanceSegmentToSegment(segment, s1) < epsilon) {
				return true;
			}
			else if (DistanceSegmentToSegment(segment, s2) < epsilon) {
				return true;
			}
			else if (DistanceSegmentToSegment(segment, s3) < epsilon) {
				return true;
			}

			//平行かつ線分が三角形内部に存在しない
			return false;
		}

		// tを求める
		float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;

		// 線分範囲外なら交差しない
		if (t < 0.0f || t > 1.0f) {
			return false;
		}

		// 衝突点p
		Vector3 p = segment.origin + segment.diff * t;

		// ---- 三角形内判定 ----
		Vector3 v01 = triangle.vertices[1] - triangle.vertices[0];
		Vector3 v12 = triangle.vertices[2] - triangle.vertices[1];
		Vector3 v20 = triangle.vertices[0] - triangle.vertices[2];

		Vector3 v0p = p - triangle.vertices[0];
		Vector3 v1p = p - triangle.vertices[1];
		Vector3 v2p = p - triangle.vertices[2];

		Vector3 cross01 = Cross(v01, v0p);
		Vector3 cross12 = Cross(v12, v1p);
		Vector3 cross20 = Cross(v20, v2p);

		if (Dot(cross01, plane.normal) >= 0.0f &&
			Dot(cross12, plane.normal) >= 0.0f &&
			Dot(cross20, plane.normal) >= 0.0f) {
			return true;  // 衝突
		}

		return false; // 平面には当たっているが、三角形外部
	}

	bool MyMath::IsCollision(const Triangle& triangle, const Segment& segment) {
		return IsCollision(segment, triangle);
	}

	bool MyMath::IsCollision(const AABB& a, const AABB& b) {
		if ((a.min.x <= b.max.x && a.max.x >= b.min.x) &&
			(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
			(a.min.z <= b.max.z && a.max.z >= b.min.z)) {
			//衝突
			return true;
		}
		else {
			return false;
		}
	}

	bool MyMath::IsCollision(const AABB& aabb, const Vector3& point) {
		//点の全ての座標成分がAABB内にあるなら衝突
		if ((point.x >= aabb.min.x && point.x <= aabb.max.x) &&
			(point.y >= aabb.min.y && point.y <= aabb.max.y) &&
			(point.z >= aabb.min.z && point.z <= aabb.max.z)) {
			return true;
		}
		return false;
	}

	bool MyMath::IsCollision(const Vector3& point, const AABB& aabb) {
		return IsCollision(aabb, point);
	}

	bool MyMath::IsCollision(const AABB& aabb, const Sphere& sphere) {
		//球の中心とAABBとの最近接点を求める
		Vector3 closestPoint = ClosestPoint(sphere.center, aabb);
		//最近接点と球の中心の距離を求める
		float distance = Length(Subtract(closestPoint, sphere.center));
		//距離が平均よりも小さければ衝突
		if (distance <= sphere.radius) {
			return true;
		}
		else {
			return false;
		}
	}

	bool MyMath::IsCollision(const Sphere& sphere, const AABB& aabb) {
		return IsCollision(aabb, sphere);
	}

	bool MyMath::IsCollision(const AABB& aabb, const Line& line) {
		//segmentの成分が全て0(点)の場合エラー
		if (line.diff.x == 0 && line.diff.y == 0 && line.diff.z == 0) {
			assert("線の成分が全て0");
		}

		//6つの平面を構造体に入れる
		Plane pxmin;
		Plane pxmax;
		Plane pymin;
		Plane pymax;
		Plane pzmin;
		Plane pzmax;
		//法線の値を入力
		pxmin.normal = { 1,0,0 };
		pxmax.normal = { 1,0,0 };
		pymin.normal = { 0,1,0 };
		pymax.normal = { 0,1,0 };
		pzmin.normal = { 0,0,1 };
		pzmax.normal = { 0,0,1 };
		//距離の値を入力
		pxmin.distance = Dot(aabb.min, pxmin.normal);
		pymin.distance = Dot(aabb.min, pymin.normal);
		pzmin.distance = Dot(aabb.min, pzmin.normal);
		pxmax.distance = Dot(aabb.max, pxmax.normal);
		pymax.distance = Dot(aabb.max, pymax.normal);
		pzmax.distance = Dot(aabb.max, pzmax.normal);
		//それぞれの平面と線分の衝突点を求める
		if (IsCollision(line, pxmin)) {
			Vector3 cp = CollisionPoint(line, pxmin);
			if (
				cp.x >= aabb.min.x && cp.x <= aabb.max.x &&
				cp.y >= aabb.min.y && cp.y <= aabb.max.y &&
				cp.z >= aabb.min.z && cp.z <= aabb.max.z
				) {
				return true;
			}
		}
		if (IsCollision(line, pxmax)) {
			Vector3 cp = CollisionPoint(line, pxmax);
			if (
				cp.x >= aabb.min.x && cp.x <= aabb.max.x &&
				cp.y >= aabb.min.y && cp.y <= aabb.max.y &&
				cp.z >= aabb.min.z && cp.z <= aabb.max.z
				) {
				return true;
			}
		}
		if (IsCollision(line, pymin)) {
			Vector3 cp = CollisionPoint(line, pymin);
			if (
				cp.x >= aabb.min.x && cp.x <= aabb.max.x &&
				cp.y >= aabb.min.y && cp.y <= aabb.max.y &&
				cp.z >= aabb.min.z && cp.z <= aabb.max.z
				) {
				return true;
			}
		}
		if (IsCollision(line, pymax)) {
			Vector3 cp = CollisionPoint(line, pymax);
			if (
				cp.x >= aabb.min.x && cp.x <= aabb.max.x &&
				cp.y >= aabb.min.y && cp.y <= aabb.max.y &&
				cp.z >= aabb.min.z && cp.z <= aabb.max.z
				) {
				return true;
			}
		}
		if (IsCollision(line, pzmin)) {
			Vector3 cp = CollisionPoint(line, pzmin);
			if (
				cp.x >= aabb.min.x && cp.x <= aabb.max.x &&
				cp.y >= aabb.min.y && cp.y <= aabb.max.y &&
				cp.z >= aabb.min.z && cp.z <= aabb.max.z
				) {
				return true;
			}
		}
		if (IsCollision(line, pzmax)) {
			Vector3 cp = CollisionPoint(line, pzmax);
			if (
				cp.x >= aabb.min.x && cp.x <= aabb.max.x &&
				cp.y >= aabb.min.y && cp.y <= aabb.max.y &&
				cp.z >= aabb.min.z && cp.z <= aabb.max.z
				) {
				return true;
			}
		}
		//これ以上衝突条件は無い
		return false;
	}

	bool MyMath::IsCollision(const Line& line, const AABB& aabb) {
		return IsCollision(aabb, line);
	}

	bool MyMath::IsCollision(const AABB& aabb, const Ray& ray) {
		//segmentの成分が全て0(点)の場合エラー
		if (ray.diff.x == 0 && ray.diff.y == 0 && ray.diff.z == 0) {
			assert("線の成分が全て0");
		}

		//6つの平面を構造体に入れる
		Plane pxmin;
		Plane pxmax;
		Plane pymin;
		Plane pymax;
		Plane pzmin;
		Plane pzmax;
		//法線の値を入力
		pxmin.normal = { 1,0,0 };
		pxmax.normal = { 1,0,0 };
		pymin.normal = { 0,1,0 };
		pymax.normal = { 0,1,0 };
		pzmin.normal = { 0,0,1 };
		pzmax.normal = { 0,0,1 };
		//距離の値を入力
		pxmin.distance = Dot(aabb.min, pxmin.normal);
		pymin.distance = Dot(aabb.min, pymin.normal);
		pzmin.distance = Dot(aabb.min, pzmin.normal);
		pxmax.distance = Dot(aabb.max, pxmax.normal);
		pymax.distance = Dot(aabb.max, pymax.normal);
		pzmax.distance = Dot(aabb.max, pzmax.normal);
		//それぞれの平面と線分の衝突点を求める
		if (IsCollision(ray, pxmin)) {
			Vector3 cp = CollisionPoint(ray, pxmin);
			if (
				cp.x >= aabb.min.x && cp.x <= aabb.max.x &&
				cp.y >= aabb.min.y && cp.y <= aabb.max.y &&
				cp.z >= aabb.min.z && cp.z <= aabb.max.z
				) {
				return true;
			}
		}
		if (IsCollision(ray, pxmax)) {
			Vector3 cp = CollisionPoint(ray, pxmax);
			if (
				cp.x >= aabb.min.x && cp.x <= aabb.max.x &&
				cp.y >= aabb.min.y && cp.y <= aabb.max.y &&
				cp.z >= aabb.min.z && cp.z <= aabb.max.z
				) {
				return true;
			}
		}
		if (IsCollision(ray, pymin)) {
			Vector3 cp = CollisionPoint(ray, pymin);
			if (
				cp.x >= aabb.min.x && cp.x <= aabb.max.x &&
				cp.y >= aabb.min.y && cp.y <= aabb.max.y &&
				cp.z >= aabb.min.z && cp.z <= aabb.max.z
				) {
				return true;
			}
		}
		if (IsCollision(ray, pymax)) {
			Vector3 cp = CollisionPoint(ray, pymax);
			if (
				cp.x >= aabb.min.x && cp.x <= aabb.max.x &&
				cp.y >= aabb.min.y && cp.y <= aabb.max.y &&
				cp.z >= aabb.min.z && cp.z <= aabb.max.z
				) {
				return true;
			}
		}
		if (IsCollision(ray, pzmin)) {
			Vector3 cp = CollisionPoint(ray, pzmin);
			if (
				cp.x >= aabb.min.x && cp.x <= aabb.max.x &&
				cp.y >= aabb.min.y && cp.y <= aabb.max.y &&
				cp.z >= aabb.min.z && cp.z <= aabb.max.z
				) {
				return true;
			}
		}
		if (IsCollision(ray, pzmax)) {
			Vector3 cp = CollisionPoint(ray, pzmax);
			if (
				cp.x >= aabb.min.x && cp.x <= aabb.max.x &&
				cp.y >= aabb.min.y && cp.y <= aabb.max.y &&
				cp.z >= aabb.min.z && cp.z <= aabb.max.z
				) {
				return true;
			}
		}
		//これ以上衝突条件は無い
		return false;
	}

	bool MyMath::IsCollision(const Ray& ray, const AABB& aabb) {
		return IsCollision(aabb, ray);
	}

	bool MyMath::IsCollision(const AABB& aabb, const Segment& segment) {
		//segmentが点の場合
		if (segment.diff.x == 0 && segment.diff.y == 0 && segment.diff.z == 0) {
			//点がAABB内にあるかチェック
			if (IsCollision(aabb, segment.origin)) {
				return true;
			}
			return false;
		}

		//AABBの6つの平面を作成
		Plane planes[6];

		//法線のセット
		planes[0].normal = { -1, 0, 0 };
		planes[1].normal = { 1, 0, 0 };
		planes[2].normal = { 0, -1, 0 };
		planes[3].normal = { 0, 1, 0 };
		planes[4].normal = { 0, 0, -1 };
		planes[5].normal = { 0, 0, 1 };

		//平面の距離をセット
		planes[0].distance = Dot(aabb.min, planes[0].normal);
		planes[1].distance = Dot(aabb.max, planes[1].normal);
		planes[2].distance = Dot(aabb.min, planes[2].normal);
		planes[3].distance = Dot(aabb.max, planes[3].normal);
		planes[4].distance = Dot(aabb.min, planes[4].normal);
		planes[5].distance = Dot(aabb.max, planes[5].normal);

		//各平面との衝突判定
		for (int i = 0; i < 6; ++i) {
			if (IsCollision(segment, planes[i])) {
				Vector3 cp = CollisionPoint(segment, planes[i]);
				//衝突点がAABBの範囲内にあるかチェック
				if (cp.x >= aabb.min.x && cp.x <= aabb.max.x &&
					cp.y >= aabb.min.y && cp.y <= aabb.max.y &&
					cp.z >= aabb.min.z && cp.z <= aabb.max.z) {
					return true;
				}
			}
		}

		//線分の両端点のいずれかがAABB内にある場合も衝突と判定
		Vector3 endPoint = segment.origin + segment.diff;
		bool originInside =
			segment.origin.x >= aabb.min.x && segment.origin.x <= aabb.max.x &&
			segment.origin.y >= aabb.min.y && segment.origin.y <= aabb.max.y &&
			segment.origin.z >= aabb.min.z && segment.origin.z <= aabb.max.z;

		bool endPointInside =
			endPoint.x >= aabb.min.x && endPoint.x <= aabb.max.x &&
			endPoint.y >= aabb.min.y && endPoint.y <= aabb.max.y &&
			endPoint.z >= aabb.min.z && endPoint.z <= aabb.max.z;

		if (originInside || endPointInside) {
			return true;
		}

		//衝突なし
		return false;
	}


	bool MyMath::IsCollision(const Segment& segment, const AABB& aabb) {
		return IsCollision(aabb, segment);
	}

	bool MyMath::IsCollision(const AABB& aabb, const OBB& obb) {
		//ブロードフェーズ処理
		{
			Vector3 aabbCenter = (aabb.min + aabb.max) * 0.5f;
			Vector3 aabbExtents = (aabb.max - aabb.min) * 0.5f;

			float aabbRadius = Length(aabbExtents);
			float obbRadius = Length(obb.size);

			Vector3 diff = aabbCenter - obb.center;
			float distSq = Dot(diff, diff);

			float radiusSum = aabbRadius + obbRadius;
			if (distSq > radiusSum * radiusSum) {
				return false;
			}
		}

		//ワールド行列
		Matrix4x4 obbWorldMatrix = {
			obb.orientations[0].x, obb.orientations[0].y, obb.orientations[0].z, 0,
			obb.orientations[1].x, obb.orientations[1].y, obb.orientations[1].z, 0,
			obb.orientations[2].x, obb.orientations[2].y, obb.orientations[2].z, 0,
			obb.center.x, obb.center.y, obb.center.z, 1
		};
		//ワールド逆行列
		Matrix4x4 obbWorldMatrixInverse = Inverse(obbWorldMatrix);

		//AABBの8頂点
		Vector3 vertices[8] = {
			{aabb.min.x, aabb.min.y, aabb.min.z},
			{aabb.max.x, aabb.min.y, aabb.min.z},
			{aabb.min.x, aabb.max.y, aabb.min.z},
			{aabb.max.x, aabb.max.y, aabb.min.z},
			{aabb.min.x, aabb.min.y, aabb.max.z},
			{aabb.max.x, aabb.min.y, aabb.max.z},
			{aabb.min.x, aabb.max.y, aabb.max.z},
			{aabb.max.x, aabb.max.y, aabb.max.z}
		};

		//OBBローカル空間に変換
		Vector3 transformed[8];
		for (int i = 0; i < 8; ++i) {
			transformed[i] = Transform(vertices[i], obbWorldMatrixInverse);
		}

		//変換後のAABB(min/max)を計算
		Vector3 newMin = transformed[0];
		Vector3 newMax = transformed[0];
		for (int i = 1; i < 8; ++i) {
			newMin.x = std::min(newMin.x, transformed[i].x);
			newMin.y = std::min(newMin.y, transformed[i].y);
			newMin.z = std::min(newMin.z, transformed[i].z);

			newMax.x = std::max(newMax.x, transformed[i].x);
			newMax.y = std::max(newMax.y, transformed[i].y);
			newMax.z = std::max(newMax.z, transformed[i].z);
		}

		AABB aabbInOBBLocalSpace = { newMin, newMax };
		//OBBのローカル空間におけるOBB
		AABB aabbOBBLoacalSpace = {
			.min = -obb.size,
			.max = obb.size
		};

		//AABBどうしの当たり判定をとる
		return IsCollision(aabbInOBBLocalSpace, aabbOBBLoacalSpace);
	}

	bool MyMath::IsCollision(const OBB& obb, const AABB& aabb) {
		return IsCollision(aabb, obb);
	}

	bool MyMath::IsCollision(const AABB& aabb, const Capsule& capsule) {
		//AABBを半径分膨張
		AABB expandedAABB;
		expandedAABB.min = aabb.min - Vector3{ capsule.radius, capsule.radius, capsule.radius };
		expandedAABB.max = aabb.max + Vector3{ capsule.radius, capsule.radius, capsule.radius };

		return IsCollision(expandedAABB, capsule.segment);
	}

	bool MyMath::IsCollision(const Capsule& capsule, const AABB& aabb) {
		return IsCollision(aabb, capsule);
	}

	bool MyMath::IsCollision(const OBB& obb1, const OBB& obb2) {
		//ブロードフェーズ処理
		{
			// 中心間ベクトル
			Vector3 diff = obb2.center - obb1.center;
			// 中心間距離の2乗
			float centerDistSq = Dot(diff, diff);
			// 各OBBの最大半径（中心→最遠頂点）
			float r1 = Length(obb1.size);
			float r2 = Length(obb2.size);
			// 半径合計
			float radiusSum = r1 + r2;
			// 半径合計の2乗
			float radiusSumSq = radiusSum * radiusSum;
			// 明らかに離れているなら即終了
			if (centerDistSq > radiusSumSq) {
				return false;
			}
		}

		//各OBBの頂点をローカル座標で定義
		Vector3 obb1Vertex[8] = {
			{ -obb1.size.x, obb1.size.y, -obb1.size.z },
			{ obb1.size.x, obb1.size.y, -obb1.size.z },
			{ -obb1.size.x, -obb1.size.y, -obb1.size.z },
			{ obb1.size.x, -obb1.size.y, -obb1.size.z },
			{ -obb1.size.x, obb1.size.y, obb1.size.z },
			{ obb1.size.x, obb1.size.y, obb1.size.z },
			{ -obb1.size.x, -obb1.size.y, obb1.size.z },
			{ obb1.size.x, -obb1.size.y, obb1.size.z },
		};

		Vector3 obb2Vertex[8] = {
			{ -obb2.size.x, obb2.size.y, -obb2.size.z },
			{ obb2.size.x, obb2.size.y, -obb2.size.z },
			{ -obb2.size.x, -obb2.size.y, -obb2.size.z },
			{ obb2.size.x, -obb2.size.y, -obb2.size.z },
			{ -obb2.size.x, obb2.size.y, obb2.size.z },
			{ obb2.size.x, obb2.size.y, obb2.size.z },
			{ -obb2.size.x, -obb2.size.y, obb2.size.z },
			{ obb2.size.x, -obb2.size.y, obb2.size.z },
		};

		//ワールド行列
		Matrix4x4 obb1WorldMatrix = {
			obb1.orientations[0].x, obb1.orientations[0].y, obb1.orientations[0].z, 0,
			obb1.orientations[1].x, obb1.orientations[1].y, obb1.orientations[1].z, 0,
			obb1.orientations[2].x, obb1.orientations[2].y, obb1.orientations[2].z, 0,
			obb1.center.x, obb1.center.y, obb1.center.z, 1
		};
		Matrix4x4 obb2WorldMatrix = {
			obb2.orientations[0].x, obb2.orientations[0].y, obb2.orientations[0].z, 0,
			obb2.orientations[1].x, obb2.orientations[1].y, obb2.orientations[1].z, 0,
			obb2.orientations[2].x, obb2.orientations[2].y, obb2.orientations[2].z, 0,
			obb2.center.x, obb2.center.y, obb2.center.z, 1
		};

		//各頂点をワールド座標に変換
		Vector3 obb1WorldVertex[8];
		Vector3 obb2WorldVertex[8];
		for (int i = 0; i < 8; i++) {
			obb1WorldVertex[i] = Transform(obb1Vertex[i], obb1WorldMatrix);
			obb2WorldVertex[i] = Transform(obb2Vertex[i], obb2WorldMatrix);
		}

		//分離軸の定義
		Vector3 axes[15] = {
			obb1.orientations[0], obb1.orientations[1], obb1.orientations[2],
			obb2.orientations[0], obb2.orientations[1], obb2.orientations[2],
			Cross(obb1.orientations[0], obb2.orientations[0]),
			Cross(obb1.orientations[0], obb2.orientations[1]),
			Cross(obb1.orientations[0], obb2.orientations[2]),
			Cross(obb1.orientations[1], obb2.orientations[0]),
			Cross(obb1.orientations[1], obb2.orientations[1]),
			Cross(obb1.orientations[1], obb2.orientations[2]),
			Cross(obb1.orientations[2], obb2.orientations[0]),
			Cross(obb1.orientations[2], obb2.orientations[1]),
			Cross(obb1.orientations[2], obb2.orientations[2]),
		};

		//各分離軸について投影範囲を計算し、重なりを確認
		for (const auto& axis : axes) {
			if (axis == Vector3{ 0, 0, 0 }) continue; //ゼロベクトルは無視

			Vector3 normalizedAxis = Normalize(axis);

			auto [min1, max1] = ProjectOntoAxis(obb1WorldVertex, 8, normalizedAxis);
			auto [min2, max2] = ProjectOntoAxis(obb2WorldVertex, 8, normalizedAxis);

			if (max1 < min2 || max2 < min1) {
				return false; //分離軸が見つかった → 衝突していない
			}
		}

		return true; //すべての軸で重なりあり → 衝突している
	}

	bool MyMath::IsCollision(const OBB& obb, const Sphere& sphere) {
		//ワールド行列
		Matrix4x4 obbWorldMatrix = {
			obb.orientations[0].x, obb.orientations[0].y, obb.orientations[0].z, 0,
			obb.orientations[1].x, obb.orientations[1].y, obb.orientations[1].z, 0,
			obb.orientations[2].x, obb.orientations[2].y, obb.orientations[2].z, 0,
			obb.center.x, obb.center.y, obb.center.z, 1
		};
		//ワールド逆行列
		Matrix4x4 obbWorldMatrixInverse = Inverse(obbWorldMatrix);

		Vector3 centerInOBBLocalSpace = Transform(sphere.center, obbWorldMatrixInverse);
		AABB aabbOBBLocal = {
			.min = -obb.size,
			.max = obb.size
		};
		Sphere sphereOBBLocal = { centerInOBBLocalSpace, sphere.radius };

		//ローカル空間で衝突判定
		return IsCollision(aabbOBBLocal, sphereOBBLocal);
	}

	bool MyMath::IsCollision(const Sphere& sphere, const OBB& obb) {
		return IsCollision(obb, sphere);
	}

	bool MyMath::IsCollision(const OBB& obb, const Line& line) {
		//ワールド行列
		Matrix4x4 obbWorldMatrix = {
			obb.orientations[0].x, obb.orientations[0].y, obb.orientations[0].z, 0,
			obb.orientations[1].x, obb.orientations[1].y, obb.orientations[1].z, 0,
			obb.orientations[2].x, obb.orientations[2].y, obb.orientations[2].z, 0,
			obb.center.x, obb.center.y, obb.center.z, 1
		};
		//ワールド逆行列
		Matrix4x4 obbWorldMatrixInverse = Inverse(obbWorldMatrix);


		Vector3 centerInOBBLocalLine = Transform(line.origin, obbWorldMatrixInverse);
		AABB aabbOBBLocal = {
			.min = -obb.size,
			.max = obb.size
		};
		Line lineOBBLocal = { centerInOBBLocalLine, line.diff };

		//ローカル空間で衝突判定
		return IsCollision(aabbOBBLocal, lineOBBLocal);
	}

	bool MyMath::IsCollision(const Line& line, const OBB& obb) {
		return IsCollision(obb, line);
	}

	bool MyMath::IsCollision(const OBB& obb, const Ray& ray) {
		//ワールド行列
		Matrix4x4 obbWorldMatrix = {
			obb.orientations[0].x, obb.orientations[0].y, obb.orientations[0].z, 0,
			obb.orientations[1].x, obb.orientations[1].y, obb.orientations[1].z, 0,
			obb.orientations[2].x, obb.orientations[2].y, obb.orientations[2].z, 0,
			obb.center.x, obb.center.y, obb.center.z, 1
		};
		//ワールド逆行列
		Matrix4x4 obbWorldMatrixInverse = Inverse(obbWorldMatrix);


		Vector3 centerInOBBLocalRay = Transform(ray.origin, obbWorldMatrixInverse);
		AABB aabbOBBLocal = {
			.min = -obb.size,
			.max = obb.size
		};
		Ray rayOBBLocal = { centerInOBBLocalRay, ray.diff };

		//ローカル空間で衝突判定
		return IsCollision(aabbOBBLocal, rayOBBLocal);
	}

	bool MyMath::IsCollision(const Ray& ray, const OBB& obb) {
		return IsCollision(obb, ray);
	}

	bool MyMath::IsCollision(const OBB& obb, const Segment& segment) {
		//ワールド行列
		Matrix4x4 obbWorldMatrix = {
			obb.orientations[0].x, obb.orientations[0].y, obb.orientations[0].z, 0,
			obb.orientations[1].x, obb.orientations[1].y, obb.orientations[1].z, 0,
			obb.orientations[2].x, obb.orientations[2].y, obb.orientations[2].z, 0,
			obb.center.x, obb.center.y, obb.center.z, 1
		};
		//ワールド逆行列
		Matrix4x4 obbWorldMatrixInverse = Inverse(obbWorldMatrix);

		Vector3 centerInOBBLocalSegment = Transform(segment.origin, obbWorldMatrixInverse);
		Vector3 diffInOBBLocalSegment = TransformNormal(segment.diff, obbWorldMatrixInverse);
		AABB aabbOBBLocal = {
			.min = -obb.size,
			.max = obb.size
		};
		Segment segmentOBBLocal = { centerInOBBLocalSegment, diffInOBBLocalSegment };

		//ローカル空間で衝突判定
		return IsCollision(aabbOBBLocal, segmentOBBLocal);
	}

	bool MyMath::IsCollision(const Segment& segment, const OBB& obb) {
		return IsCollision(obb, segment);
	}

	bool MyMath::IsCollision(const OBB& obb, const Capsule& capsule) {
		//ブロードフェーズ処理
		{
			// OBBの最大半径
			float obbRadius = Length(obb.size);
			// カプセルの半径 + 半分の線分長
			float capsuleHalfLen = Length(capsule.segment.diff) * 0.5f;
			float capsuleRadius = capsule.radius + capsuleHalfLen;
			// カプセル中心
			Vector3 capsuleCenter = capsule.segment.origin + capsule.segment.diff * 0.5f;
			// 中心距離
			Vector3 diff = capsuleCenter - obb.center;
			float distSq = Dot(diff, diff);
			float radiusSum = obbRadius + capsuleRadius;

			if (distSq > radiusSum * radiusSum) {
				return false;
			}
		}


		//ある座標がOBB内にあるか判定するラムダ式
		auto isPointInOBB = [&](const Vector3& point) -> bool {
			Vector3 d = point - obb.center;
			for (int i = 0; i < 3; ++i) {
				float dist = Dot(d, obb.orientations[i]);
				float halfLength = (i == 0) ? obb.size.x
					: (i == 1) ? obb.size.y
					: obb.size.z;
				if (std::fabs(dist) > halfLength) {
					return false; // どれか1軸でも範囲外なら外部
				}
			}
			return true;
			};
		//OBB→三角形平面に分解するラムダ式
		auto makeOBBTriangles = [&]() -> std::vector<Triangle> {
			std::vector<Vector3> axes = {
				obb.orientations[0] * obb.size.x,
				obb.orientations[1] * obb.size.y,
				obb.orientations[2] * obb.size.z
			};

			// 8頂点生成（中心±各軸の組み合わせ）
			std::vector<Vector3> v(8);
			v[0] = obb.center + axes[0] + axes[1] + axes[2];
			v[1] = obb.center + axes[0] + axes[1] - axes[2];
			v[2] = obb.center + axes[0] - axes[1] + axes[2];
			v[3] = obb.center + axes[0] - axes[1] - axes[2];
			v[4] = obb.center - axes[0] + axes[1] + axes[2];
			v[5] = obb.center - axes[0] + axes[1] - axes[2];
			v[6] = obb.center - axes[0] - axes[1] + axes[2];
			v[7] = obb.center - axes[0] - axes[1] - axes[2];

			// 各面を三角形に分割
			std::vector<Triangle> tris;

			auto addFace = [&](int a, int b, int c, int d) {
				tris.push_back({ v[a], v[b], v[c] });
				tris.push_back({ v[a], v[c], v[d] });
				};

			// +X, -X, +Y, -Y, +Z, -Z の6面
			addFace(0, 1, 3, 2); // +X面
			addFace(4, 6, 7, 5); // -X面
			addFace(0, 4, 5, 1); // +Y面
			addFace(2, 3, 7, 6); // -Y面
			addFace(0, 2, 6, 4); // +Z面
			addFace(1, 5, 7, 3); // -Z面

			return tris;
			};


		//カプセル内線分がOBB内にあるかの判定処理
		Vector3 start = capsule.segment.origin;
		Vector3 end = capsule.segment.origin + capsule.segment.diff;

		bool startInside = isPointInOBB(start);
		bool endInside = isPointInOBB(end);

		if (startInside && endInside) {
			return true; // カプセル線分がOBB内に完全に含まれる
		}

		//OBBを三角形に分解
		auto triangles = makeOBBTriangles();

		//各三角形とカプセルの当たり判定
		for (const auto& tri : triangles) {
			if (IsCollision(capsule, tri)) {
				return true;
			}
		}

		//どの面とも衝突していないのでfalse
		return false;

	}

	bool MyMath::IsCollision(const Capsule& capsule, const OBB& obb) {
		return IsCollision(obb, capsule);
	}

	bool MyMath::IsCollision(const Capsule& capsule1, const Capsule& capsule2) {
		//ブロードフェーズ処理
		{
			// カプセル1
			Vector3 center1 = capsule1.segment.origin + capsule1.segment.diff * 0.5f;
			float halfLen1 = Length(capsule1.segment.diff) * 0.5f;
			float broadRadius1 = halfLen1 + capsule1.radius;
			// カプセル2
			Vector3 center2 = capsule2.segment.origin + capsule2.segment.diff * 0.5f;
			float halfLen2 = Length(capsule2.segment.diff) * 0.5f;
			float broadRadius2 = halfLen2 + capsule2.radius;
			// 中心距離
			Vector3 diffCenter = center1 - center2;
			float centerDistSq = Dot(diffCenter, diffCenter);
			// 半径合計
			float broadRadiusSum = broadRadius1 + broadRadius2;
			// 明らかに離れているなら終了
			if (centerDistSq > broadRadiusSum * broadRadiusSum) {
				return false;
			}
		}

		//線分同士の最短距離（二乗）を求めるラムダ式
		auto SegmentSegmentDistSq = [&](const Segment& seg1, const Segment& seg2) -> float {
			Vector3 p1 = seg1.origin;
			Vector3 q1 = seg1.origin + seg1.diff;
			Vector3 p2 = seg2.origin;
			Vector3 q2 = seg2.origin + seg2.diff;

			Vector3 d1 = q1 - p1;
			Vector3 d2 = q2 - p2;
			Vector3 r = p1 - p2;

			float a = Dot(d1, d1);
			float e = Dot(d2, d2);
			float f = Dot(d2, r);

			float s, t;

			if (a <= 1e-6f && e <= 1e-6f) {
				return Dot(r, r); //両方の線分が点
			}
			if (a <= 1e-6f) {
				s = 0.0f;
				t = std::clamp(f / e, 0.0f, 1.0f);
			}
			else {
				float c = Dot(d1, r);
				if (e <= 1e-6f) {
					t = 0.0f;
					s = std::clamp(-c / a, 0.0f, 1.0f);
				}
				else {
					float b = Dot(d1, d2);
					float denom = a * e - b * b;

					if (denom != 0.0f) {
						s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
					}
					else {
						s = 0.0f;
					}

					t = (b * s + f) / e;

					if (t < 0.0f) {
						t = 0.0f;
						s = std::clamp(-c / a, 0.0f, 1.0f);
					}
					else if (t > 1.0f) {
						t = 1.0f;
						s = std::clamp((b - c) / a, 0.0f, 1.0f);
					}
				}
			}

			Vector3 c1 = p1 + d1 * s;
			Vector3 c2 = p2 + d2 * t;
			Vector3 diff = c1 - c2;

			return Dot(diff, diff); //最短距離の二乗
			};

		//半径の合計
		float radiusSum = capsule1.radius + capsule2.radius;

		//最短距離の二乗を求める
		float distSq = SegmentSegmentDistSq(capsule1.segment, capsule2.segment);

		//衝突しているかを判定
		return distSq <= radiusSum * radiusSum;
	}

	void MyMath::CreateLineSphere(const Sphere& sphere, Vector4 color, uint32_t subdivision) {
		const auto lineManager = LineManager::GetInstance();
		const uint32_t kSubdivision = subdivision;//分割数
		const float kLonEvery = 2.0f * pi / kSubdivision;//経度分割1つ分の角度
		const float kLatEvery = pi / kSubdivision;//緯度分割1つ分の角度
		//緯度の方向に分割 -π/2~π/2
		for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
			float lat = -pi / 2.0f + kLatEvery * latIndex;//現在の経度
			//経度の方向に分割 -π/2~π/2
			for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
				float lon = lonIndex * kLonEvery;//現在の緯度
				//world座標系でのa,b,cを求める
				Vector3 a, b, c;
				a = {
					sphere.center.x + sphere.radius * cosf(lat) * cosf(lon),
					sphere.center.y + sphere.radius * sinf(lat),
					sphere.center.z + sphere.radius * cosf(lat) * sinf(lon)
				};
				b = {
					sphere.center.x + sphere.radius * cosf(kLatEvery + lat) * cosf(lon),
					sphere.center.y + sphere.radius * sinf(kLatEvery + lat),
					sphere.center.z + sphere.radius * cosf(kLatEvery + lat) * sinf(lon)
				};
				c = {
					sphere.center.x + sphere.radius * cosf(lat) * cosf(kLonEvery + lon),
					sphere.center.y + sphere.radius * sinf(lat),
					sphere.center.z + sphere.radius * cosf(lat) * sinf(kLonEvery + lon)
				};
				//描画
				lineManager->CreateLine(a, b, color);
				lineManager->CreateLine(a, c, color);
			}
		}
	}

	void MyMath::CreateLineAABB(const AABB& aabb, Vector4 color) {
		const auto lineManager = LineManager::GetInstance();
		//8頂点を計算
		Vector3 v[8] = {
			{ aabb.min.x, aabb.min.y, aabb.min.z }, //0
			{ aabb.max.x, aabb.min.y, aabb.min.z }, //1
			{ aabb.max.x, aabb.max.y, aabb.min.z }, //2
			{ aabb.min.x, aabb.max.y, aabb.min.z }, //3
			{ aabb.min.x, aabb.min.y, aabb.max.z }, //4
			{ aabb.max.x, aabb.min.y, aabb.max.z }, //5
			{ aabb.max.x, aabb.max.y, aabb.max.z }, //6
			{ aabb.min.x, aabb.max.y, aabb.max.z }  //7
		};

		//12本の辺を描画（線で結ぶ）
		const uint32_t edgeIndices[12][2] = {
			{0, 1}, {1, 2}, {2, 3}, {3, 0}, //底面
			{4, 5}, {5, 6}, {6, 7}, {7, 4}, //上面
			{0, 4}, {1, 5}, {2, 6}, {3, 7}  //側面
		};

		for (uint32_t i = 0; i < 12; ++i) {
			const Vector3& from = v[edgeIndices[i][0]];
			const Vector3& to = v[edgeIndices[i][1]];
			lineManager->CreateLine(from, to, color);
		}
	}

	void MyMath::CreateLineOBB(const OBB& obb, Vector4 color) {
		const auto lineManager = LineManager::GetInstance();

		// ローカル座標系での8頂点
		Vector3 localVertex[8] = {
			{ -obb.size.x, -obb.size.y, -obb.size.z }, // 0
			{ +obb.size.x, -obb.size.y, -obb.size.z }, // 1
			{ +obb.size.x, +obb.size.y, -obb.size.z }, // 2
			{ -obb.size.x, +obb.size.y, -obb.size.z }, // 3
			{ -obb.size.x, -obb.size.y, +obb.size.z }, // 4
			{ +obb.size.x, -obb.size.y, +obb.size.z }, // 5
			{ +obb.size.x, +obb.size.y, +obb.size.z }, // 6
			{ -obb.size.x, +obb.size.y, +obb.size.z }  // 7
		};

		// ワールド座標に変換
		Vector3 worldVertex[8];
		for (int i = 0; i < 8; i++) {
			// OBBのローカル→ワールド変換
			// center + local * orientations
			worldVertex[i] =
				obb.center
				+ obb.orientations[0] * localVertex[i].x
				+ obb.orientations[1] * localVertex[i].y
				+ obb.orientations[2] * localVertex[i].z;
		}

		// AABB と同じ線の結び方
		const uint32_t edgeIndices[12][2] = {
			{0, 1}, {1, 2}, {2, 3}, {3, 0}, // 底面
			{4, 5}, {5, 6}, {6, 7}, {7, 4}, // 上面
			{0, 4}, {1, 5}, {2, 6}, {3, 7}  // 側面
		};

		for (uint32_t i = 0; i < 12; ++i) {
			const Vector3& from = worldVertex[edgeIndices[i][0]];
			const Vector3& to = worldVertex[edgeIndices[i][1]];
			lineManager->CreateLine(from, to, color);
		}
	}

	float MyMath::DistancePointToPlane(const Vector3& point, const Plane& plane) {
		//点と平面の最近接点を求める
		Vector3 closestPoint = ClosestPoint(point, plane);

		//長さを返す
		return Vector3(point - closestPoint).Length();
	}

	float MyMath::DistancePointToSegment(const Vector3& point, const Segment& segment) {
		//点と線分の最近接点を求める
		Vector3 closestPoint = ClosestPoint(point, segment);

		//長さを返す
		return Vector3(point - closestPoint).Length();
	}

	float MyMath::DistanceSegmentToSegment(const Segment& s1, const Segment& s2) {
		Vector3 p1 = s1.origin;
		Vector3 q1 = s1.origin + s1.diff;
		Vector3 p2 = s2.origin;
		Vector3 q2 = s2.origin + s2.diff;

		//各線分の方向ベクトル
		Vector3 u = q1 - p1; //s1の方向ベクトル
		Vector3 v = q2 - p2; //s2の方向ベクトル
		Vector3 w = p1 - p2; //p1とp2の差

		//便利変数
		float a = Dot(u, u); //s1の長さの2乗	
		float b = Dot(u, v);
		float c = Dot(v, v); //s2の長さの2乗
		float d = Dot(u, w);
		float e = Dot(v, w);

		float denom = a * c - b * b; //直線の場合の分母

		float s = 0.0f; //パラメータ s (s1 上の位置: p1 + s * u), 期待範囲 [0,1]
		float t = 0.0f; //パラメータ t (s2 上の位置: p2 + t * v), 期待範囲 [0,1]

		//まずは直線（無制約）上の解を計算（denom が十分大きければ）
		if (denom > epsilon) {
			//二本の非平行線の最近接点パラメータ（直線同士）
			s = (b * e - c * d) / denom;
			t = (a * e - b * d) / denom;
		}
		else {
			//平行（またはほぼ平行）: 任意に s = 0 として t を求めるのが一つの選択肢
			//このとき t は v に沿った p1 の射影
			s = 0.0f;
			if (c > epsilon) {
				t = e / c;
			}
			else {
				//両方ともほぼゼロ長（退化）：どちらも点に近い
				t = 0.0f;
			}
		}

		//s, t を [0,1] に制約する（clamp）。ただし一方だけクランプされた場合は、もう一方を再計算して調整する。
		//1) s をクランプしてから t を再計算
		s = std::clamp(s, 0.0f, 1.0f);
		if (c > epsilon) {
			// t = (b * s + e) / c は、s を固定したときの s2 側パラメータの最小化解
			t = (b * s + e) / c;
			t = std::clamp(t, 0.0f, 1.0f);
		}
		else {
			t = 0.0f;
		}

		//2) t をクランプした結果に基づき s を再計算（もし t がクランプされているなら）
		if (a > epsilon) {
			// s = (b * t - d) / a は、t を固定したときの s1 側パラメータの最小化解
			float sCandidate = (b * t - d) / a;
			sCandidate = std::clamp(sCandidate, 0.0f, 1.0f);

			//もし sCandidate が現在の s と異なる（つまり t のクランプにより s が変わるべきなら）
			//再代入して、t を再計算して最終調整
			if (fabsf(sCandidate - s) > epsilon) {
				s = sCandidate;
				if (c > epsilon) {
					t = (b * s + e) / c;
					t = std::clamp(t, 0.0f, 1.0f);
				}
				else {
					t = 0.0f;
				}
			}
		}

		//最近接点
		Vector3 closestOnS1 = p1 + s * u;
		Vector3 closestOnS2 = p2 + t * v;

		//距離を返す
		float distance = Length(closestOnS1 - closestOnS2);
		return distance;
	}

	Vector3 MyMath::ClosestPoint(const Vector3& point, const Plane& plane) {
		float distance = Dot(plane.normal, point) - plane.distance;
		return point - distance * plane.normal;
	}

	Vector3 MyMath::ClosestPoint(const Vector3& point, const Triangle& tri) {
		//三角形を含む平面を作成
		Plane plane = MakePlane(tri);
		//点を平面上に射影
		Vector3 projected = ClosestPoint(point, plane);
		//射影点が三角形内部かチェック
		if (IsPointInTriangle(projected, tri)) {
			return projected;	//内側にある場合はそのまま返す
		}
		//三角形の各辺上の最近接点を求めて、一番近い点を探す
		Vector3 closest;
		//三角形から3つの線分を取り出す
		Segment s1 = {
		.origin = tri.vertices[0],
		.diff = tri.vertices[1] - tri.vertices[0]
		};
		Segment s2 = {
		.origin = tri.vertices[1],
		.diff = tri.vertices[2] - tri.vertices[1]
		};
		Segment s3 = {
		.origin = tri.vertices[2],
		.diff = tri.vertices[0] - tri.vertices[2]
		};
		//ひとつずつ求めて一番短いやつを使う
		Vector3 c1 = ClosestPoint(point, s1);
		Vector3 c2 = ClosestPoint(point, s2);
		Vector3 c3 = ClosestPoint(point, s3);
		//一番近い点を選ぶ
		float d1 = Length(point - c1);
		float d2 = Length(point - c2);
		float d3 = Length(point - c3);

		closest = c1;
		float minDist = d1;

		if (d2 < minDist) {
			minDist = d2;
			closest = c2;
		}
		if (d3 < minDist) {
			minDist = d3;
			closest = c3;
		}

		//最も近い点を返す
		return closest;
	}

	Vector3 MyMath::ClosestPoint(const Vector3& point, const Segment& segment) {
		// diffが0ベクトル（始点＝終点）の場合
		if (segment.diff.x == 0.0f && segment.diff.y == 0.0f && segment.diff.z == 0.0f) {
			return segment.origin;
		}

		Vector3 a = point - segment.origin;
		float t = Dot(a, segment.diff) / Dot(segment.diff, segment.diff);

		// 線分の範囲 [0,1] にクランプ
		t = std::clamp(t, 0.0f, 1.0f);

		// 始点 + t * diff が最近接点
		Vector3 cp = segment.origin + segment.diff * t;
		return cp;
	}

	Vector3 MyMath::ClosestPoint(const Vector3& point, const AABB& aabb) {
		Vector3 closestPoint{
			std::clamp(point.x,aabb.min.x,aabb.max.x),
			std::clamp(point.y,aabb.min.y,aabb.max.y),
			std::clamp(point.z,aabb.min.z,aabb.max.z)
		};
		return closestPoint;
	}

	Plane MyMath::MakePlane(const Triangle& tri) {
		//三角形のある面を作る
		Plane plane;
		//三角形の座標から、法線nを求める
		Vector3 vv1, vv2, n;
		vv1 = Subtract(tri.vertices[1], tri.vertices[0]);
		vv2 = Subtract(tri.vertices[2], tri.vertices[1]);
		n = Normalize(Cross(vv1, vv2));
		//距離を求める
		float d = Dot(tri.vertices[0], n);
		//面に変換
		plane.normal = n;
		plane.distance = d;

		return plane;
	}

	bool MyMath::CheckParallel(const Segment& segment, const Plane& plane) {
		// 線分の方向ベクトル
		Vector3 dir = segment.diff;

		// 平面の法線ベクトル
		Vector3 normal = plane.normal;

		// 方向ベクトルを正規化（任意だが安定性のため推奨）
		dir = Normalize(dir);
		normal = Normalize(normal);

		// 平面の法線と線分の方向が垂直（内積がほぼ0）なら「平行」
		float dot = Dot(dir, normal);

		return fabsf(dot) < epsilon;
	}

	Segment MyMath::ProjectSegmentOntoPlane(const Segment& segment, const Plane& plane) {
		Segment projected;

		//始点と終点を求める
		Vector3 start = segment.origin;
		Vector3 end = segment.origin + segment.diff;

		//各点を平面に射影
		Vector3 projStart = ClosestPoint(start, plane);
		Vector3 projEnd = ClosestPoint(end, plane);

		//結果の線分をセット
		projected.origin = projStart;
		projected.diff = projEnd - projStart;

		return projected;
	}

	bool MyMath::IsPointInTriangle(const Vector3& point, const Triangle& tri) {
		const Vector3& a = tri.vertices[0];
		const Vector3& b = tri.vertices[1];
		const Vector3& c = tri.vertices[2];

		//三角形の法線
		Vector3 normal = Normalize(Cross(b - a, c - a));

		//各辺に対するクロス積
		Vector3 c0 = Cross(b - a, point - a);
		Vector3 c1 = Cross(c - b, point - b);
		Vector3 c2 = Cross(a - c, point - c);

		//全てのクロス積が法線と同じ向きなら内側
		if (Dot(c0, normal) >= 0.0f &&
			Dot(c1, normal) >= 0.0f &&
			Dot(c2, normal) >= 0.0f) {
			return true;
		}

		return false;
	}

	Vector3 MyMath::IntersectionSegmentToSegment(const Segment& s1, const Segment& s2) {
		const Vector3 p1 = s1.origin;
		const Vector3 p2 = s1.origin + s1.diff;
		const Vector3 q1 = s2.origin;
		const Vector3 q2 = s2.origin + s2.diff;

		const Vector3 u = p2 - p1; // s1方向ベクトル
		const Vector3 v = q2 - q1; // s2方向ベクトル
		const Vector3 w = p1 - q1;

		float a = Dot(u, u); // |u|^2
		float b = Dot(u, v);
		float c = Dot(v, v); // |v|^2
		float d = Dot(u, w);
		float e = Dot(v, w);

		float D = a * c - b * b; // 平行なら0

		float sc, tc;

		if (D < epsilon) {
			// ほぼ平行
			sc = 0.0f;
			tc = (b > c ? d / b : e / c);
		}
		else {
			sc = (b * e - c * d) / D;
			tc = (a * e - b * d) / D;
		}

		// パラメータを[0,1]にクランプ
		sc = std::clamp(sc, 0.0f, 1.0f);
		tc = std::clamp(tc, 0.0f, 1.0f);

		// 交点（どちらの線分を使っても同じはず）
		Vector3 pointOnS1 = p1 + u * sc;
		Vector3 pointOnS2 = q1 + v * tc;

		// 最短距離が0前提なら、両点はほぼ一致している
		return (pointOnS1 + pointOnS2) * 0.5f; // 安定化のため中点を採用
	}

	///------------------------------------///
	///      演算子のオーバーロード
	///------------------------------------///

	Vector2 operator+(const Vector2& v1, const Vector2& v2) {
		return MyMath::Add(v1, v2);
	}

	Vector2 operator-(const Vector2& v1, const Vector2& v2) {
		return MyMath::Subtract(v1, v2);
	}

	Vector2 operator*(float s, const Vector2& v) {
		return MyMath::Multiply(s, v);
	}

	Vector2 operator*(const Vector2& v, float s) {
		return s * v;
	}

	Vector2 operator/(const Vector2& v, float s) {
		return MyMath::Multiply(1.0f / s, v);
	}

	Vector3 operator+(const Vector3& v1, const Vector3& v2) {
		return MyMath::Add(v1, v2);
	}

	Vector3 operator-(const Vector3& v1, const Vector3& v2) {
		return MyMath::Subtract(v1, v2);
	}

	Vector3 operator*(float s, const Vector3& v) {
		return MyMath::Multiply(s, v);
	}

	Vector3 operator*(const Vector3& v, float s) {
		return s * v;
	}

	Vector3 operator/(const Vector3& v, float s) {
		return MyMath::Multiply(1.0f / s, v);
	}

	Vector3 operator*(const Matrix4x4& mat, const Vector3& vec) {
		//行列とベクトルの掛け算
		float x = mat.m[0][0] * vec.x + mat.m[0][1] * vec.y + mat.m[0][2] * vec.z + mat.m[0][3];
		float y = mat.m[1][0] * vec.x + mat.m[1][1] * vec.y + mat.m[1][2] * vec.z + mat.m[1][3];
		float z = mat.m[2][0] * vec.x + mat.m[2][1] * vec.y + mat.m[2][2] * vec.z + mat.m[2][3];
		return Vector3(x, y, z);
	}

	Vector3 operator*(const Quaternion& q, const Vector3& v) {
		//クォータニオンを正規化
		Quaternion normalized_q = q;
		normalized_q.normalize();

		//ベクトルをクォータニオンとして扱う
		Quaternion q_v(0, v.x, v.y, v.z);

		//クォータニオンの掛け算で回転操作: q * v * q^-1
		Quaternion rotated_q = MyMath::Multiply(MyMath::Multiply(normalized_q, q_v), normalized_q.conjugate());

		return Vector3(rotated_q.x, rotated_q.y, rotated_q.z);
	}


	Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) {
		return MyMath::Add(m1, m2);
	}

	Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) {
		return MyMath::Subtract(m1, m2);
	}

	Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) {
		return MyMath::Multiply(m1, m2);
	}

	Quaternion operator+(const Quaternion& q1, const Quaternion& q2) {
		return MyMath::Add(q1, q2);
	}

	Quaternion operator-(const Quaternion& q1, const Quaternion& q2) {
		return MyMath::Subtract(q1, q2);
	}

	Quaternion operator*(const Quaternion& q1, const Quaternion& q2) {
		return MyMath::Multiply(q1, q2);
	}

	Quaternion operator*(float s, const Quaternion& q) {
		return MyMath::Multiply(s, q);
	}

	Quaternion operator*(const Quaternion& q, float s) {
		return s * q;
	}

	Vector3 operator-(const Vector3& v) {
		return { -v.x,-v.y,-v.z };
	}

	Vector3 operator+(const Vector3& v) {
		return v;
	}

	Quaternion operator-(const Quaternion& q) {
		return { -q.x,-q.y,-q.z,-q.w };
	}

	Quaternion operator+(const Quaternion& q) {
		return q;
	}

}