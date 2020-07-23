#include "BasicVariableInfo.h"

bool BasicVariableInfo::isFullused() {
    for (int i = 0; i < ARGS_COUNT; i++) {
            if (is_used[i] != true) {
                return false;
            }
        }
    return true;
}
