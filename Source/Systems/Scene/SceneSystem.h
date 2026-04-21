// File Name:    SceneSystem.h
// Author(s):    main Taro Omiya, secondary Boyuan Hu, Niko Bekris, Steven Yacoub
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing scene loading and transitions.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <functional>
#include <memory>
#include <string_view>
#include <typeinfo>

#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"
#include "ISceneSystem.h"

// Forward declaration
namespace RassEngine {
class Scene;
class Entity;
}

namespace RassEngine::Systems {

class SceneSystem : public ISceneSystem {
public:
	SceneSystem(const std::string_view& firstSceneName);
	virtual ~SceneSystem();

	// Inherited via ISystem
	bool Initialize() override;
	const std::string_view &NameClass() const override;
	void Shutdown() override;
	void AddEntity(std::unique_ptr<Entity> &&entity) override;
	void EndScene() override;
	void Restart() override;
	bool IsRunning() const override;
	bool IsSceneChanging() const override;
	bool IsSceneRestarting() const override;

	// === NEW: Scene access implementation ===
	Scene *GetCurrentScene() const override;
	void ForEachActiveEntity(std::function<void(const Entity *)> callback) const override;
	Entity *FindEntity(const std::string_view &name) const override;
	Entity *FindEntity(const UUID &id) const override;
	const std::vector<Entity *> *FindEntities(const std::string_view &name) const override;

protected:
	void SetPendingScene(const std::type_info &sceneType, std::function<std::unique_ptr<Scene>()> sceneGenerator) override;

private:
	bool OnUpdate(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);

	inline void ClearRestartFlag() {
		isRestarting = false;
	}

	// Avoid throwing a null-reference exception when there is no active scene.
	void CallInitialize() const;
	void CallShutdown() const;

	// Private Variables:
private:
	std::unique_ptr<Scene> activeScene{nullptr};
	std::unique_ptr<Scene> pendingScene{nullptr};

	bool isEnding{false};
	bool isRestarting{false};

	Events::GlobalEventListener<SceneSystem> updateListener;
};

} // namespace RassEngine::Systems
