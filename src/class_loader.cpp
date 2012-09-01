#include "class_loader.h"

namespace plugins
{

ClassLoader::ClassLoader()
{
}

ClassLoader::~ClassLoader()
{
  std::cout << "Destroying class loader, unloading all associated libraries..." << std::endl;
  unloadAllLibrariesInUseByMe();
}

bool ClassLoader::isLibraryLoaded(const std::string& library_path)
{
  return(plugins::plugins_private::isLibraryLoaded(library_path, this));
}

bool ClassLoader::isLibraryLoadedByExternalObject(const std::string& library_path)
{
  return(plugins::plugins_private::isLibraryLoadedByAnybody(library_path));
}

void ClassLoader::loadLibrary(const std::string& library_path)
{
  plugins::plugins_private::loadLibrary(library_path, this);
}

void ClassLoader::unloadAllLibrariesInUseByMe()
{
  std::vector<std::string> all_libs_in_use = plugins::plugins_private::getAllLibrariesUsedByClassLoader(this);
  for(unsigned int c = 0; c < all_libs_in_use.size(); c++)
    unloadLibrary(all_libs_in_use.at(c));
}

void ClassLoader::unloadLibrary(const std::string& library_path)
{
  plugins::plugins_private::unloadLibrary(library_path, this);
}

};
