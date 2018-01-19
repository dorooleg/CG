#pragma once

#include "types.h"
#include "callbacks.h"

void GLUTBackendRun(ICallbacks* pCallbacks);

PCDEV_KEY GLUTKeyToPCDEVKey(uint Key);
