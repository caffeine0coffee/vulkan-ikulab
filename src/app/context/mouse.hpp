#pragma once

#include <atomic>

class Mouse {
  public:
    void Reset();

    [[nodiscard]] bool left_button() const { return left_button_; }
    [[nodiscard]] bool right_button() const { return right_button_; }
    [[nodiscard]] bool middle_button() const { return middle_button_; }

    [[nodiscard]] double drag_start_x() const { return drag_start_x_; }
    [[nodiscard]] double drag_start_y() const { return drag_start_y_; }
    [[nodiscard]] double drag_end_x() const { return drag_end_x_; }
    [[nodiscard]] double drag_end_y() const { return drag_end_y_; }

    [[nodiscard]] double current_x() const { return current_x_; }
    [[nodiscard]] double current_y() const { return current_y_; }

    [[nodiscard]] double delta_x() const { return delta_x_; }
    [[nodiscard]] double delta_y() const { return delta_y_; }

    [[nodiscard]] double scroll_offset_x() const { return scroll_offset_x_; }
    [[nodiscard]] double scroll_offset_y() const { return scroll_offset_y_; }

    void set_left_button(const bool left_button) { left_button_ = left_button; }
    void set_right_button(const bool right_button) {
        right_button_ = right_button;
    }
    void set_middle_button(const bool middle_button) {
        middle_button_ = middle_button;
    }
    void set_drag_start_x(const double drag_start_x) {
        drag_start_x_ = drag_start_x;
    }
    void set_drag_start_y(const double drag_start_y) {
        drag_start_y_ = drag_start_y;
    }
    void set_drag_end_x(const double drag_end_x) { drag_end_x_ = drag_end_x; }
    void set_drag_end_y(const double drag_end_y) { drag_end_y_ = drag_end_y; }
    void set_current_x(const double current_x) { current_x_ = current_x; }
    void set_current_y(const double current_y) { current_y_ = current_y; }
    void set_delta_x(const double delta_x) { delta_x_ = delta_x; }
    void set_delta_y(const double delta_y) { delta_y_ = delta_y; }
    void set_scroll_offset_x(const double scroll_offset_x) {
        scroll_offset_x_ = scroll_offset_x;
    }
    void set_scroll_offset_y(const double scroll_offset_y) {
        scroll_offset_y_ = scroll_offset_y;
    }

  private:
    std::atomic<bool> left_button_ = false;
    std::atomic<bool> right_button_ = false;
    std::atomic<bool> middle_button_ = false;

    std::atomic<double> drag_start_x_ = 0.0;
    std::atomic<double> drag_start_y_ = 0.0;
    std::atomic<double> drag_end_x_ = 0.0;
    std::atomic<double> drag_end_y_ = 0.0;

    std::atomic<double> current_x_ = 0.0;
    std::atomic<double> current_y_ = 0.0;

    std::atomic<double> delta_x_ = 0.0;
    std::atomic<double> delta_y_ = 0.0;

    std::atomic<double> scroll_offset_x_ = 0.0;
    std::atomic<double> scroll_offset_y_ = 0.0;
};
