#pragma once

#include "Cloneable.h"
#include "Component.h"
#include "Stream.h"

#include "Systems/Render/IRenderSystem.h"

namespace RassEngine::Components {
class PostProcessSettings : public Cloneable<Component, PostProcessSettings> {
public:
	PostProcessSettings() = default;
	PostProcessSettings(const PostProcessSettings &other) = default;
	virtual ~PostProcessSettings() override = default;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;

private:
	Systems::IRenderSystem::BloomSettings bloom_{};
};
}
