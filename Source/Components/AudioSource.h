// File Name:    AudioSource.h
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component for playing audio at an entity location.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once
#include "Component.h"
#include "Cloneable.h"

namespace RassEngine::Components {
	class AudioSource : public Cloneable<Component, AudioSource>{
		public:
			AudioSource();
			AudioSource(const AudioSource &other);
			virtual ~AudioSource() override;
			virtual bool Initialize() override;
			virtual const std::string_view &NameClass() const override;
			virtual bool Read(Stream &stream) override;

			inline const std::string_view& GetFile() const {
				return fileName;
			}

			void Play() const;
			void Stop() const;
		private:
			std::string fileName;
			float volume{1.0f};
			float pitch{1.0f};
			bool isLooping{false};
			bool isSpatial{false};
	};
}

