#include "mouse.hpp"

void Mouse::Reset() {
    scroll_offset_x_ = 0;
    scroll_offset_y_ = 0;
    delta_x_ = 0;
    delta_y_ = 0;
}

void Mouse::InitDragPosition() {
    set_drag_start_x(current_x());
    set_drag_start_y(current_y());
}
