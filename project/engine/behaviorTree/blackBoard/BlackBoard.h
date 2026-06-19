#pragma once
#include <any>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <MyMath.h>

namespace Norm {

	/// <summary>
	/// ブラックボード
	/// </summary>
	class BlackBoard {
	public:
		// 要素をセット
		template<typename T>
		void SetValue(const std::string& key, const T& value) {
			mData[key] = value;
		}

		// 要素を取得
		template<typename T>
		T GetValue(const std::string& key) const {
			auto it = mData.find(key);
			if (it != mData.end()) {
				return std::any_cast<T>(it->second);
			}

			throw std::runtime_error("キーが見つかりませんでした： " + key);
		}

		// キーがあるかチェック
		bool HasKey(const std::string& key) const {
			return mData.find(key) != mData.end();
		}

	private:
		//データのコンテナ
		std::unordered_map<std::string, std::any> mData;
	};

}