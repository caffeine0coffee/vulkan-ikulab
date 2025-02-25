//
// Created by caffeine on 25/02/24.
//

#pragma once

#include <atomic>
#include <vector>

class InputContext {
  public:
    // key_pressing_list_ getter
    std::vector<int> get_key_pressing_list() {
        return key_pressing_list_.load();
    }

  private:
    std::atomic<std::vector<int>> key_pressing_list_;
    std::atomic<std::vector<int>> mouse_key_pressing_list_;
};
