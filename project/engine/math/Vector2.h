#pragma once
#include <stdexcept>
#include <cmath>

namespace Norm {

	/// <summary>
	/// 2次元ベクトル
	/// </summary>
	struct Vector2 final {
		float x;
		float y;
		/// <summary>
		/// 初期化リスト
		/// </summary>
		/// <param name="x">x</param>
		/// <param name="y">y</param>
		Vector2(float x = 0, float y = 0) : x(x), y(y) {}
		//複合代入演算子のオーバーロード
		Vector2& operator+=(const Vector2& other) {
			x += other.x;
			y += other.y;
			return *this;
		}
		Vector2& operator-=(const Vector2& other) {
			x -= other.x;
			y -= other.y;
			return *this;
		}
		Vector2& operator*=(float scalar) {
			x *= scalar;
			y *= scalar;
			return *this;
		}
		Vector2& operator/=(float scalar) {
			if (scalar == 0.0f)
				throw std::runtime_error("Division by zero");
			x /= scalar;
			y /= scalar;
			return *this;
		}
		bool operator==(const Vector2& other) const {
			return x == other.x && y == other.y;
		}

		bool operator!=(const Vector2& other) const {
			return !(*this == other);
		}

		/// <summary>
		/// ベクトルの長さを返す
		/// </summary>
		/// <returns>ベクトルの長さ</returns>
		float Length() const {
			return std::sqrt(x * x + y * y);
		}
		/// <summary>
		/// ベクトルを正規化した時の値を返す
		/// </summary>
		/// <returns>正規化したベクトルの値</returns>
		Vector2 OutputNormalized() const {
			float len = Length();
			if (len == 0.0f) return Vector2(0.0f, 0.0f);
			return Vector2(x / len, y / len);
		}
		/// <summary>
		/// 自分自身を正規化して値を返す
		/// </summary>
		/// <returns>正規化したベクトルの値</returns>
		Vector2& Normalize() {
			float len = Length();
			if (len == 0.0f) {
				x = y = 0.0f;
				return *this;
			}
			x /= len;
			y /= len;
			return *this;
		}
	};

}