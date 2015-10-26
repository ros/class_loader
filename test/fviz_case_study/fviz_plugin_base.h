#ifndef FVIZ_PLUGIN_BASE_H
#define FVIZ_PLUGIN_BASE_H

class FvizPluginBase
{
public:
  virtual ~FvizPluginBase() {}
  virtual void speak() = 0;
};

#endif  // FVIZ_PLUGIN_BASE_H
