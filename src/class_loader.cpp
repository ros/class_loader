#include "class_loader.h"
#include <iostream>

namespace plugins
{

ClassLoader::ClassLoader(const std::string& library_path, bool enable_ondemand_loadunload) :
enable_ondemand_loadunload_(enable_ondemand_loadunload),
library_path_(library_path),
load_ref_count_(0),
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

bool ClassLoader::isLibraryLoadedByAnyClassloader()
{
  return(plugins::plugins_private::isLibraryLoadedByAnybody(getLibraryPath()));
}

void ClassLoader::loadLibrary()
{
  boost::mutex::scoped_lock lock(load_ref_count_mutex_);
  load_ref_count_ = load_ref_count_ + 1;
  plugins::plugins_private::loadLibrary(getLibraryPath(), this);
}

void ClassLoader::unloadLibrary()
{
  unloadLibraryInternal(true);
}

void ClassLoader::unloadLibraryInternal(bool lock_plugin_ref_count)
{
  boost::mutex::scoped_lock load_ref_lock(load_ref_count_mutex_);
  if(lock_plugin_ref_count)
    plugin_ref_count_mutex_.lock(); //Can't use scoped lock, remember to unlock...

  if(plugin_ref_count_ > 0)
    std::cout << "ClassLoader: MAJOR WARNING!!! Attempting to unload library while objects created by this loader exist in the heap! You should delete your objects before attempting to unload the library or destroying the ClassLoader. The library will NOT be unloaded." << std::endl;
  else
  {
    load_ref_count_ = load_ref_count_ - 1;
    if(load_ref_count_ == 0)
      plugins::plugins_private::unloadLibrary(getLibraryPath(), this);
    else if(load_ref_count_ < 0)
      load_ref_count_ = 0;
  }

  if(lock_plugin_ref_count)
    plugin_ref_count_mutex_.unlock(); //...and this is where we unlock
 
}

}
