// File Name:    FlipOrigin.h
// Author(s):    main Taro Omiya, secondary Niko Bekris, Steven Yacoub
// Course:       GAM541
// Project:      RASS
// Purpose:      Component defining the origin point for flip mechanics.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <Cloneable.h>
#include <Component.h>
#include <Components/Transform.h>
#include <Events/EventArgs.h>
#include <Events/GlobalEventArgs.h>
#include <Events/GlobalEventListener.h>
#include <IEvent.h>
#include <IEventListener.h>
#include <memory>
#include <Stream.h>
#include <string>
#include <string_view>

namespace RassGame::Components {
class FlipOrigin : public RassEngine::Cloneable<RassEngine::Component, FlipOrigin> {
	static constexpr float ANIMATION_DURATION_SECONDS = 0.75f;
public:
	// Arguments for flip events
	class FlipEventArgs : public RassEngine::Events::EventArgs {
		friend class FlipOrigin;
		inline FlipEventArgs(RassEngine::Components::Transform *animatedTransform, bool isRightSideUp)
			: RassEngine::Events::EventArgs{}
			, animatedTransform{animatedTransform}
			, isRightSideUp{isRightSideUp} {}

	public:
		RassEngine::Components::Transform *const animatedTransform{nullptr};
		const bool isRightSideUp{true};
	};

	FlipOrigin();
	FlipOrigin(const FlipOrigin &other);
	virtual ~FlipOrigin() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

	bool StartFlip();

	inline bool IsRightSideUp() const {
		return isRightSideUp;
	}
	inline bool IsAnimating() const {
		return isAnimating;
	}
	inline void BindOnFlipStart(RassEngine::IEventListener<FlipEventArgs> *listener) {
		onFlipStart->bind(listener);
	}
	inline void UnbindOnFlipStart(RassEngine::IEventListener<FlipEventArgs> *listener) {
		onFlipStart->unbind(listener);
	}
	inline void BindOnFlipEnd(RassEngine::IEventListener<FlipEventArgs> *listener) {
		onFlipEnd->bind(listener);
	}
	inline void UnbindOnFlipEnd(RassEngine::IEventListener<FlipEventArgs> *listener) {
		onFlipEnd->unbind(listener);
	}

private:
	bool Update(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *, const RassEngine::Events::GlobalEventArgs &);
private:
	RassEngine::Events::GlobalEventListener<FlipOrigin> onUpdateListener;
	std::unique_ptr<RassEngine::IEvent<FlipEventArgs>> onFlipStart, onFlipEnd;
	std::string entityToFollowName{};
	RassEngine::Components::Transform *transformToFollow{nullptr};
	bool isAnimating{false}, isRightSideUp{true};
};

}
