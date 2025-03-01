#pragma once

#include <chrono>
#include <memory>
#include <vector>

#include <GLFW/glfw3.h>

#include "renderEngine/renderEngine.hpp"
#include "window/nativeWindow/glfwNativeWindow.hpp"
#include "window/virtualWindow/imGuiVirtualWindow.hpp"

namespace ikura {
class AppEngine {
    std::shared_ptr<RenderEngine> renderEngine;
    std::vector<std::shared_ptr<NativeWindow>> nativeWindows;

    float fps = 60.0;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point currentTime;

    float secondsFromStart;
    float deltaTime;

  public:
    AppEngine(std::shared_ptr<RenderEngine> renderEngine);

    void addWindow(std::shared_ptr<GlfwNativeWindow> glfwNativeWindow);

    void vSync();
    void setStartTime();
    float getSecondsFromStart() const;
    float getDeltaTime() const;

    int shouldTerminated();
    void drawAllWindows();
    void destroyClosedWindow();
};
} // namespace ikura