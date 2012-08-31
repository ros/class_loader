#ifndef PLUGIN_REGISTER_MACRO_H_DEFINED
#define PLUGIN_REGISTER_MACRO_H_DEFINED

#include "plugin_proxy.h"

/**
* @brief This is the REGISTER_CLASS macro which must be declared within the source (.cpp) file for each class that is to be exported as plugin. The macro utilizes a trick where a new struct is generated along with a declaration of static global variable of same type after it. The struct's constructor invokes a registration function with the plugin system. When the plugin system loads a library with registered classes in it, the initialization of static variables forces the invocation of the struct constructors, and all plugins are automatically registerd.
*/
#define REGISTER_CLASS(Derived, Base) \
  struct ProxyExec_##Derived \
  { \
     typedef  Derived _derived; \
     typedef  Base    _base; \
     ProxyExec_##Derived() \
     { \
       plugins::plugins_private::registerPlugin<_derived, _base>(#Derived); \
     } \
  }; \
  \
  static ProxyExec_##Derived g_register_plugin_##Derived;

#endif

