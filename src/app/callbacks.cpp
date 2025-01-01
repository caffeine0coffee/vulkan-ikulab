#include "./app.hpp"

void App::cursorPositionCallback(GLFWwindow *window, double xPos, double yPos) {
    App *app = static_cast<App *>(glfwGetWindowUserPointer(window));

    app->mouse->deltaX = xPos - app->mouse->currentX;
    app->mouse->deltaY = yPos - app->mouse->currentY;

    app->mouse->currentX = xPos;
    app->mouse->currentY = yPos;

    // record drag end position
    if (app->mouse->leftButton) {
        app->mouse->dragEndX = xPos;
        app->mouse->dragEndY = yPos;
    }
}

void App::mouseButtonCallback(GLFWwindow *window, int button, int action,
                              int mods) {
    App *app = static_cast<App *>(glfwGetWindowUserPointer(window));
    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        app->mouse->leftButton = (action == GLFW_PRESS);
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        app->mouse->rightButton = (action == GLFW_PRESS);
        break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        app->mouse->middleButton = (action == GLFW_PRESS);
        break;
    default:
        break;
    }

    // init drag position
    if (app->mouse->leftButton) {
        app->mouse->dragStartX = app->mouse->currentX;
        app->mouse->dragStartY = app->mouse->currentY;
        app->mouse->dragEndX = app->mouse->currentX;
        app->mouse->dragEndY = app->mouse->currentY;
    }
}

void App::scrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
    App *app = static_cast<App *>(glfwGetWindowUserPointer(window));
    app->mouse->scrollOffsetX = xOffset;
    app->mouse->scrollOffsetY = yOffset;
}

void App::keyCallback(GLFWwindow *window, int key, int scanCode, int action,
                      int mods) {
    App *app = static_cast<App *>(glfwGetWindowUserPointer(window));

    switch (key) {
    case GLFW_KEY_LEFT_CONTROL:
    case GLFW_KEY_RIGHT_CONTROL:
        app->keyboard->ctrl = (action != GLFW_RELEASE);
        break;
    case GLFW_KEY_LEFT_ALT:
    case GLFW_KEY_RIGHT_ALT:
        app->keyboard->alt = (action != GLFW_RELEASE);
        break;
    case GLFW_KEY_LEFT_SHIFT:
    case GLFW_KEY_RIGHT_SHIFT:
        app->keyboard->shift = (action != GLFW_RELEASE);
        break;
    default:
        break;
    }
}
