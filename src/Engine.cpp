#include "Engine.h"

#include "DeviceContext.h"
#include "Window.h"

Engine::Engine(DeviceContext &rContext, Window &rWindow)
    : rContext_(rContext), rWindow_(rWindow), swapchain_(rContext, rWindow) {
}

Engine::~Engine() {
}

void Engine::Render() {
    // First update swapchain
    bool outOfDate = swapchain_.Update();

    // Do we need to update attachements etc also? -> swapchain_.Update could return true if resize is needed
    // In this case we would also resize attachements etc, but how?

    // First we need to gather any objects we want to render and put them into correct RenderPass -> Pipeline -> Materials -> Mesh

    // May need to read up on RenderPass vs SubPass, why can't I do everything with sub passes?

}
