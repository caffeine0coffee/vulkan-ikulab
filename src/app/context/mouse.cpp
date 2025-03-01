#include "mouse.hpp"

void Mouse::reset() {
    scroll_offset_x_ = 0;
    scroll_offset_y_ = 0;
    delta_x_ = 0;
    delta_y_ = 0;
}