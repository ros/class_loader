#ifndef PLUGIN_REGISTER_MACRO_H_DEFINED
#define PLUGIN_REGISTER_MACRO_H_DEFINED

#include "plugins_core.h"

#define PLUGINS_REGISTER_CLASS_INTERNAL(Derived, Base, UniqueID) \
  struct ProxyExec##UniqueID \
  { \
     typedef  Derived _derived; \
     typedef  Base    _base; \
     ProxyExec##UniqueID() \
     { \
       plugins::plugins_private::registerPlugin<_derived, _base>(#Derived); \
     } \
  }; \
  static ProxyExec##UniqueID g_register_plugin_##UniqueID;

#define PLUGINS_REGISTER_CLASS_INTERNAL_HOP1(Derived, Base, UniqueID) PLUGINS_REGISTER_CLASS_INTERNAL(Derived, Base, UniqueID)

/**
* @brief This is the PLUGINS_REGISTER_CLASS macro which must be declared within the source (.cpp) file for each class that is to be exported as plugin.
* The macro utilizes a trick where a new struct is generated along with a declaration of static global variable of same type after it. The struct's constructor invokes a registration function with the plugin system. When the plugin system loads a library with registered classes in it, the initialization of static variables forces the invocation of the struct constructors, and all plugins are automatically registerd.
*/
#define PLUGINS_REGISTER_CLASS(Derived, Base)  PLUGINS_REGISTER_CLASS_INTERNAL_HOP1(Derived, Base, __COUNTER__)


#endif

