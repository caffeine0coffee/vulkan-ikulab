#include "ui.hpp"

#include <ikura_ext_imgui/imgui.h>

void UI::makePadding(int pad) {
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (pad));
}