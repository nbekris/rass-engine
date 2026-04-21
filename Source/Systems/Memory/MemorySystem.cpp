// File Name:    MemorySystem.cpp
// Author(s):    main Steven Yacoub, secondary Taro Omiya, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System providing custom memory allocation.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include <Precompiled.h>
#include "MemorySystem.h"

#include <string_view>
#include <malloc.h>
#include <atomic>

#include "Systems/Logging/ILoggingSystem.h"
#include "Utils.h"

namespace RassEngine::Systems {
	std::atomic<size_t> MemorySystem::activeAllocation{0};
	std::atomic<size_t> MemorySystem::totalBytes{0};

	MemorySystem::MemorySystem(){}

	MemorySystem::~MemorySystem() {
		if(activeAllocation.load() > 0) {
			LOG_ERROR("MEMORY LEAK ALERT: {} allocations remain standing!", activeAllocation.load());
		} else {
			LOG_INFO("All allocations cleared.");
		}
	}

	bool MemorySystem::Initialize() {
		LOG_INFO("MemorySystem initialized");
		return true;
	}

	void MemorySystem::Shutdown() {
		// Do nothing
	}

	size_t MemorySystem::GetActiveAllocationCount() const {
		return activeAllocation.load();
	}

	size_t MemorySystem::GetTotalBytesAllocated() const {
		return totalBytes.load();
	}


	const std::string_view &MemorySystem::NameClass() const {
		static constexpr std::string_view className = NAMEOF(RassEngine::Systems::MemorySystem);
		return className;
	}

	void MemorySystem::Allocated(size_t size) {
		activeAllocation++;
		totalBytes += size;
	}

	void MemorySystem::Deallocated() {
		activeAllocation--;
	}
}

void* operator new(size_t size) {
	RassEngine::Systems::MemorySystem::Allocated(size);
	return malloc(size);
}

void operator delete(void *memory) noexcept{
	if(memory) {
		RassEngine::Systems::MemorySystem::Deallocated();
		free(memory);
	}
}

void *operator new[](size_t size) {
	RassEngine::Systems::MemorySystem::Allocated(size);
	return malloc(size);
}

void operator delete[](void *memory) noexcept {
	if(memory) {
		RassEngine::Systems::MemorySystem::Deallocated();
		free(memory);
	}
}
