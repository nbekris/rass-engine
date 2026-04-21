// File Name:    IGlobalSystem.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Interface for globally accessible systems.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <mutex>

#include "ISystem.h"

namespace RassEngine::Systems {

// Forward declarations
class SystemsManager;

template <class T>
class IGlobalSystem : public ISystem {
	friend class SystemsManager;

	class Impl {
		friend class SystemsManager;
	public:
		/// <summary>
		/// Gets an instance.  *Can* return null!
		/// </summary>
		static T *Get(const void *processId);

	private:
		/// <summary>
		/// Thread-safe set, with process registration
		/// </summary>
		static void Set(const void *processId, T *setTo);
		/// <summary>
		/// Thread-safe set
		/// </summary>
		inline static void Set(T *newInstance) {
			Set(nullptr, newInstance);
		}

		static T *instance;
	};

public:
	/// <summary>
	/// Retrieves the global instance (or process-specific instance) of <typeref name="T"/>.  *Can* return null!
	/// </summary>
	inline static T *Get(const void *processId = nullptr) {
		return Impl::Get(processId);
	}
};

template<class T>
T *IGlobalSystem<T>::Impl::instance = nullptr;

template<class T>
inline T *IGlobalSystem<T>::Impl::Get(const void *) {
	//static std::mutex mtx;
	//std::lock_guard<std::mutex> lock(mtx);
	return instance;
}

template<class T>
inline void IGlobalSystem<T>::Impl::Set(const void *, T *setTo) {
	static std::mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	instance = setTo;
}

}
