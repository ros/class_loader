#ifndef PLUGINS_MULTI_LIBRARY_CLASS_LOADER_H_DEFINED
#define PLUGINS_MULTI_LIBRARY_CLASS_LOADER_H_DEFINED

#include "class_loader.h"
#include <boost/thread.hpp>

namespace plugins
{

typedef std::string LibraryPath;
typedef std::map<LibraryPath, plugins::ClassLoader*> LibraryToClassLoaderMap;
typedef std::vector<ClassLoader*> ClassLoaderVector;

template <class Base>
class MultiLibraryClassLoader
{
  public:
    MultiLibraryClassLoader(bool enable_ondemand_loadunload);
    virtual ~MultiLibraryClassLoader();

    bool isClassAvailable(const std::string& class_name);
    bool isLibraryAvailable(const std::string& library_name);
    std::vector<std::string> getAvailableClasses();
    std::vector<std::string> getRegisteredLibraries();

    boost::shared_ptr<Base> createInstance(const std::string& class_name, const std::string& library_path)
    {
      return(getClassLoaderForLibrary(library_path)->createInstance<Base>(class_name));
    }

    void loadLibrary(const std::string& library_path);
    void unloadLibrary(const std::string& library_path);


  private:
    bool isOnDemandLoadUnloadEnabled(){return(enable_ondemand_loadunload_);}
    ClassLoader* getClassLoaderForLibrary(const std::string& library_path);
    ClassLoaderVector getAllAvailableClassLoaders();
    void shutdownAllClassLoaders();

  private:
    bool enable_ondemand_loadunload_;
    LibraryToClassLoaderMap active_class_loaders_;
    boost::mutex loader_mutex_;
};


}
#endif
