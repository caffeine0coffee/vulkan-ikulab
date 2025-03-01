#include "app.hpp"

void App::cursorPositionCallback(GLFWwindow *window, double xPos, double yPos) {
    App *app = static_cast<App *>(glfwGetWindowUserPointer(window));

    app->mouse->set_delta_x(xPos - app->mouse->current_x());
    app->mouse->set_delta_y(yPos - app->mouse->current_y());

    app->mouse->set_current_x(xPos);
    app->mouse->set_current_y(yPos);
}

void App::mouseButtonCallback(GLFWwindow *window, int button, int action,
                              int mods) {
    App *app = static_cast<App *>(glfwGetWindowUserPointer(window));
    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        app->mouse->set_left_button(action == GLFW_PRESS);
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        app->mouse->set_right_button(action == GLFW_PRESS);
        break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        app->mouse->set_middle_button(action == GLFW_PRESS);
        break;
    default:
        break;
    }

    // init drag position
    if (app->mouse->left_button()) {
        app->mouse->InitDragPosition();
    }
}

void App::scrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
    App *app = static_cast<App *>(glfwGetWindowUserPointer(window));
    app->mouse->set_scroll_offset_x(xOffset);
    app->mouse->set_scroll_offset_y(yOffset);
}

void App::keyCallback(GLFWwindow *window, int key, int scanCode, int action,
                      int mods) {
    App *app = static_cast<App *>(glfwGetWindowUserPointer(window));

    switch (key) {
    case GLFW_KEY_LEFT_CONTROL:
    case GLFW_KEY_RIGHT_CONTROL:
        app->keyboard->set_ctrl(action != GLFW_RELEASE);
        break;
    case GLFW_KEY_LEFT_ALT:
    case GLFW_KEY_RIGHT_ALT:
        app->keyboard->set_alt(action != GLFW_RELEASE);
        break;
    case GLFW_KEY_LEFT_SHIFT:
    case GLFW_KEY_RIGHT_SHIFT:
        app->keyboard->set_shift(action != GLFW_RELEASE);
        break;
    default:
        break;
    }
}
