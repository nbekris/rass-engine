#pragma once
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <Cloneable.h>
#include <Component.h>
#include <IEvent.h>
#include <IFeelTrigger.h>
#include <Events/GlobalEventArgs.h>
#include <Events/GlobalEventListener.h>
#include <Stream.h>

#include "../Systems/GameFeel/FeelContext.h"

namespace RassGame::Systems {
class IGameFeelAction;
}

namespace RassGame::Components {
class GameFeelEvents
	: public RassEngine::Cloneable<RassEngine::Component, GameFeelEvents>
	, public RassEngine::IFeelTrigger {
public:
	GameFeelEvents();
	GameFeelEvents(const GameFeelEvents &other);
	virtual ~GameFeelEvents() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

	// IFeelTrigger
	virtual bool Play(const std::string_view &eventName) override;
	virtual bool PlayDetached(const std::string_view &eventName) override;
	// For sources without an entity (e.g. a destroyed tile): caller supplies the world position.
	virtual bool PlayDetachedAt(const std::string_view &eventName, const glm::vec3 &worldPos) override;
	void AddAction(const std::string_view &eventName,
		const std::shared_ptr<Systems::IGameFeelAction> &action);
private:
	bool Update(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *,
		const RassEngine::Events::GlobalEventArgs &);
	RassGame::Systems::FeelContext CaptureContext() const;
	struct ActionList {
		std::vector<std::shared_ptr<Systems::IGameFeelAction>> actions{};
	};
	struct Playback {
		const ActionList *list{nullptr};
		float elapsed{0.0f};
		std::size_t cursor{0};
		Systems::FeelContext context{};
	};

	std::unordered_map<std::string, ActionList> allActionLists{};
	std::vector<Playback> activePlaybacks{};
	RassEngine::Events::GlobalEventListener<GameFeelEvents> onUpdateListener;
	bool isInitialized{false};
};
}
