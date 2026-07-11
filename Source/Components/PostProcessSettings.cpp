#include "Precompiled.h"
#include "Components/PostProcessSettings.h"

#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Render/IRenderSystem.h"

namespace RassEngine::Components {
using namespace RassEngine::Systems;

bool PostProcessSettings::Initialize() {
    if (IRenderSystem::Get() == nullptr) {
        LOG_WARNING("{}: {} not registered; settings ignored", NameClass(), NAMEOF(IRenderSystem));
        return false;
    }
    IRenderSystem::Get()->SetBloomSettings(bloom_);
    return true;
}

const std::string_view &PostProcessSettings::NameClass() const {
    static constexpr std::string_view className = NAMEOF(RassEngine::Components::PostProcessSettings);
    return className;
}

bool PostProcessSettings::Read(Stream &stream) {
    if (!Component::Read(stream)) {
        return false;
    }
    stream.Read("BloomEnabled",    bloom_.enabled);
    stream.Read("BloomThreshold",  bloom_.threshold);
    stream.Read("BloomIntensity",  bloom_.intensity);
    stream.Read("BloomBlurPasses", bloom_.blurPasses);
    stream.Read("Hdr", bloom_.hdr);
    return true;
}
}