#pragma once

#include <atomic>

class Keyboard {
  public:
    [[nodiscard]] bool ctrl() const { return ctrl_; }
    [[nodiscard]] bool alt() const { return alt_; }
    [[nodiscard]] bool shift() const { return shift_; }

    void set_ctrl(const bool ctrl) { ctrl_ = ctrl; }
    void set_alt(const bool alt) { alt_ = alt; }
    void set_shift(const bool shift) { shift_ = shift; }

  private:
    std::atomic<bool> ctrl_ = false;
    std::atomic<bool> alt_ = false;
    std::atomic<bool> shift_ = false;
};
