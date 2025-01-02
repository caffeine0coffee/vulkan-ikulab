#include "init.hpp"

#include <easylogging++.h>

#include "./common/logLevels.hpp"

namespace ikura {
void init() {
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "Initializing ikura...";
    VLOG(VLOG_LV_3_PROCESS_TRACKING) << "ikura has been initialized.";
}
} // namespace ikura
