#pragma once

#include <memory>

#include <ikura.hpp>

#include "./context/camera.hpp"
#include "./context/keyboard.hpp"
#include "./context/mouse.hpp"

class App {
    // Variables ==========
    // ikura objects ----------
    std::unique_ptr<ikura::AppEngine> appEngine;
    std::shared_ptr<ikura::RenderEngine> renderEngine;

    std::shared_ptr<ikura::GlfwNativeWindow> mainWindow;
    std::shared_ptr<ikura::BasicRenderTarget> renderTarget;
    std::shared_ptr<ikura::BasicRenderContent> renderContent;

    std::shared_ptr<ikura::BasicRenderComponentProvider> basicRenderComponentProvider;

    // Contexts ----------
    Camera camera;
    Keyboard keyboard;
    Mouse mouse;

    // Functions ==========
    // Init ----------
    void initIkura();
    void setShapes();
    void setGlfwWindowEvents();

    // Glfw Callbacks ----------
    static void cursorPositionCallback(GLFWwindow *window, double xPos,
                                       double yPos);
    static void mouseButtonCallback(GLFWwindow *window, int button, int action,
                                    int mods);
    static void scrollCallback(GLFWwindow *window, double xOffset,
                               double yOffset);
    static void keyCallback(GLFWwindow *window, int key, int scanCode,
                            int action, int mods);

  public:
    App();
    void run();
};