// File Name:    SpriteSwapper.h
// Author(s):    main Niko Bekris, secondary Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component swapping sprites in response to game events.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <Cloneable.h>
#include <Component.h>
#include <vector>
#include <memory>
#include <Stream.h>
#include <string>
#include <string_view>
#include <Components/Sprite.h>

namespace RassGame::Components {

class SpriteSwapper : public RassEngine::Cloneable<RassEngine::Component, SpriteSwapper> {
public:
	SpriteSwapper();
	SpriteSwapper(const SpriteSwapper &other);
	virtual ~SpriteSwapper() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

	inline unsigned char NumberOfSprites() const {
		return static_cast<unsigned char>(paths.size());
	}
	inline unsigned char GetIndex() const {
		return index;
	}
	void SetIndex(unsigned char newIndex);

private:
	unsigned char index{0};
	std::vector<std::string> paths{};
	RassEngine::Components::Sprite *sprite{nullptr};
};

}
