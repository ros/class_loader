#include "multi_library_class_loader.h"

namespace plugins
{

MultiLibraryClassLoader::MultiLibraryClassLoader(bool enable_ondemand_loadunload) :
enable_ondemand_loadunload_(enable_ondemand_loadunload)
{
}

MultiLibraryClassLoader::~MultiLibraryClassLoader()
{
  shutdownAllClassLoaders();
}

std::vector<std::string> MultiLibraryClassLoader::getRegisteredLibraries()
{
  std::vector<std::string> libraries;
  for(LibraryToClassLoaderMap::iterator itr = active_class_loaders_.begin(); itr != active_class_loaders_.end(); itr++)
    libraries.push_back(itr->first);
  return(libraries);
}

ClassLoader* MultiLibraryClassLoader::getClassLoaderForLibrary(const std::string& library_path)
{
  return(active_class_loaders_[library_path]);
}

ClassLoaderVector MultiLibraryClassLoader::getAllAvailableClassLoaders()
{
  ClassLoaderVector loaders;
  for(LibraryToClassLoaderMap::iterator itr = active_class_loaders_.begin(); itr != active_class_loaders_.end(); itr++)
    loaders.push_back(itr->second);
  return(loaders);
}

bool MultiLibraryClassLoader::isLibraryAvailable(const std::string& library_name)
{
  std::vector<std::string> available_libraries = getRegisteredLibraries();
  return(available_libraries.end() != std::find(available_libraries.begin(), available_libraries.end(), library_name));
}

void MultiLibraryClassLoader::loadLibrary(const std::string& library_path)
{
  if(!isLibraryAvailable(library_path))
    active_class_loaders_[library_path] = new plugins::ClassLoader(library_path, isOnDemandLoadUnloadEnabled());
}

void MultiLibraryClassLoader::shutdownAllClassLoaders()
{
  std::vector<std::string> available_libraries = getRegisteredLibraries();
  for(unsigned int c = 0; c < available_libraries.size(); c++)
    unloadLibrary(available_libraries.at(c));
}

void MultiLibraryClassLoader::unloadLibrary(const std::string& library_path)
{
  if(isLibraryAvailable(library_path))
  {
    ClassLoader* loader = getClassLoaderForLibrary(library_path);
    active_class_loaders_[library_path] = NULL;
    delete(loader);
  }
}

}
