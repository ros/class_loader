#include <cstdio>
#include <string>

#include <class_loader/class_loader.h>

#include "fviz.h"
#include "fviz_plugin_base.h"

std::string name = class_loader::systemLibraryFormat("class_loader_Test_FvizDefaultPlugin");

int main(void)
{
  foo("starting fviz");
  foo("loading plugin: " + name);
  try {
    class_loader::ClassLoader loader(name);
    loader.createInstance<FvizPluginBase>("Bar")->speak();
    loader.createInstance<FvizPluginBase>("Baz")->speak();
  } catch (const class_loader::ClassLoaderException & e) {
    fprintf(stderr, "ClassLoaderException: %s\n", e.what());
    throw;
  }
  foo("shutting down fviz");
  return 0;
}
