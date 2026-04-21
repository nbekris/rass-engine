// File Name:    Button.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      UI button component handling click interaction.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <glm/vec2.hpp>
#include <string>
#include <string_view>

#include "Cloneable.h"
#include "Component.h"
#include "Events/EventArgs.h"
#include "Systems/UI/IUiSystem.h"

// forward declaration
namespace RassEngine {
class Stream;
}

namespace RassEngine::Components {

class Button : public Cloneable<Component, Button> {
public:

	Button();
	Button(const Button& other);
	virtual ~Button() override;

	// Inherited via Object
	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;

	// Inherited via Component
	virtual bool Read(Stream& stream) override;	

	inline bool IsInteractable() const {
		return isInteractable;
	}
	inline bool IsHighlighted() const {
		return isHighlighted;
	}
	inline const std::string &GetText() const {
		return text;
	}
	void SetText(const std::string &text);
	void SetInteractable(bool flag);
	void SetHighlighted(bool flag);

	struct EventArgs : public RassEngine::Events::EventArgs {
		friend class Button;
		const Button &source;
		inline EventArgs(const Button &source)
			: RassEngine::Events::EventArgs{}
			, source{source}
		{}
	};
private:
	bool isInteractable{true};
	bool isHighlighted{false};
	std::string text{};
	glm::vec2 size{50.f, 20.f};
	glm::vec2 pivot{0.5f, 0.5f};
	Systems::IUiSystem::UiElement uiElement{};
};

}
