// File Name:    CameraSystem.h
// Author(s):    main Niko Bekris, secondary Boyuan Hu, Taro Omiya, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing the game camera and viewport.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "ICameraSystem.h"
#include "Events/GlobalEventListener.h"
#include "../Input/IInputSystem.h"

// Forward declaration
namespace RassEngine::Components {
class Transform;
class Camera;
}

namespace RassEngine::Systems {
class CameraSystem : public ICameraSystem {
public:
	//enum class CameraFollowMode {
	//	Lerp,
	//	Push //Character push the camera window like push a box.
	//};
	CameraSystem();
	virtual ~CameraSystem();
	bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) override;

	// Add these overrides to satisfy the ISystem interface
	bool Initialize() override;
	void Shutdown() override;
	const std::string_view &NameClass() const override;

	glm::mat4 GetViewMatrix() const override;
	Components::Camera *GetActiveCamera() const override;
	void RegisterCamera(Components::Camera *camera) override;
	void UnregisterCamera(Components::Camera *camera) override;
	//glm::vec2 ViewportToWorld(const glm::vec2 &ndcPos) const;
	void MoveCamera(float dt);
	void TestZoom(float dt);
	void LerpAimOffset(float dt);
	void LerpWindowOffset(float dt);
	void SetCameraWindowOffset(const glm::vec3 &offset);
	bool IsInCameraWindow(const glm::vec3 &worldPos) const;
	std::pair<glm::vec3, glm::vec3> GetCameraWindowPosSize() const;
	void FollowCamera(float dt);
	void LerpZoom(float dt);
	glm::vec2 ViewportToWorld(const glm::vec2 &ndcPos) const override;
	glm::vec2 WorldToScreenPixels(const glm::vec3 &worldPos) const override;
	glm::vec2 WorldToScreenViewport(const glm::vec3 &worldPos) const override;
private:
	Events::GlobalEventListener<CameraSystem> updateListener;
	Events::GlobalEventListenerLambda onSceneShutdown;

	Systems::IInputSystem *inputSystem{nullptr};

	// only one camera for now, but we can extend this to support multiple cameras later
	Components::Camera *camera{nullptr};
	
	glm::vec2 position{0.0f, 0.0f};
	float speed = 0.5f;
	
};
}

