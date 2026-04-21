// File Name:    DemoScene.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris, Steven Yacoub, Boyuan Hu, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Demo scene showcasing engine features.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Scenes/DemoScene.h"

#include <memory>
#include <utility>

#include "Entity.h"
#include "Stream.h"
#include "Systems/Entity/IEntityFactory.h"
#include "Systems/Audio/IAudioSystem.h"
#include "Components/Transform.h"
#include "Components/Collider.h"
#include "Components/Trigger.h"
#include "Events/TriggerEventIDs.h"
#include "Events/TriggerEventArgs.h"
#include "Events/EventListenerLambda.h"
#include "IEvent.h"

using namespace RassEngine::Systems;

namespace RassEngine::Scenes {

bool DemoScene::Initialize() {

	std::unique_ptr<Entity> monkey1 = IEntityFactory::Get()->Build("Monkey");
	std::unique_ptr<Entity> floor = IEntityFactory::Get()->Build("Floor");
	std::unique_ptr<Entity> box = IEntityFactory::Get()->Build("Box");
	std::unique_ptr<Entity> triggerEntity = IEntityFactory::Get()->Build("TriggerEntity");

	// Example of setting up trigger event listeners
	if(triggerEntity) {
		auto *triggerComponent = triggerEntity->Get<Components::Trigger>();
		if(triggerComponent) {
			// OnTriggerEnter listener
			auto enterListener = new Events::EventListenerLambda<Events::EventArgs>(
				[](const IEvent<Events::EventArgs> *, const Events::EventArgs &args) {
					const auto &triggerArgs = static_cast<const Events::TriggerEventArgs&>(args);
					LOG_INFO("Entity entered trigger zone!");
					return true;
				});
			triggerEntity->BindEvent(Events::TriggerEventID::OnTriggerEnter, enterListener);
			// OnTriggerExit listener
			auto exitListener = new Events::EventListenerLambda<Events::EventArgs>(
				[](const IEvent<Events::EventArgs> *, const Events::EventArgs &args) {
					const auto &triggerArgs = static_cast<const Events::TriggerEventArgs&>(args);
					LOG_INFO("Entity exited trigger zone!");
					return true;
				});
			triggerEntity->BindEvent(Events::TriggerEventID::OnTriggerExit, exitListener);
		}
	}

	// Audio file loading
	std::string audioFilePath = "Monke.flac";
	std::string jumpAudioFilePath = "MonkeJump1.wav";
	auto audioSystem = Systems::IAudioSystem::Get();
	audioSystem->LoadSound(jumpAudioFilePath);
	audioSystem->Get()->StreamMusic(audioFilePath, true);
	audioSystem->Get()->PlayMusic(audioFilePath);

	std::unique_ptr<Entity> textEntity = IEntityFactory::Get()->Build("TextEntity");
	std::unique_ptr<Entity> tileEntity = IEntityFactory::Get()->Build("TileEntity");
	std::unique_ptr<Entity> tileEntity_back = IEntityFactory::Get()->Build("TileEntity_decoration_back");
	std::unique_ptr<Entity> tileEntity_trigger = IEntityFactory::Get()->Build("TileEntity_trigger");
	// Adding background
	std::unique_ptr<Entity> bkg = IEntityFactory::Get()->Build("Background");

	// Since we have to move them, add entities last
	AddEntity(std::move(monkey1));
	AddEntity(std::move(box));
	AddEntity(std::move(triggerEntity));
	AddEntity(std::move(textEntity));
	AddEntity(std::move(tileEntity));
	AddEntity(std::move(tileEntity_back));
	AddEntity(std::move(tileEntity_trigger));
	AddEntity(std::move(bkg));
	AddEntity(std::move(floor));

	// Call the base method
	return Scene::Initialize();
}

const std::string_view &DemoScene::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Scenes::DemoScene);
	return className;
}

}
