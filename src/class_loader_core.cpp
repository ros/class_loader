/*
 * Copyright (c) 2012, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "class_loader/class_loader_core.h"
#include <cassert>

namespace class_loader
{
namespace class_loader_private
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

bool& hasANonPurePluginLibraryBeenOpenedReference()
/*****************************************************************************/
{
  static bool hasANonPurePluginLibraryBeenOpenedReference = false;
}

bool hasANonPurePluginLibraryBeenOpened()
/*****************************************************************************/
{
  return(hasANonPurePluginLibraryBeenOpenedReference());
}

void hasANonPurePluginLibraryBeenOpened(bool hasIt)
/*****************************************************************************/
{
  hasANonPurePluginLibraryBeenOpenedReference() = hasIt;
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
  logDebug("class_loader::class_loader_core: Purging MetaObjects associated with library %s and class loader %p.\n", library_path.c_str(), loader);

  //We have to walk through all FactoryMaps to be sure
  BaseToFactoryMapMap& factory_map_map = getGlobalPluginBaseToFactoryMapMap();
  BaseToFactoryMapMap::iterator itr;
  for(itr = factory_map_map.begin(); itr != factory_map_map.end(); itr++)
    destroyMetaObjectsForLibrary(library_path, itr->second, loader);

  logDebug("class_loader::class_loader_core: Metaobjects removed.\n");
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
  boost::mutex::scoped_lock lock(getCriticalSectionMutex());

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
    logDebug("class_loader::class_loader_core: Tagging existing MetaObject %p with class loader %p.\n", all_meta_objs.at(c), loader);
    all_meta_objs.at(c)->addOwningClassLoader(loader);
  }
}

void loadLibrary(const std::string& library_path, ClassLoader* loader)
/*****************************************************************************/
{
  logDebug("class_loader::class_loader_core: Attempting to load library %s...\n", library_path.c_str());

  if(isLibraryLoadedByAnybody(library_path))
  {
    logDebug("class_loader::class_loader_core: Library already in memory, but binding existing MetaObjects to loader if necesesary.\n");
    addClassLoaderOwnerForAllExistingMetaObjectsForLibrary(library_path, loader);
    return;
  }
  
  boost::mutex::scoped_lock lock(getCriticalSectionMutex()); //Note: We start critical section here as isLibraryLoaded() call above locks CS as well
  
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
    throw(class_loader::LibraryLoadException("Could not load library (Poco exception = " + std::string(e.name()) + ")"));
  }
  catch(const Poco::LibraryAlreadyLoadedException& e)
  {
    throw(class_loader::LibraryLoadException("Library already loaded (Poco exception = " + std::string(e.name()) + ")"));
  }
  catch(const Poco::NotFoundException& e)
  {
    throw(class_loader::LibraryLoadException("Library not found (Poco exception = " + std::string(e.name()) + ")"));
  }
}

void unloadLibrary(const std::string& library_path, ClassLoader* loader)
/*****************************************************************************/
{ 
  boost::mutex::scoped_lock lock(getCriticalSectionMutex());

  if(hasANonPurePluginLibraryBeenOpened())
  {
    logError("class_loader::class_loader_core: Cannot unload %s or ANY other library as a non-pure plugin library was opened. As class_loader has no idea which libraries class factories were exported from, it can safely close any library without potentially unlinking symbols that are still actively being used. You must refactor your plugin libraries to be made exclusively of plugins in order for this error to stop happening.", library_path.c_str());
  }
  else
  { 
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
          logDebug("class_loader::class_loader_core: There are no more MetaObjects left for %s so unloading library and removing from loaded library vector.\n", library_path.c_str());
          library->unload();
          delete(library);
          itr = open_libraries.erase(itr);
        }
      }
      catch(const Poco::RuntimeException& e)
      {
        delete(library);
        throw(class_loader::LibraryUnloadException("Could not unload library (Poco exception = " + std::string(e.name()) + ")"));
      }
    }
  }
}

} //End namespace class_loader_private
} //End namespace class_loader
