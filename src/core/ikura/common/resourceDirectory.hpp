#pragma once

#include <filesystem>
#include <iostream>

namespace ikura {
// subPath is NOT starts with '/'
// ex) "dirA/dirB/aaa.txt", "dirA/destinationDir"
std::filesystem::path
createResourceDirectoryPath(std::filesystem::path subPath);

} // namespace ikura