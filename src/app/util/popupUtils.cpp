//
// Created by kota.miyakawa on 2024/07/10.
//

#include "popupUtils.hpp"

#include <tinyfiledialogs.h>

void showInfoPopup(const std::string& message, const std::string& title) {
    tinyfd_messageBox(title.c_str(), message.c_str(), "ok", "info", 1);
}

void showWarningPopup(const std::string& message, const std::string& title) {
    tinyfd_messageBox(title.c_str(), message.c_str(), "ok", "warning", 1);
}

void showErrorPopup(const std::string& message, const std::string& title) {
    tinyfd_messageBox(title.c_str(), message.c_str(), "ok", "error", 1);
}
