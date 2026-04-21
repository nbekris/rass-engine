// File Name:    MusicFader.h
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that fades music volume in or out.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once
#include "Component.h"
#include "Cloneable.h"

namespace RassEngine::Components {
	class MusicFader : public Cloneable<Component, MusicFader>{
		public:
			MusicFader();
			MusicFader(const MusicFader &other);
			virtual ~MusicFader() override;
			virtual bool Initialize() override;
			virtual const std::string_view &NameClass() const override;
			virtual bool Read(Stream &stream) override;

			inline const std::string_view& GetFile() const {
				return fileName;
			}

			virtual void Play() const;
			virtual void Stop() const;
		private:
			std::string fileName;
			float volume{1.0f};
			float pitch{1.0f};
			bool isLooping{true};
			bool playOnInit{false};
	};
}

