//
// Created by kota.miyakawa on 2024/07/10.
//

#pragma once

#include <string>

void showInfoPopup(const std::string& message, const std::string& title = "Info");

void showWarningPopup(const std::string& message, const std::string& title = "Warning");

void showErrorPopup(const std::string& message, const std::string& title = "Error");
