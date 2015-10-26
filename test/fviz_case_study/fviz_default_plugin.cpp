#include <class_loader/class_loader.h>

#include "fviz.h"  // For foo()
#include "fviz_plugin_base.h"

class Bar : public FvizPluginBase
{
public:
  virtual ~Bar() = default;
  void speak()
  {
    foo("from plugin Bar");
  }
};

class Baz : public FvizPluginBase
{
public:
  virtual ~Baz() = default;
  void speak()
  {
    foo("from plugin Baz");
  }
};

CLASS_LOADER_REGISTER_CLASS(Bar, FvizPluginBase);
CLASS_LOADER_REGISTER_CLASS(Baz, FvizPluginBase);
