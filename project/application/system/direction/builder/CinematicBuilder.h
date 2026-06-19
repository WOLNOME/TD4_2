#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <memory>

//前方宣言（エンジン層）
namespace Norm {
	class BlackBoard;
}

//前方宣言（アプリ層）
class IShotBase;

/// <summary>
/// 映像演出ビルダー
/// </summary>
class CinematicBuilder {
public:
	static std::vector<std::unique_ptr<IShotBase>> BuildCinematic(const std::string& _fileName, Norm::BlackBoard* _blackBoard);
};

