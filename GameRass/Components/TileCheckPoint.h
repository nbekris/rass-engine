// File Name:    TileCheckPoint.h
// Author(s):    main Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Component marking a tile as a player checkpoint.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <climits>
#include <Cloneable.h>
#include <Component.h>
#include <Components/TileMap.h>
#include <Events/EventListenerMethod.h>
#include <Events/GlobalEventArgs.h>
#include <IEvent.h>
#include <Stream.h>
#include <string_view>
#include <UUID.h>

namespace RassGame::Components {

class TileCheckPoint : public RassEngine::Cloneable<RassEngine::Component, TileCheckPoint> {
public:
	TileCheckPoint();
	TileCheckPoint(const TileCheckPoint &other);
	virtual ~TileCheckPoint() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

private:
	bool OnUpdate(
		const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *,
		const RassEngine::Events::GlobalEventArgs &);

	RassEngine::Entity *FindPlayer(const std::string &name) const;

private:
	unsigned checkpointTileIndex{0};
	//RassEngine::UUID playerEntityId{0u};
	std::string playerEntityName{"Player"};
	int checkAreaHeight{5};  // Number of tiles above the checkpoint tile that also count as "on the checkpoint"
	RassEngine::Entity *cachedPlayer{nullptr};
	RassEngine::Components::TileMap *tileMap{nullptr};

	// Last checkpoint tile that updated the respawn position; prevents re-triggering
	// the same tile every frame while the player stands still
	unsigned lastCpRow{UINT_MAX};
	unsigned lastCpCol{UINT_MAX};

	RassEngine::Events::EventListenerMethod<TileCheckPoint, RassEngine::Events::GlobalEventArgs> onUpdate;
};

} // namespace RassGame::Components
