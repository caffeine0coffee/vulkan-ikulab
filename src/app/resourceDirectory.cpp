#include "./resourceDirectory.hpp"

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <mach-o/dyld.h>
#elif IS_WINDOWS
#include <windows.h>
#include <shlobj.h>
#endif

#include "./util/errorUtils.hpp"

// ------------------------------------------------------------
// getReadOnlyResourceDirectory
// getWritableResourceDirectory
// ------------------------------------------------------------

#ifdef RESOURCE_DIR

std::filesystem::path getReadOnlyResourceDirectory() {
    std::filesystem::path resourceDir = RESOURCE_DIR;

    return resourceDir;
}

std::filesystem::path getWritableResourceDirectory() {
    return getReadOnlyResourceDirectory();
}

#else

#ifdef __linux__

std::filesystem::path getReadOnlyResourceDirectory() {
    std::filesystem::path resourceDir = "/usr/local/share/ikulab-motion-viewer";

    return resourceDir;
}

std::filesystem::path getWritableResourceDirectory() {
    return getReadOnlyResourceDirectory();
}

#elif __APPLE__

/**
 * @brief Check if the application is launched from a bundle.
 * i.e. if the application launched by double-clicking .app file from Finder.
 * 
 * @return true if the application is launched from a bundle.
 
 */
bool launchedFromBundle() {
    char result[PATH_MAX];
    uint32_t size = PATH_MAX;
    if (_NSGetExecutablePath(result, &size) == 0) {
        auto resultString = std::string(result);
        return resultString.find(".app/Contents/MacOS/") != std::string::npos;
    }

    return false;
}

/**
 * @brief Get the resource directory from the bundle.
 * i.g. /path/to/ikulab-motion-viewer.app/Contents/Resources
 * 
 * @return path to Resources directory in the bundle.
 */
std::filesystem::path getBundleResourceDirectory() {
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);

    char path[PATH_MAX];
    if (CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path,
                                         PATH_MAX)) {
        CFRelease(resourcesURL);
        return path;
    }
    CFRelease(resourcesURL);
    return "";
}

/**
 * @brief Get read-only resource directory path.
 *
 * The read-only resource directory does not assume write permission.
 * On macOS, returns bundle resource directory if the application is launched from a bundle.
 * Otherwise, returns /usr/local/share/ikulab-motion-viewer.
 *
 * @return read-only resource directory path.
 */
std::filesystem::path getReadOnlyResourceDirectory() {
    if (launchedFromBundle()) {
        return getBundleResourceDirectory();
    } else {
        return "/usr/local/share/ikulab-motion-viewer";
    }
}

/**
 * @brief Get writable resource directory path.
 *
 * The writable resource directory assumes write permission.
 * On macOS, returns the same path as getReadOnlyResourceDirectory.
 *
 * @return writable resource directory path.
 */
std::filesystem::path getWritableResourceDirectory() {
    return getReadOnlyResourceDirectory();
}

#elif IS_WINDOWS

/**
 * @brief Get read-only resource directory path.
 *
 * The read-only resource directory does not assume write permission.
 * On Windows, returns the directory where the executable is located.
 *
 * @return read-only resource directory path.
 */
std::filesystem::path getReadOnlyResourceDirectory() {
    char buffer[MAX_PATH];
    GetModuleFileName(nullptr, buffer, MAX_PATH);
    const std::filesystem::path path = buffer;
    std::filesystem::path resourceDir = path.parent_path();

    return resourceDir;
}

/**
 * @brief Get writable resource directory path.
 *
 * The writable resource directory assumes write permission.
 * On Windows, returns %ALLUSERSPROFILE%/ikulab-motion-viewer directory.
 * Usually, %ALLUSERSPROFILE% is C:/ProgramData.
 *
 * @return writable resource directory path.
 */
std::filesystem::path getWritableResourceDirectory() {
    PWSTR programDataPathPwstr = nullptr;
    const auto result = SHGetKnownFolderPath(FOLDERID_ProgramData, 0, nullptr,
                                             &programDataPathPwstr);
    if (FAILED(result)) {
        notifyErrorAndExit("Failed to get ProgramData directory.");
    }

    const std::filesystem::path programDataPath = programDataPathPwstr;
    const auto resourceDir = programDataPath / "ikulab-motion-viewer";

    if (!std::filesystem::exists(resourceDir)) {
        notifyErrorAndExit("Writable resource directory does not exist.");
    }

    return resourceDir;
}

#endif

#endif

// ------------------------------------------------------------
// getHomeDirectory
// ------------------------------------------------------------

#ifdef __linux__

std::filesystem::path getHomeDirectory() { return getenv("HOME"); }

#elif __APPLE__

std::filesystem::path getHomeDirectory() { return getenv("HOME"); }

#elif IS_WINDOWS

std::filesystem::path getHomeDirectory() {

    std::filesystem::path homeDrive = getenv("HOMEDRIVE");
    std::filesystem::path homePath = getenv("HOMEPATH");

    return homeDrive / homePath;
}
#endif