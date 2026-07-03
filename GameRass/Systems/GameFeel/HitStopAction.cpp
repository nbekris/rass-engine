#include <Precompiled.h>
#include "HitStopAction.h"
#include "IGameFeelFactory.h"
#include <Stream.h>

using namespace RassEngine;

namespace RassGame::Systems {
bool HitStopAction::Read(Stream &stream) {
	stream.Read("Duration", duration);
	stream.Read("TimeScale", timeScale);
	return true;
}
void HitStopAction::Execute(const FeelContext &) {
	if(auto *factory = IGameFeelFactory::Get()) {
		factory->RequestHitStop(duration, timeScale);   // factory owns restore timing
	}
}
}
