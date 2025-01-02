#include "./window.hpp"

#include <easylogging++.h>

namespace ikura {
bool Window::closed() { return false; }

Window::~Window() {}

void Window::destroyResources() {}

// Getters ----------

const int Window::getNumOfFrames() const { return numOfFrames; }

const int Window::getWidth() const { return width; }

const int Window::getHeight() const { return height; }

const std::shared_ptr<RenderTarget> &Window::getRenderTarget() {
    return renderTarget;
}

const std::shared_ptr<RenderContent> &Window::getRenderContent() {
    return renderContent;
}

// Setters ----------

void Window::setRenderTarget(std::shared_ptr<RenderTarget> renderTarget) {
    this->renderTarget = renderTarget;
}

void Window::setRenderContent(std::shared_ptr<RenderContent> renderContent) {
    this->renderContent = renderContent;
}
} // namespace ikura