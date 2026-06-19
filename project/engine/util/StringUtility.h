#pragma once
#include <windows.h>
#include <string>

namespace Norm {

	/// <summary>
	/// 文字列の型変換
	/// </summary>
	namespace StringUtility {
		/// <summary>
		/// stringをwstringに変換する
		/// </summary>
		/// <param name="str">変換するstring型文字列</param>
		/// <returns>変換後のwstring型文字列</returns>
		std::wstring ConvertString(const std::string& str);

		/// <summary>
		/// wstringをstringに変換する
		/// </summary>
		/// <param name="str">変換するwstring型文字列</param>
		/// <returns>変換後のstring型文字列</returns>
		std::string ConvertString(const std::wstring& str);

	}

}