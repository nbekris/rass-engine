// File Name:    IOptionsMenuSystem.h
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing the options menu UI.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "Systems/GlobalEvents/IGlobalSystem.h"

namespace RassEngine::Systems {

class IOptionsMenuSystem : public IGlobalSystem<IOptionsMenuSystem> {
public:
	virtual ~IOptionsMenuSystem() = default;

protected:
	IOptionsMenuSystem() = default;

private:
	IOptionsMenuSystem(const IOptionsMenuSystem &) = delete;
	IOptionsMenuSystem(IOptionsMenuSystem &&) noexcept = delete;
	IOptionsMenuSystem &operator=(const IOptionsMenuSystem &) = delete;
	IOptionsMenuSystem &operator=(IOptionsMenuSystem &&) noexcept = delete;
};

}
