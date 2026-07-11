#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include "IGameFeelAction.h"

namespace RassGame::Systems {
class GameFeelActionFactory {
public:
	using Constructor = std::function<std::shared_ptr<IGameFeelAction>()>;
	static std::shared_ptr<IGameFeelAction> Create(const std::string &type);
	static bool Register(const std::string &type, Constructor constructor);
private:
	static std::unordered_map<std::string, Constructor> &Registry();
	static void EnsureDefaults();
};
}
