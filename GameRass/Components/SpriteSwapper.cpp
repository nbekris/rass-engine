// File Name:    SpriteSwapper.cpp
// Author(s):    main Niko Bekris, secondary Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component swapping sprites in response to game events.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "SpriteSwapper.h"

#include <Cloneable.h>
#include <Component.h>
#include <Components/Sprite.h>
#include <Entity.h>
#include <memory>
#include <Stream.h>
#include <string>
#include <string_view>
#include <Systems/Logging/ILoggingSystem.h>
#include <Utils.h>

using namespace RassEngine;
using namespace RassEngine::Components;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;

namespace RassGame::Components {

SpriteSwapper::SpriteSwapper() : Cloneable<Component, SpriteSwapper>{}
	, index{0}, paths{}, sprite{nullptr}
{
	paths.reserve(2);
}

SpriteSwapper::SpriteSwapper(const SpriteSwapper &other) : Cloneable<Component, SpriteSwapper>{other}
	, index{other.index}, paths{other.paths}, sprite{nullptr}
{ }

bool SpriteSwapper::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: Entity was not found", NameClass());
		return false;
	}

	// Retrieve the sprite
	sprite = Parent()->Get<Sprite>();
	if(sprite == nullptr) {
		LOG_WARNING("{}: Sprite was not found", NameClass());
		return false;
	}
	return true;
}

SpriteSwapper::~SpriteSwapper() {}

const std::string_view &SpriteSwapper::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::SpriteSwapper);
	return className;
}

bool SpriteSwapper::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	// Read a list of strings
	paths.clear();
	stream.ReadArray("Paths", [this, &stream] () {
		std::string_view name;
		if(stream.ReadAs<std::string_view>(name)) {
			paths.emplace_back(std::string{name});
		}
		return true;
	});
	return true;
}

void SpriteSwapper::SetIndex(unsigned char newIndex) {
	if(newIndex >= NumberOfSprites()) {
		LOG_WARNING("{}: Attempted to set index {} which is out of bounds (number of sprites: {})", NameClass(), static_cast<std::byte>(newIndex), NumberOfSprites());
		return;
	} else if (sprite == nullptr) {
		LOG_WARNING("{}: Either not initialized yet, or not attached to an existing sprite ", NameClass());
		return;
	}

	// Update index and sprite
	index = newIndex;
	sprite->SetTexture(paths[index]);
}

}
