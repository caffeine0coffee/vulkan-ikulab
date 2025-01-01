#pragma once

#include <array>

#include <ikura/external/ikura_ext_imgui/imgui.h>

class UI {
  public:
    static void makePadding(int pad);

    struct AnimationControlWindow {
        bool windowInitialized = false;
        bool show = true;

        const std::array<const char *, 2> MODE_ITEMS = {"Normal", "Edit"};
        static const int MODE_INDEX_NORMAL = 0;
        static const int MODE_INDEX_EDIT = 1;
        int modeIndex = 0;

        bool isSeekBarDragging;
    } animationControlWindow;

    struct DebugWindow {
        bool sizeInitialized = false;
        bool show = false;
    } debugWindow;

    struct Config {
        const char *rotationOrderComboItems[6] = {"X-Y-Z", "X-Z-Y", "Y-X-Z",
                                                  "Y-Z-X", "Z-X-Y", "Z-Y-X"};
        int rotationOrderIndex = -1;
        bool exportAllPositionChannel = false;
    } config;

    bool showImGuiDemoWindow = false;
    bool showFloor = true;
    bool showAxisObject = false;
    bool enableVsinc = true;
};
