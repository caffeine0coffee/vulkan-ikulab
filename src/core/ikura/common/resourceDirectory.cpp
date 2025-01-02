#include "resourceDirectory.hpp"

namespace ikura {

#ifdef __linux__
std::filesystem::path
createResourceDirectoryPath(std::filesystem::path subPath) {
    std::filesystem::path homeDir = getenv("HOME");
    std::filesystem::path resourceDir = homeDir / ".local" / "share" / "ikura";
    std::filesystem::path destinationPath = resourceDir / subPath;

    return destinationPath;
}
#endif

#ifdef __APPLE__
std::filesystem::path
createResourceDirectoryPath(std::filesystem::path subPath) {
    std::filesystem::path homeDir = getenv("HOME");
    std::filesystem::path resourceDir = homeDir / ".local" / "share" / "ikura";
    std::filesystem::path destinationPath = resourceDir / subPath;

    return destinationPath;
}
#endif

#ifdef IS_WINDOWS
std::filesystem::path
createResourceDirectoryPath(std::filesystem::path subPath) {
    std::filesystem::path homeDrive = getenv("HOMEDRIVE");
    std::filesystem::path homePath = getenv("HOMEPATH");
    std::filesystem::path resourceDir = homeDrive / homePath / ".ikura";
    std::filesystem::path destinationPath = resourceDir / subPath;

    return destinationPath;
}
#endif

} // namespace ikura