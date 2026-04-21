// File Name:    TileDoor.h
// Author(s):    main Boyuan Hu, secondary Taro Omiya, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component for a tile-based door controlled by switches.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <Cloneable.h>
#include <Component.h>
#include <Components/TileMap.h>
#include <Events/EventListenerMethod.h>
#include <Events/GlobalEventArgs.h>
#include <IEvent.h>
#include <Stream.h>
#include <string_view>
#include <utility>
#include <UUID.h>
#include <vector>

#include "Components/Switch.h"

namespace RassGame::Components {

class TileDoor : public RassEngine::Cloneable<RassEngine::Component, TileDoor> {
public:
	TileDoor();
	TileDoor(const TileDoor &other);
	virtual ~TileDoor() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

private:
	struct TileData {
		unsigned short row;
		unsigned short column;
		bool isOpenByDefault;
	};
	struct DoorEntry {
		RassEngine::UUID switchId{0u};
		Switch *cachedSwitch{nullptr};
		std::vector<TileData> tiles;
	};

	Switch *FindSwitch(const RassEngine::UUID &id) const;
	bool OnSwitchChanged(const RassEngine::IEvent<Switch::ToggleEventArgs> *, const Switch::ToggleEventArgs &args);
	bool OnFirstUpdate(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *, const RassEngine::Events::GlobalEventArgs &);
	void EjectEntitiesFromTile(unsigned short row, unsigned short col);

private:
	std::vector<DoorEntry> doors;
	RassEngine::Components::TileMap *tileMap{nullptr};
	RassEngine::Events::EventListenerMethod<TileDoor, Switch::ToggleEventArgs> onSwitchChanged;

	bool initialSyncDone{false};
	RassEngine::Events::EventListenerMethod<TileDoor, RassEngine::Events::GlobalEventArgs> onFirstUpdate;
};

} // namespace RassGame::Components
