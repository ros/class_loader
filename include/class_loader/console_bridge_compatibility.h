#ifndef CLASS_LOADER__CONSOLE_BRIDGE_COMPATIBILITY_H
#define CLASS_LOADER__CONSOLE_BRIDGE_COMPATIBILITY_H

#include <console_bridge/console.h>

#ifndef CONSOLE_BRIDGE_logError
#define CONSOLE_BRIDGE_logError logError
#endif

#ifndef CONSOLE_BRIDGE_logWarn
#define CONSOLE_BRIDGE_logWarn logWarn
#endif

#ifndef CONSOLE_BRIDGE_logDebug
#define CONSOLE_BRIDGE_logDebug logDebug
#endif

#ifndef CONSOLE_BRIDGE_logInform
#define CONSOLE_BRIDGE_logInform logInform
#endif

#endif  // CLASS_LOADER__CONSOLE_BRIDGE_COMPATIBILITY_H
