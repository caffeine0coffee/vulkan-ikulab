#include "./appEngine.hpp"

#include <algorithm>
#include <iostream>
#include <thread>

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>

#include "../shape/shapes.hpp"

namespace ikura {
AppEngine::AppEngine(std::shared_ptr<RenderEngine> renderEngine) {
    this->renderEngine = renderEngine;
}

void AppEngine::addWindow(std::shared_ptr<GlfwNativeWindow> glfwNativeWindow) {
    nativeWindows.push_back(glfwNativeWindow);
}

void AppEngine::vSync() {
    auto rightNow = std::chrono::high_resolution_clock::now();

    // time taken to prev drawing process
    // currentTime must be updated previous frame
    auto delta = std::chrono::duration<float, std::chrono::nanoseconds::period>(
        rightNow - currentTime);

    auto waitTime =
        std::chrono::duration<float, std::chrono::nanoseconds::period>(
            std::chrono::nanoseconds(
                uint32_t(1000.0 * 1000.0 * 1000.0 * 1.0 / fps)) -
            delta);

    std::this_thread::sleep_for(waitTime);

    // update deltaTime for main-loop use
    deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(
                    std::chrono::high_resolution_clock::now() - currentTime)
                    .count();
    // update currentTime for next vSync
    currentTime = std::chrono::high_resolution_clock::now();
    secondsFromStart =
        std::chrono::duration<float, std::chrono::seconds::period>(currentTime -
                                                                   startTime)
            .count();
}

void AppEngine::setStartTime() {
    startTime = std::chrono::high_resolution_clock::now();
}

float AppEngine::getSecondsFromStart() const { return secondsFromStart; }

float AppEngine::getDeltaTime() const { return deltaTime; }

int AppEngine::shouldTerminated() {
    return std::all_of(nativeWindows.begin(), nativeWindows.end(),
                       [&](const std::shared_ptr<NativeWindow> window) {
                           return window->closed();
                       });
}

void AppEngine::drawAllWindows() {
    // Poll all GLFW Window Events (execute once per loop)
    for (const auto &window : nativeWindows) {
        auto pw = dynamic_cast<GlfwNativeWindow *>(window.get());
        if (pw != nullptr) {
            glfwPollEvents();
            break;
        }
    }

    for (auto &window : nativeWindows) {
        window->draw();
    }
}

void AppEngine::destroyClosedWindow() {
    auto endIter = std::remove_if(nativeWindows.begin(), nativeWindows.end(),
                                  [](std::shared_ptr<NativeWindow> window) {
                                      bool closed = window->closed();
                                      if (closed) {
                                          window->destroyResources();
                                      }
                                      return closed;
                                  });

    nativeWindows.assign(nativeWindows.begin(), endIter);
}
} // namespace ikura