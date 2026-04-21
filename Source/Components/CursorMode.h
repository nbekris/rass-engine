// File Name:    CursorMode.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component controlling the mouse cursor display mode.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

#include "Cloneable.h"
#include "Component.h"

// forward declaration
namespace RassEngine {
class Stream;
}

namespace RassEngine::Components {

// forward declarations
class CursorMode : public Cloneable<Component, CursorMode> {
public:

	CursorMode();
	CursorMode(const CursorMode& other);
	virtual ~CursorMode() override;

	// Inherited via Object
	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;

	// Inherited via Component
	virtual bool Read(Stream& stream) override;

private:
	bool isCursorVisible{true};
};

}
