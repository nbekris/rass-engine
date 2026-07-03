#include <Precompiled.h>
#include "ScreenFlashAction.h"
#include "IGameFeelFactory.h"
#include <Stream.h>

using namespace RassEngine;

namespace RassGame::Systems {
bool ScreenFlashAction::Read(Stream &stream) {
	stream.ReadVec3("Color", color);
	curve.Read(stream);   // same curve keys SpriteFader already reads
	return true;
}

void ScreenFlashAction::Execute(const FeelContext &) {
	if(auto *factory = IGameFeelFactory::Get()) {
		factory->Show(color, curve);
	}
}
}
