#pragma once

class Mouse {
  public:
    bool left_button_ = false;
    bool right_button_ = false;
    bool middle_button_ = false;

    double drag_start_x_ = 0.0;
    double drag_start_y_ = 0.0;
    double drag_end_x_ = 0.0;
    double drag_end_y_ = 0.0;

    double current_x_ = 0.0;
    double current_y_ = 0.0;

    double delta_x_ = 0.0;
    double delta_y_ = 0.0;

    double scroll_offset_x_ = 0.0;
    double scroll_offset_y_ = 0.0;

    void reset();
};
