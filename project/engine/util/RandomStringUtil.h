#pragma once
#include <string>

namespace Norm {

	/// <summary>
	/// ランダムな文字列生成用クラス
	/// </summary>
	class RandomStringUtil {
	public:
		/// ============================== ///
		///		メンバ関数
		///	============================== ///

		/// <summary>
		/// ランダムな文字列を生成する
		/// </summary>
		/// <param name="length">文字列の長さ</param>
		/// <returns>ランダムなstring型文字列</returns>
		static std::string GenerateRandomString(size_t length);
	};

}