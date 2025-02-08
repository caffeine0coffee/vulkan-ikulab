//
// Created by kota.miyakawa on 2024/07/11.
//

#include "errorUtils.hpp"

#include "popupUtils.hpp"

void notifyErrorAndExit(const std::string &message) {
    showErrorPopup(message);
    
    LOG(ERROR) << message;
    el::Loggers::flushAll();

    exit(EXIT_FAILURE);
}