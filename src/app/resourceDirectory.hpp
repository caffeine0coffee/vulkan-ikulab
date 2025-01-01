#pragma once
#include <cstdlib>
#include <filesystem>
#include <string>

std::filesystem::path getReadOnlyResourceDirectory();
std::filesystem::path getWritableResourceDirectory();

std::filesystem::path getHomeDirectory();