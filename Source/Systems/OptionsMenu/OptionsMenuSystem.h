// File Name:    OptionsMenuSystem.h
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing the options menu UI.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"
#include "IOptionsMenuSystem.h"

namespace RassEngine::Systems {

class OptionsMenuSystem : public IOptionsMenuSystem {
public:
	OptionsMenuSystem();
	virtual ~OptionsMenuSystem() override;

	bool Initialize() override;
	const std::string_view &NameClass() const override;
	void Shutdown() override;

private:
	OptionsMenuSystem(const OptionsMenuSystem &) = delete;
	OptionsMenuSystem(OptionsMenuSystem &&) noexcept = delete;
	OptionsMenuSystem &operator=(const OptionsMenuSystem &) = delete;
	OptionsMenuSystem &operator=(OptionsMenuSystem &&) noexcept = delete;

	bool Render(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);

	Events::GlobalEventListener<OptionsMenuSystem> renderListener;
};

}
