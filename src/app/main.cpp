#include "app.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <easylogging++.h>

#include "./resourceDirectory.hpp"
#include "./versionChecker.hpp"
#include "./util/errorUtils.hpp"

INITIALIZE_EASYLOGGINGPP

#ifdef IS_WINDOWS
#include <windows.h>
#endif

void initEasyloggingpp() {
    el::Configurations conf;
    conf.setToDefault();

#ifdef __APPLE__
    auto logFilePath =
        getHomeDirectory() / "Library" / "Logs" / "ikulab-motion-viewer.log";
    conf.set(el::Level::Global, el::ConfigurationType::Filename, logFilePath);
#elif IS_WINDOWS
    auto logFilePath = getReadOnlyResourceDirectory() / "ikulab-motion-viewer.log";
    conf.set(el::Level::Global, el::ConfigurationType::Filename,
             logFilePath.string());
#endif

    el::Loggers::reconfigureAllLoggers(conf);
}

int main(int argc, char **argv) {
    initEasyloggingpp();

    if (argc > 1) {
        int vLevel = std::stoi(argv[1]);
        el::Loggers::setVerboseLevel(vLevel);
    }

    el::Loggers::setVerboseLevel(9);

    LOG(ERROR) << "test";

    spawnVersionCheckerProcess();

    try {
        App app;
        std::cout << "Hello Ikura!!" << std::endl;
        app.run();
    } catch (const std::runtime_error &e) {
        std::string msg;
        msg += "Caught a Runtime Error: ";
        msg += e.what();

        notifyErrorAndExit(msg);
    } catch (const std::exception &e) {
        std::string msg;
        msg += "Caught a Fatal Error: ";
        msg += e.what();

        notifyErrorAndExit(msg);
    }

    return EXIT_SUCCESS;
}

#ifdef IS_WINDOWS
/**
 * @brief Entry point for Windows.
 *
 * Convert the command line arguments to char* list and call main().
 */
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow) {
    auto cmdLine = GetCommandLineW();

    int argc;
    auto argvWin32 = CommandLineToArgvW(cmdLine, &argc);

    std::vector<char *> argv;
    for (int i = 0; i < argc; i++) {
        auto arg = argvWin32[i];
        auto len = wcslen(arg);
        auto buf = new char[len + 1];
        wcstombs(buf, arg, len);
        buf[len] = '\0';
        argv.push_back(buf);
    }

    return main(argc, argv.data());
}
#endif
