#pragma once
#include <stdexcept>
#include <cmath>

namespace Norm {

	/// <summary>
	/// 3次元ベクトル
	/// </summary>
	struct Vector3 final {
		float x;
		float y;
		float z;
		/// <summary>
		/// 初期化リスト
		/// </summary>
		/// <param name="x">x</param>
		/// <param name="y">y</param>
		/// <param name="z">z</param>
		Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
		//複合代入演算子のオーバーロード
		Vector3& operator+=(const Vector3& other) {
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}
		Vector3& operator-=(const Vector3& other) {
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return *this;
		}
		Vector3& operator*=(float scalar) {
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}
		Vector3& operator/=(float scalar) {
			if (scalar == 0.0f)
				throw std::runtime_error("Division by zero");
			x /= scalar;
			y /= scalar;
			z /= scalar;
			return *this;
		}
		bool operator==(const Vector3& other) const {
			return x == other.x && y == other.y && z == other.z;
		}

		bool operator!=(const Vector3& other) const {
			return !(*this == other);
		}

		/// <summary>
		/// ベクトルの長さを返す
		/// </summary>
		/// <returns>ベクトルの長さ</returns>
		float Length() const {
			return std::sqrtf(x * x + y * y + z * z);
		}
		/// <summary>
		/// ベクトルの長さの2乗を返す
		/// </summary>
		/// <returns>ベクトルの長さの2乗</returns>
		float LengthSq() const {
			return x * x + y * y + z * z;
		}
		/// <summary>
		/// ベクトルを正規化した時の値を返す
		/// </summary>
		/// <returns>正規化したベクトルの値</returns>
		Vector3 Normalized() const {
			float len = Length();
			if (len == 0.0f) return Vector3(0.0f, 0.0f, 0.0f);
			return Vector3(x / len, y / len, z / len);
		}
		/// <summary>
		/// 自分自身を正規化して値を返す
		/// </summary>
		/// <returns>正規化したベクトルの値</returns>
		Vector3& Normalize() {
			float len = Length();
			if (len == 0.0f) {
				x = y = z = 0.0f;
				return *this;
			}
			x /= len;
			y /= len;
			z /= len;
			return *this;
		}
	};

}