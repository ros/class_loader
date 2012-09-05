#include "class_loader.h"
#include <iostream>

namespace plugins
{

ClassLoader::ClassLoader(const std::string& library_path, bool enable_ondemand_loadunload) :
enable_ondemand_loadunload_(enable_ondemand_loadunload),
library_path_(library_path),
plugin_ref_count_(0)
{
  if(!isOnDemandLoadUnloadEnabled())
    loadLibrary();
}

ClassLoader::~ClassLoader()
{
  std::cout << "Destroying class loader, unloading associated library..." << std::endl;
  unloadLibrary();
}

bool ClassLoader::isLibraryLoaded()
{
  return(plugins::plugins_private::isLibraryLoaded(getLibraryPath(), this));
}

bool ClassLoader::isLibraryLoadedByExternalObject()
{
  return(plugins::plugins_private::isLibraryLoadedByAnybody(getLibraryPath()));
}

void ClassLoader::loadLibrary()
{
  plugins::plugins_private::loadLibrary(getLibraryPath(), this);
}

void ClassLoader::unloadLibrary()
{
  if(plugin_ref_count_ > 0)
    std::cout << "ClassLoader: MAJOR WARNING!!! Attempting to unload library while objects created by this loader exist in the heap! You should delete your objects before attempting to unload the library or destroying the ClassLoader. The library will NOT be unloaded." << std::endl;
  else
    plugins::plugins_private::unloadLibrary(getLibraryPath(), this);
}

}
