// File Name:    ICameraSystem.h
// Author(s):    main Niko Bekris, secondary Taro Omiya, Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing the game camera and viewport.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once
#include <utility>
#include <glm/detail/type_mat.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Systems/GlobalEvents/IGlobalSystem.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"

// Forward declaration
namespace RassEngine::Components {
class Transform;
class Camera;
}

namespace RassEngine::Systems {

class ICameraSystem : public IGlobalSystem<ICameraSystem> {
public:
	virtual ~ICameraSystem() = default;
	virtual bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) = 0;

	virtual glm::mat4 GetViewMatrix() const = 0;
	virtual Components::Camera *GetActiveCamera() const = 0;
	virtual void RegisterCamera(Components::Camera *camera) = 0;
	virtual void UnregisterCamera(Components::Camera *camera) = 0;
	virtual void SetCameraWindowOffset(const glm::vec3 &offset) = 0;
	virtual glm::vec2 ViewportToWorld(const glm::vec2 &ndcPos) const = 0;
	virtual std::pair<glm::vec3, glm::vec3> GetCameraWindowPosSize() const = 0;
	virtual glm::vec2 WorldToScreenPixels(const glm::vec3 &worldPos) const = 0;
	virtual glm::vec2 WorldToScreenViewport(const glm::vec3 &worldPos) const = 0;

protected:
	ICameraSystem() = default;
};
}
