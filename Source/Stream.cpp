// File Name:    Stream.cpp
// Author(s):    main Taro Omiya, secondary Steven Yacoub, Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      File stream utilities for reading and writing data.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Stream.h"

#include <fstream>

#include "Systems/Logging/ILoggingSystem.h"
#include "Utils.h"

using json = nlohmann::json;

namespace RassEngine {

Stream::Stream(const std::string_view &filePath) {
	Open(filePath);
}

Stream::~Stream(void) {}

void Stream::ReadArray(const std::string_view &key, std::function<void()> lambda) {
	if(!Contains(key)) {
		return;
	}

	PushNode(key);

	for(auto &item : dataNode->items()) {
		dataStack.push_back(dataNode);
		dataNode = &item.value();

		lambda();

		dataNode = dataStack.back();
		dataStack.pop_back();
	}

	PopNode();
}

void Stream::ReadObject(const std::string_view &key, std::function<void(const std::string &key)> lambda) {
	if(!Contains(key)) {
		return;
	}

	PushNode(key);

	for(auto &[compType, compVal] : dataNode->items()) {
		lambda(compType);
	}

	PopNode();
}

void Stream::PushNode(const std::string_view &key) {
	if(Contains(key)) {
		dataStack.push_back(dataNode);
		this->dataNode = (&(*dataNode)[key]);
	}
}

void Stream::PopNode() {
	if(!dataStack.empty()) {
		this->dataNode = dataStack.back();
		dataStack.pop_back();
	}
}

bool Stream::Open(const std::string_view &filePath) {
	dataNode = &jsonData;
	dataStack.clear();
	isValid = false;

	std::ifstream file{std::string{filePath}};
	if(!file.is_open()) {
		LOG_ERROR("Stream: Failed to open {}", filePath);
		return isValid;
	}

	try {
		file >> jsonData;
	} catch(const json::exception &exception) {
		LOG_ERROR("Stream: JSON parsing error reading {}: {}", filePath, exception.what());
		return isValid;
	}

	isValid = true;
	return isValid;
}

bool Stream::ReadVec2(const std::string_view &key, glm::vec2 &value) const {
	return ReadVector<float>(key, NAMEOF(glm::vec2), [&value, &key] (const std::vector<float> &vector) {
		// Make sure it has enough data
		if(vector.size() < 2) {
			LOG_ERROR("Stream: JSON parsing error reading key {} as {}: Array doesn't contain 2 or more floats", key, NAMEOF(glm::vec2));
			return false;
		}

		// Read the data
		value.x = vector[0];
		value.y = vector[1];
		return true;
	});
}

bool Stream::ReadVec3(const std::string_view &key, glm::vec3 &value) const {
	return ReadVector<float>(key, NAMEOF(glm::vec3), [&value, &key](const std::vector<float> &vector) {
		// Make sure it has enough data
		if(vector.size() < 3) {
			LOG_ERROR("Stream: JSON parsing error reading key {} as {}: Array doesn't contain 3 or more floats", key, NAMEOF(glm::vec3));
			return false;
		}

		// Read the data
		value.x = vector[0];
		value.y = vector[1];
		value.z = vector[2];
		return true;
	});
}

bool Stream::ReadVec4(const std::string_view &key, glm::vec4 &value) const {
	return ReadVector<float>(key, NAMEOF(glm::vec4), [&value, &key] (const std::vector<float> &vector) {
		// Make sure it has enough data
		if(vector.size() < 4) {
			LOG_ERROR("Stream: JSON parsing error reading key {} as {}: Array doesn't contain 4 or more floats", key, NAMEOF(glm::vec4));
			return false;
		}

		// Read the data
		value.x = vector[0];
		value.y = vector[1];
		value.z = vector[2];
		value.w = vector[3];
		return true;
	});
}

}
