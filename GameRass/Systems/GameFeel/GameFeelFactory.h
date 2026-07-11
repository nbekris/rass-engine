#pragma once
#include "IGameFeelFactory.h"

#include <cstddef>
#include <memory>
#include <string_view>
#include <vector>

#include <Components/SpriteFader.h>
#include <Events/GlobalEventArgs.h>
#include <Events/GlobalEventListener.h>
#include <IEvent.h>
#include "FeelContext.h"

namespace RassGame::Systems {
class IGameFeelAction;

class GameFeelFactory : public IGameFeelFactory {
public:
	GameFeelFactory();
	~GameFeelFactory() override;

	bool Initialize() override;
	const std::string_view &NameClass() const override;
	void Shutdown() override;

	bool Show(const glm::vec3 &color, const RassEngine::TweenCurve &curve) override;
	void PlayActions(const std::vector<std::shared_ptr<IGameFeelAction>> &actions, const FeelContext &context) override;
	void RequestHitStop(float unscaledDuration, float timeScale) override;

private:
	bool OnSceneLoaded(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *, const RassEngine::Events::GlobalEventArgs &);
	bool OnSceneUnloaded(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *, const RassEngine::Events::GlobalEventArgs &);
	bool OnUpdate(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *, const RassEngine::Events::GlobalEventArgs &);

	struct DetachedPlayback {
		std::vector<std::shared_ptr<IGameFeelAction>> actions;   // copy shared_ptr to keep them alive until finished
		float elapsed{0.0f};
		FeelContext context{};
		std::size_t cursor{0};
	};

	static constexpr std::string_view ENTITY_NAME = "ScreenFlash";
	RassEngine::Components::SpriteFader *flashComponent{nullptr};
	std::vector<DetachedPlayback> activePlaybacks{};
	RassEngine::Events::GlobalEventListener<GameFeelFactory> onSceneLoaded, onSceneUnloaded, onUpdate;
	bool  hitStopActive{false};
	float hitStopRemaining{0.0f};
	float hitStopPrevScale{1.0f};
};
}
