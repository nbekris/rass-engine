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
