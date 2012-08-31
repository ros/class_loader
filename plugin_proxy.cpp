#include "plugin_proxy.h"
#include <cassert>

namespace plugins
{
namespace plugins_private
{

//Global data 
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

boost::mutex& getCriticalSectionMutex()
/*****************************************************************************/
{
  static boost::mutex m;
  return m;
}

BaseToFactoryMapMap& getGlobalPluginBaseToFactoryMapMap()
/*****************************************************************************/
{
  static BaseToFactoryMapMap instance;
  return instance;
}

LibraryVector& getLoadedLibraryVector()
/*****************************************************************************/
{
  static LibraryVector instance;
  return instance;
}

std::string& getCurrentlyLoadingLibraryNameReference()
/*****************************************************************************/
{
  static std::string library_name;
  return library_name;
}

std::string getCurrentlyLoadingLibraryName()
/*****************************************************************************/
{
  return(getCurrentlyLoadingLibraryNameReference());
}

void setCurrentlyLoadingLibraryName(const std::string& library_name)
/*****************************************************************************/
{
  std::string& library_name_ref = getCurrentlyLoadingLibraryNameReference();
  library_name_ref = library_name;
}

ClassLoader*& getCurrentlyActiveClassLoaderReference()
/*****************************************************************************/
{
  static ClassLoader* loader = NULL;
  return(loader);
}

ClassLoader* getCurrentlyActiveClassLoader()
/*****************************************************************************/
{
  return(getCurrentlyActiveClassLoaderReference());
}

void setCurrentlyActiveClassLoader(ClassLoader* loader)
/*****************************************************************************/
{
  ClassLoader*& loader_ref = getCurrentlyActiveClassLoaderReference();
  loader_ref = loader;
}

//MetaObject search/insert/removal/query
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

typedef std::vector<AbstractMetaObjectBase*> MetaObjectVector;

MetaObjectVector allMetaObjects(const FactoryMap& factories)
/*****************************************************************************/
{
  MetaObjectVector all_meta_objs;
  for(FactoryMap::const_iterator factoryItr = factories.begin(); factoryItr != factories.end(); factoryItr++)
      all_meta_objs.push_back(factoryItr->second);
  return(all_meta_objs);
}

MetaObjectVector allMetaObjects()
/*****************************************************************************/
{
  MetaObjectVector all_meta_objs;
  BaseToFactoryMapMap& factory_map_map = getGlobalPluginBaseToFactoryMapMap();
  BaseToFactoryMapMap::iterator itr;
  for(itr = factory_map_map.begin(); itr != factory_map_map.end(); itr++)
  {
    MetaObjectVector objs = allMetaObjects(itr->second);
    all_meta_objs.insert(all_meta_objs.end(), objs.begin(), objs.end());
  }
  return(all_meta_objs);
}

MetaObjectVector filterAllMetaObjectsOwnedBy(const MetaObjectVector& to_filter, const ClassLoader* owner)
/*****************************************************************************/
{
  MetaObjectVector filtered_objs;
  for(unsigned int c = 0; c < to_filter.size(); c++)
    if(to_filter.at(c)->isOwnedBy(owner))
      filtered_objs.push_back(to_filter.at(c));
  return(filtered_objs);
}

MetaObjectVector filterAllMetaObjectsAssociatedWithLibrary(const MetaObjectVector& to_filter, const std::string& library_path)
/*****************************************************************************/
{
  MetaObjectVector filtered_objs;
  for(unsigned int c = 0; c < to_filter.size(); c++)
    if(to_filter.at(c)->getAssociatedLibraryPath()==library_path)
      filtered_objs.push_back(to_filter.at(c));
  return(filtered_objs);
}

MetaObjectVector allMetaObjectsForClassLoader(const ClassLoader* owner)
/*****************************************************************************/
{
  return(filterAllMetaObjectsOwnedBy(allMetaObjects(), owner));
}

MetaObjectVector allMetaObjectsForLibrary(const std::string& library_path)
/*****************************************************************************/
{
  return(filterAllMetaObjectsAssociatedWithLibrary(allMetaObjects(), library_path));
}

MetaObjectVector allMetaObjectsForLibraryOwnedBy(const std::string& library_path, const ClassLoader* owner)
/*****************************************************************************/
{
  return(filterAllMetaObjectsOwnedBy(allMetaObjectsForLibrary(library_path), owner));
}

void destroyMetaObjectsForLibrary(const std::string& library_path, FactoryMap& factories, const ClassLoader* loader)
/*****************************************************************************/
{
  FactoryMap::iterator factory_itr = factories.begin();    
  while(factory_itr != factories.end())
  {
    AbstractMetaObjectBase* meta_obj = factory_itr->second;
    if(meta_obj->getAssociatedLibraryPath() == library_path && meta_obj->isOwnedBy(loader))
    {
      meta_obj->removeOwningClassLoader(loader);
      if(!meta_obj->isOwnedByAnybody())
      {
        delete(meta_obj);
        FactoryMap::iterator factory_itr_copy= factory_itr;
        factory_itr++;
        factories.erase(factory_itr_copy); //Note: map::erase does not return iterator like vector::erase does. Hence the ugliness of this code and a need for copy. Should be fixed in next C++ revision
      }
      else
        factory_itr++;      
    }
    else
      factory_itr++;
  }
}

void destroyMetaObjectsForLibrary(const std::string& library_path, const ClassLoader* loader)
/*****************************************************************************/
{
  //Note: Called within scope of unloadLibrary so no need for critical section
  std::cout << "Purging MetaObjects associated with library " << library_path << " and class loader " << loader << "." << std::endl;

  //We have to walk through all FactoryMaps to be sure
  BaseToFactoryMapMap& factory_map_map = getGlobalPluginBaseToFactoryMapMap();
  BaseToFactoryMapMap::iterator itr;
  for(itr = factory_map_map.begin(); itr != factory_map_map.end(); itr++)
    destroyMetaObjectsForLibrary(library_path, itr->second, loader);

  std::cout << "Metaobjects removed." << std::endl;
}

bool areThereAnyExistingMetaObjectsForLibrary(const std::string& library_path)
/*****************************************************************************/
{
  return(allMetaObjectsForLibrary(library_path).size() > 0);
}

//Loaded Library Vector manipulation
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
LibraryVector::iterator findLoadedLibrary(const std::string& library_path)
/*****************************************************************************/
{
  LibraryVector& open_libraries =  getLoadedLibraryVector();
  LibraryVector::iterator itr;
  for(itr = open_libraries.begin(); itr != open_libraries.end(); itr++ )
  {
    if(itr->first == library_path)
      break;
  }
  return(itr);
}

bool isLibraryLoadedByAnybody(const std::string& library_path)
/*****************************************************************************/
{
  boost::mutex::scoped_lock(getCriticalSectionMutex());

  bool is_it_loaded = false;
  LibraryVector& open_libraries =  getLoadedLibraryVector();
  LibraryVector::iterator itr = findLoadedLibrary(library_path);

  if(itr != open_libraries.end())
  {
    assert(itr->second->isLoaded() == true); //Ensure Poco actually thinks the library is loaded
    return(true);
  }
  else
    return(false);
};

bool isLibraryLoaded(const std::string& library_path, ClassLoader* loader)
/*****************************************************************************/
{
  return(isLibraryLoadedByAnybody(library_path) && (allMetaObjectsForLibraryOwnedBy(library_path,loader).size() > 0));
}

std::vector<std::string> getAllLibrariesUsedByClassLoader(const ClassLoader* loader)
/*****************************************************************************/
{
  MetaObjectVector all_loader_meta_objs = allMetaObjectsForClassLoader(loader);
  std::vector<std::string> all_libs;
  for(unsigned int c = 0; c < all_loader_meta_objs.size(); c++)
  {
    std::string lib_path = all_loader_meta_objs.at(c)->getAssociatedLibraryPath();
    if(std::find(all_libs.begin(), all_libs.end(), lib_path) == all_libs.end())
      all_libs.push_back(lib_path);
  }
  return(all_libs);
}


//Implementation of Remaining Core plugin_private Functions
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

void addClassLoaderOwnerForAllExistingMetaObjectsForLibrary(const std::string& library_path, ClassLoader* loader)
/*****************************************************************************/
{
  MetaObjectVector all_meta_objs = allMetaObjectsForLibrary(library_path);
  for(unsigned int c = 0; c < all_meta_objs.size(); c++)
  {
    std::cout << "Tagging existing MetaObject " << all_meta_objs.at(c) << " with loader " << loader << std::endl;
    all_meta_objs.at(c)->addOwningClassLoader(loader);
  }
}

void loadLibrary(const std::string& library_path, ClassLoader* loader)
/*****************************************************************************/
{
  std::cout << "Attempting to load library " << library_path << "..." << std::endl;

  if(isLibraryLoadedByAnybody(library_path))
  {
    std::cout << "Library already in memory, but binding existing MetaObjects to loader if necesesary." << std::endl;
    addClassLoaderOwnerForAllExistingMetaObjectsForLibrary(library_path, loader);
    return;
  }
  
  boost::mutex::scoped_lock(getCriticalSectionMutex()); //Note: We start critical section here as isLibraryLoaded() call above locks CS as well
  
  try
  {
    setCurrentlyActiveClassLoader(loader);
    setCurrentlyLoadingLibraryName(library_path);

    LibraryVector& open_libraries =  getLoadedLibraryVector();
    open_libraries.push_back(LibraryPair(library_path, new Poco::SharedLibrary(library_path))); //Note: Poco::SharedLibrary automatically calls load() when library passed to constructor
    
    setCurrentlyLoadingLibraryName("");
    setCurrentlyActiveClassLoader(loader);
  }
  catch(const Poco::LibraryLoadException& e)
  {
    throw(plugins::LibraryLoadException("Could not load library (Poco exception = " + std::string(e.name()) + ")"));
  }
  catch(const Poco::LibraryAlreadyLoadedException& e)
  {
    throw(plugins::LibraryLoadException("Library already loaded (Poco exception = " + std::string(e.name()) + ")"));
  }
}

void unloadLibrary(const std::string& library_path, ClassLoader* loader)
/*****************************************************************************/
{ 
  boost::mutex::scoped_lock(getCriticalSectionMutex());

  LibraryVector& open_libraries =  getLoadedLibraryVector();
  LibraryVector::iterator itr = findLoadedLibrary(library_path);
  if(itr != open_libraries.end())
  {
    Poco::SharedLibrary* library = itr->second;
    std::string library_path = itr->first;
    try
    {
      destroyMetaObjectsForLibrary(library_path, loader);
   
      //Remove from loaded library list as well if no more factories associated with said library
      if(!areThereAnyExistingMetaObjectsForLibrary(library_path))
      {
        std::cout << "There are no more MetaObjects left for " << library_path << " so unloading library and removing from loaded library vector." << std::endl;
        library->unload();
        delete(library);
        itr = open_libraries.erase(itr);
      }
    }
    catch(const Poco::RuntimeException& e)
    {
      delete(library);
      throw(plugins::LibraryUnloadException("Could not unload library (Poco exception = " + std::string(e.name()) + ")"));
    }
  }
}

} //End namespace plugins_private
} //End namespace plugins
