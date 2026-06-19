#pragma once
#include <memory>

namespace Norm {

	/// <summary>
	/// Shared_ptrを持ったIDを提供するクラス
	/// 情報の重複を避けるために使う
	/// </summary>
	struct Handle {
		uint32_t id = 0u;
		std::shared_ptr<void> ref;

		/// <summary>
		/// ハンドルの作成
		/// </summary>
		/// <param name="_id">ID番号</param>
		void Create(uint32_t _id) {
			id = _id;
			ref = std::make_shared<int>(0);
		}

		/// <summary>
		/// Handleの共有
		/// </summary>
		/// <returns>同列のHandle</returns>
		Handle Share() {
			Handle handle;
			handle.id = id;
			handle.ref = ref;
			return handle;
		}

	};

}