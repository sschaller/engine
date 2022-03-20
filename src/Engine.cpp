#include "Engine.h"

#include "DeviceContext.h"
#include "Window.h"


Engine::Engine(DeviceContext &rContext, Window &rWindow) : pContext_(&rContext), pWindow_(&rWindow) {}

void Engine::Render() {}