#include "./ui.hpp"

void UI::makePadding(int pad) {
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (pad));
}