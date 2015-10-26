#include <gtest/gtest.h>

#include <class_loader/class_loader.h>

#include "fviz.h"
#include "fviz_plugin_base.h"

#if defined(WIN32)
std::string name = "class_loader_Test_FvizDefaultPlugin";
#else
std::string name = "libclass_loader_Test_FvizDefaultPlugin" + class_loader::systemLibrarySuffix();
#endif

TEST(FvizTest, basic_test)
{
  try {
    class_loader::ClassLoader loader(name);
    loader.createInstance<FvizPluginBase>("Bar")->speak();
    loader.createInstance<FvizPluginBase>("Baz")->speak();
  } catch (const class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }

  SUCCEED();
}
