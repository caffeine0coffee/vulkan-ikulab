//
// Created by Caffeine on 24/12/22.
//

#pragma once

#include <easylogging++.h>

#include "./resourceDirectory.hpp"

#ifdef IS_WINDOWS
#include <windows.h>
#endif

#ifdef __APPLE__
#include <unistd.h>
#endif

#ifdef IS_WINDOWS

static void spawnVersionCheckerProcess() {
    auto versionCheckerExePath = getReadOnlyResourceDirectory() / "imv_version_checker.exe";
    versionCheckerExePath.make_preferred();

    auto versionInfoDirPath = getWritableResourceDirectory() / "version_info";
    versionInfoDirPath.make_preferred();

    ShellExecute(nullptr, "open",
                 reinterpret_cast<LPCSTR>(versionCheckerExePath.string().c_str()),
                 reinterpret_cast<LPCSTR>(versionInfoDirPath.string().c_str()),
                 nullptr,
                 SW_HIDE);
    if (const auto result = GetLastError(); result < 32) {
        LOG(ERROR) << "Failed to launch version-checker.exe: " << result;
    }

}

#endif

#ifdef __APPLE__

static void spawnVersionCheckerProcess() {
    pid_t pid = fork();

    if (pid == 0) {
        // child process
        auto versionCheckerExePath = getReadOnlyResourceDirectory() / "imv_version_checker";
        versionCheckerExePath.make_preferred();

        auto versionInfoDirPath = getWritableResourceDirectory() / "version_info";
        versionInfoDirPath.make_preferred();

        const auto arg0 = versionCheckerExePath.string();
        const auto arg1 = versionInfoDirPath.string();
        execl(arg0.c_str(), arg0.c_str(), arg1.c_str(), nullptr);
    } else if (pid > 0) {
        LOG(INFO) << "Version checker process spawned.";
    } else {
        LOG(ERROR) << "Failed to fork a process for version checker.";
    }
}

#endif
