// File Name:    UUID.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Universally unique identifier generation and management.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

namespace RassEngine {
class UUID {
	friend struct std::hash<UUID>;
public:
	using Type = unsigned int;

	UUID();
	inline virtual ~UUID() = default;
	inline UUID(const UUID::Type &id) : id{id} {}
	inline UUID(const UUID &other) : id{other.id} {}
	UUID(UUID &&other) noexcept;

	inline UUID &operator=(const UUID &other) {
		id = other.id;
		return *this;
	}

	inline UUID &operator=(const UUID &&other) noexcept {
		id = other.id;
		return *this;
	}

	inline operator Type () const {
		return id;
	}

	inline bool operator==(const UUID &other) const {
		return id == other.id;
	}

	inline bool operator!=(const UUID &other) const {
		return !operator==(other);
	}

private:
	Type id;
};

/// <summary>
/// Applies for any classes that defines function:
/// <code>operator UUID::Type () const {}</code>
/// </summary>
template<class T>
concept IsUUID = std::convertible_to<T, UUID::Type>;

}

// Define a hash functor for UUIDs
template <>
struct std::hash<RassEngine::UUID> {
	std::size_t operator()(const RassEngine::UUID &k) const noexcept {
		return hash<RassEngine::UUID::Type>()(k.id);
	}
};
