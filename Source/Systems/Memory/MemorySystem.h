// File Name:    MemorySystem.h
// Author(s):    main Steven Yacoub, secondary Eric Fleegal, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System providing custom memory allocation.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "IMemorySystem.h"
#include <string_view>
#include <atomic>

namespace RassEngine::Systems {

	class MemorySystem : public IMemorySystem{
	public:
		MemorySystem();
		~MemorySystem() override;

		bool Initialize() override;
		const std::string_view &NameClass() const override; 
		void Shutdown() override;

		size_t GetActiveAllocationCount() const	override;
		size_t GetTotalBytesAllocated() const override;

		static void Allocated(size_t size);
		static void Deallocated();

	private:
		static std::atomic<size_t> activeAllocation;
		static std::atomic<size_t> totalBytes;
	};
}


