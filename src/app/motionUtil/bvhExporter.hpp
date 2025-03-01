#pragma once

#include <filesystem>

#include "animator.hpp"

void exportLoopRangeToBvhFile(const std::shared_ptr<Animator> animator,
                              std::filesystem::path destFile,
                              bool exportAllPositionChennel);
