#pragma once
#include <windows.h>
#include <string>

namespace Norm {

	/// <summary>
	/// ログ出力
	/// </summary>
	namespace Logger {
		/// <summary>
		/// ログ出力
		/// </summary>
		/// <param name="message">ログ内容</param>
		void Log(const std::string& message);
	}

}