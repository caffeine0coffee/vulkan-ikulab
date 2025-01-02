#pragma once

class Mouse {
  public:
    bool leftButton = false;
    bool rightButton = false;
    bool middleButton = false;

    double dragStartX = 0.0;
    double dragStartY = 0.0;
    double dragEndX = 0.0;
    double dragEndY = 0.0;

    double currentX = 0.0;
    double currentY = 0.0;

    double deltaX = 0.0;
    double deltaY = 0.0;

    double scrollOffsetX = 0.0;
    double scrollOffsetY = 0.0;

    void reset();
};