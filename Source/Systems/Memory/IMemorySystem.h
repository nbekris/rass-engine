// File Name:    IMemorySystem.h
// Author(s):    main Steven Yacoub, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System providing custom memory allocation.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "../GlobalEvents/IGlobalEventsSystem.h"

namespace RassEngine::Systems {

	class IMemorySystem : public IGlobalSystem<IMemorySystem> {

		public:
			virtual ~IMemorySystem() = default;

			virtual size_t GetActiveAllocationCount() const = 0;
			virtual size_t GetTotalBytesAllocated() const = 0;
	};
}
