// File Name:    Main.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris, Boyuan Hu, Steven Yacoub, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Entry point for the RASS game application.
//
// Copyright © 2026 DigiPen (USA) Corporation.

// Prototype1.cpp : Defines the entry point for the application.
//

#include "pch.h"

#include <glbinding/gl/functions.h>
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

// GLFW macros
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <windows.h>
#include <print>

#include "resource.h"
#include <Utils.h>
#include <Engine.h>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Logging/LoggingSystem.h>
#include <Systems/Entity/IEntityFactory.h>
#include <Systems/Entity/EntityFactory.h>
#include <Systems/Memory/MemorySystem.h>
#include <Systems/GlobalEvents/IGlobalEventsSystem.h>
#include <Systems/GlobalEvents/GlobalEventsSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <Systems/Scene/SceneSystem.h>
#include <Systems/Input/IInputSystem.h>
#include <Systems/Input/InputSystemGlfw.h>
#include <Systems/Render/IRenderSystem.h>
#include <Systems/Render/RenderSystemOpenGl.h>
#include <Systems/Resource/IResourceSystem.h>
#include <Systems/Resource/ResourceSystem.h>
#include <Systems/DebugDraw/IDebugDrawSystem.h>
#include <Systems/DebugDraw/DebugDrawSystem.h>
#include <Systems/ImGui/IImGuiSystem.h>
#include <Systems/ImGui/ImGuiSystem.h>
#include <Systems/Manager/SystemsManagerBuilder.h>
#include <Systems/Time/ITimeSystem.h>
#include <Systems/Time/TimeSystem.h>
#include <Systems/Physics/IPhysicsSystem.h>
#include <Systems/Physics/PhysicsSystem.h>
#include <Systems/Audio/IAudioSystem.h>
#include <Systems/Audio/AudioSystem.h>
#include <Systems/Cheat/ICheatSystem.h>
#include <Systems/Cheat/CheatSystem.h>
#include <Systems/PauseMenu/IPauseMenuSystem.h>
#include <Systems/PauseMenu/PauseMenuSystem.h>
#include <Systems/OptionsMenu/IOptionsMenuSystem.h>
#include <Systems/OptionsMenu/OptionsMenuSystem.h>
#include <Systems/Camera/ICameraSystem.h>
#include <Systems/Camera/CameraSystem.h>
#include <Systems/UI/IUiSystem.h>
#include <Systems/UI/UiSystem.h>
#include <Systems/Component/IComponentFactory.h>
#include <Systems/Component/ComponentFactory.h>

#include "Components/FlipOrigin.h"
#include "Components/Flippable.h"
#include "Components/IsKinematicMidFlip.h"
#include "Components/DisableTriggerMidFlip.h"
#include "Components/Switch.h"
#include "Components/SpriteSwapper.h"
#include "Components/Door.h"
#include "Components/Flipper.h"
#include "Components/SceneTransition.h"
#include "Components/TileDoor.h"
#include "Components/TileCheckPoint.h"
#include "Components/ZoomOutArea.h"
#include "Components/ChangeSceneOnClick.h"
#include "Components/QuitOnClick.h"
#include "Components/TimedSceneTransition.h"
#include "Components/ResumeOnClick.h"
#include "Components/ShowOptionsOnClick.h"
#include "Components/ShowHowToPlayOnClick.h"
#include "Components/BackToPauseOnClick.h"
#include "Components/ShowConfirmExitOnClick.h"
#include "Components/ShowConfirmQuitOnClick.h"
#include "Components/DeathArea.h"
#include "Components/ParallaxController.h"
#include "Components/WeaponUnlocker.h"

using namespace gl;
using namespace RassEngine;
using namespace RassEngine::Systems;
using namespace RassGame::Components;

static constexpr std::string_view DEFAULT_SCENE_NAME = "DigiSplash";


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

#ifdef _DEBUG
	// Setting up memory manager first
	auto memoryManager = std::make_unique<MemorySystem>();

	// Move console output to the pop-up
	AllocConsole();
	FILE *fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
	std::println("Debug Console Initialized...");
#endif

	// Register Memory System before all else, then
	// register Logging System after binding to the debug console,
	// but before the window is created
	auto builder = std::make_unique<Systems::SystemsManagerBuilder>();
	builder->Register<ILoggingSystem, LoggingSystem>();

	// Create a window instance
	std::shared_ptr<GLFWwindow> window = Engine::CreateGlfwWindow();
	if(!window) {
		LOG_ERROR("{} failed", NAMEOF(Engine::CreateGlfwWindow()));
		return -1;
	}

	// Bind OpenGL functions using glbinding, with the GLFW function loader
	glbinding::initialize(glfwGetProcAddress);

	// Register systems here, using the builder pattern
	(*builder)
		.Register<IGlobalEventsSystem, GlobalEventsSystem>()
		.Register<IRenderSystem, RenderSystemOpenGl>(window.get())
		.Register<IInputSystem, InputSystemGlfw>(window.get())
		.Register<ISceneSystem, SceneSystem>(DEFAULT_SCENE_NAME)
		.Register<IResourceSystem, ResourceSystem>()
		.Register<ITimeSystem, TimeSystem>()
		.Register<IPhysicsSystem, PhysicsSystem>()
		.Register<IImGuiSystem, ImGuiSystem>(window.get())
		.Register<IAudioSystem, AudioSystem>()
		.Register<IComponentFactory, ComponentFactory>()
		.Register<IEntityFactory, EntityFactory>()
		.Register<ICameraSystem, CameraSystem>()
		.Register<IUiSystem, UiSystem>()
		.Register<IDebugDrawSystem, DebugDrawSystem>();

	// Register new components here
	IComponentFactory::Get()->Register(NAMEOF(FlipOrigin), [] () -> Component* {
		return new FlipOrigin();
	});
	IComponentFactory::Get()->Register(NAMEOF(Flippable), [] () -> Component * {
		return new Flippable();
	});
	IComponentFactory::Get()->Register(NAMEOF(IsKinematicMidFlip), [] () -> Component * {
		return new IsKinematicMidFlip();
	});
	IComponentFactory::Get()->Register(NAMEOF(Switch), [] () -> Component * {
		return new Switch();
	});
	IComponentFactory::Get()->Register(NAMEOF(SpriteSwapper), [] () -> Component * {
		return new SpriteSwapper();
	});
	IComponentFactory::Get()->Register(NAMEOF(Door), [] () -> Component * {
		return new Door();
	});
	IComponentFactory::Get()->Register(NAMEOF(Flipper), [] () -> Component * {
		return new Flipper();
	});
	IComponentFactory::Get()->Register(NAMEOF(SceneTransition), [] () -> Component * {
		return new SceneTransition();
	});
	IComponentFactory::Get()->Register(NAMEOF(DisableTriggerMidFlip), [] () -> Component * {
		return new DisableTriggerMidFlip();
	});
	IComponentFactory::Get()->Register(NAMEOF(TileDoor), [] () -> Component * {
		return new TileDoor();
	});
	IComponentFactory::Get()->Register(NAMEOF(TileCheckPoint), [] () -> Component * {
		return new TileCheckPoint();
	});
	IComponentFactory::Get()->Register(NAMEOF(ZoomOutArea), [] () -> Component * {
		return new ZoomOutArea();
	});
	IComponentFactory::Get()->Register(NAMEOF(ChangeSceneOnClick), [] () -> Component * {
		return new ChangeSceneOnClick();
	});
	IComponentFactory::Get()->Register(NAMEOF(QuitOnClick), [] () -> Component * {
		return new QuitOnClick();
	});
	IComponentFactory::Get()->Register(NAMEOF(TimedSceneTransition), [] () -> Component * {
		return new TimedSceneTransition();
	});
	IComponentFactory::Get()->Register(NAMEOF(ResumeOnClick), [] () -> Component * {
		return new ResumeOnClick();
	});
	IComponentFactory::Get()->Register(NAMEOF(ShowOptionsOnClick), [] () -> Component * {
		return new ShowOptionsOnClick();
	});
	IComponentFactory::Get()->Register(NAMEOF(ShowHowToPlayOnClick), [] () -> Component * {
		return new ShowHowToPlayOnClick();
	});
	IComponentFactory::Get()->Register(NAMEOF(BackToPauseOnClick), [] () -> Component * {
		return new BackToPauseOnClick();
	});
	IComponentFactory::Get()->Register(NAMEOF(ShowConfirmExitOnClick), [] () -> Component * {
		return new ShowConfirmExitOnClick();
	});
	IComponentFactory::Get()->Register(NAMEOF(ShowConfirmQuitOnClick), [] () -> Component * {
		return new ShowConfirmQuitOnClick();
	});
	IComponentFactory::Get()->Register(NAMEOF(DeathArea), [] () -> Component * {
		return new DeathArea();
	});
	IComponentFactory::Get()->Register(NAMEOF(ParallaxController), [] () -> Component * {
		return new ParallaxController();
	});
	IComponentFactory::Get()->Register(NAMEOF(WeaponUnlocker), [] () -> Component * {
		return new WeaponUnlocker();
	});

	//#ifdef _DEBUG
	// Only register the cheats if in debug mode
	std::initializer_list<std::string_view> scenePaths = {
		"Level1Plat",
		"Level1Puzzle",
		"Level2Plat",
		"Level2Puzzle",
		"VerticalSlicePlat",
		"VerticalSlicePuzzle",
		"Level4Puzzle"
	};
	builder->Register<ICheatSystem, CheatSystem>(scenePaths);
//#endif // _DEBUG

	builder->Register<IPauseMenuSystem, PauseMenuSystem>();
	builder->Register<IOptionsMenuSystem, OptionsMenuSystem>();

	Engine gameEngine{builder->build()};

	// Clean-up builder
	builder.reset(nullptr);

	// Make sure the game engine can run
	if(!gameEngine.canRun()) {
		LOG_ERROR("{} is not setup correctly", NAMEOF(RassEngine::Engine));
		glfwTerminate();
		return 1;
	}

	// Run the game
	gameEngine.run();
	return 0;
}
