#pragma once
#include <iostream>
#include <cmath>

namespace Norm {

    /// <summary>
    /// クォータニオン
    /// </summary>
    struct Quaternion {
        float x;
        float y;
        float z;
        float w;

        /// <summary>
        /// コンストラクタ
        /// </summary>
        /// <param name="x">x</param>
        /// <param name="y">y</param>
        /// <param name="z">z</param>
        /// <param name="w">w</param>
        Quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f) : x(x), y(y), z(z), w(w) {}

        /// <summary>
        /// ノルム(大きさ)の計算
        /// </summary>
        /// <returns>ノルム</returns>
        float norm() const {
            return std::sqrt(w * w + x * x + y * y + z * z);
        }

        /// <summary>
        /// 正規化の計算
        /// </summary>
        void normalize() {
            float n = norm();
            if (n > 0.0001f) { // ほぼゼロの場合は正規化しない
                x /= n;
                y /= n;
                z /= n;
                w /= n;
            }
            else {
                std::cerr << "Warning: Quaternion norm is too close to zero to normalize\n";
            }
        }

        /// <summary>
        /// 共役の計算
        /// </summary>
        /// <returns>共役</returns>
        Quaternion conjugate() const {
            return Quaternion(-x, -y, -z, w);
        }

    };

}