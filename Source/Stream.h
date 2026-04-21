// File Name:    Stream.h
// Author(s):    main Steven Yacoub, secondary Taro Omiya, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      File stream utilities for reading and writing data.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <typeinfo>

#include "Systems/Logging/ILoggingSystem.h"
#include "Utils.h"

using json = nlohmann::json;

namespace RassEngine {

class Stream {

public:
	Stream(const std::string_view &filePath);
	~Stream(void);

public:
	bool IsValid() const {
		return isValid;
	}
	inline bool Contains(const std::string_view &key) const {
		return dataNode != nullptr ? dataNode->contains(key) : false;
	}

	void ReadArray(const std::string_view &key, std::function<void()> lambda);
	void ReadObject(const std::string_view &key, std::function<void(const std::string &key)> lambda);

	void PushNode(const std::string_view &key);
	void PopNode();

	template <typename T>
	bool Read(const std::string_view& key, T &value) const;

	template <typename T>
	bool ReadAs(T &value) const;

	bool ReadVec2(const std::string_view &key, glm::vec2 &value) const;
	bool ReadVec3(const std::string_view &key, glm::vec3 &value) const;
	bool ReadVec4(const std::string_view &key, glm::vec4 &value) const;

private:
	bool Open(const std::string_view &filePath);
	template <typename T>
	bool ReadVector(const std::string_view &key, const std::string_view &readType, std::function<bool(const std::vector<T> &)> process) const;

private:
	bool isValid = false;

	json jsonData{};

	const json *dataNode{nullptr};
	std::vector<const json *> dataStack;
};

template <typename T>
inline bool Stream::Read(const std::string_view& key, T &value) const {

	if(!isValid) {
		return false;
	} else if(!Contains(key)) {
		return false;
	}

	try {
		value = dataNode->at(key).get<T>();
	} catch(const json::exception &exception) {
		LOG_ERROR("Stream: JSON parsing error reading key {} as {}: {}", key, typeid(T).name(), exception.what());
		return false;
	}

	return true;
}

template <typename T>
inline bool Stream::ReadVector(const std::string_view &key, const std::string_view &readType, std::function<bool(const std::vector<T> &)> process) const {
	if(!isValid) {
		return false;
	} else if(!Contains(key)) {
		return false;
	}

	try {
		// Retrieve the data as vector
		const std::vector<T> &vector = dataNode->at(key).get<std::vector<T>>();
		return process(vector);
	} catch(const json::exception &exception) {
		LOG_ERROR("Stream: JSON parsing error reading key {} as {}: {}", key, readType, exception.what());
		return false;
	}

	return true;
}

template <typename T>
inline bool Stream::ReadAs(T &value) const {
	if(!isValid) {
		return false;
	}

	try {
		// Retrieve the data directly
		value = dataNode->get<T>();
	} catch(const json::exception &exception) {
		return false;
	}

	return true;
}

}
