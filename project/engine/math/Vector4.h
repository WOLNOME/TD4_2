#pragma once
#include <stdexcept>
#include <cmath>

namespace Norm {

	/// <summary>
	/// 4次元ベクトル
	/// </summary>
	struct Vector4 final {
		float x;
		float y;
		float z;
		float w;
		/// <summary>
		/// 初期化リスト
		/// </summary>
		/// <param name="x">x</param>
		/// <param name="y">y</param>
		/// <param name="z">z</param>
		/// <param name="w">w</param>
		Vector4(float x = 0, float y = 0, float z = 0, float w = 0) : x(x), y(y), z(z), w(w) {}
		//複合代入演算子のオーバーロード
		Vector4& operator+=(const Vector4& other) {
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
			return *this;
		}
		Vector4& operator-=(const Vector4& other) {
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;
			return *this;
		}
		Vector4& operator*=(float scalar) {
			x *= scalar;
			y *= scalar;
			z *= scalar;
			w *= scalar;
			return *this;
		}
		Vector4& operator/=(float scalar) {
			if (scalar == 0.0f)
				throw std::runtime_error("Division by zero");
			x /= scalar;
			y /= scalar;
			z /= scalar;
			w /= scalar;
			return *this;
		}
		bool operator==(const Vector4& other) const {
			return x == other.x && y == other.y && z == other.z && w == other.w;
		}

		bool operator!=(const Vector4& other) const {
			return !(*this == other);
		}

		/// <summary>
		/// ベクトルの長さを返す
		/// </summary>
		/// <returns>ベクトルの長さ</returns>
		float Length() const {
			return std::sqrt(x * x + y * y + z * z + w * w);
		}
		/// <summary>
		/// ベクトルを正規化した時の値を返す
		/// </summary>
		/// <returns>正規化したベクトルの値</returns>
		Vector4 Normalized() const {
			float len = Length();
			if (len == 0.0f) return Vector4(0.0f, 0.0f, 0.0f, 0.0f);
			return Vector4(x / len, y / len, z / len, w / len);
		}
		/// <summary>
		/// 自分自身を正規化して値を返す
		/// </summary>
		/// <returns>正規化したベクトルの値</returns>
		Vector4& Normalize() {
			float len = Length();
			if (len == 0.0f) {
				x = y = z = w = 0.0f;
				return *this;
			}

			x /= len;
			y /= len;
			z /= len;
			w /= len;
			return *this;
		}
	};

}