#include "plugins.h"

namespace plugins
{

bool isLibraryLoaded(const std::string& library_path)
{
  return plugins_private::isLibraryLoaded(library_path, NULL);
}

bool isLibraryLoadedViaClassLoader(const std::string& library_path)
{
  return plugins_private::isLibraryLoadedByAnybody(library_path);
}

void loadLibrary(const std::string& library_path)
{
  plugins_private::loadLibrary(library_path, NULL);
}

void unloadLibrary(const std::string& library_path)
{
  plugins_private::unloadLibrary(library_path, NULL);
}

void unloadAllLibraries()
{
  std::vector<std::string> all_libs_in_use = plugins::plugins_private::getAllLibrariesUsedByClassLoader(NULL);
  for(unsigned int c = 0; c < all_libs_in_use.size(); c++)
    unloadLibrary(all_libs_in_use.at(c));
}


}
