#ifndef PLUGINS_CLASS_LOADER_H_DEFINED
#define PLUGINS_CLASS_LOADER_H_DEFINED

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <string>
#include <iostream>
#include "plugins_core.h"

namespace plugins
{

/**
 * @class ClassLoader
 * @brief This class allows loading and unloading of dynamically linked libraries which contain class definitions from which objects can be created/destroyed during runtime (i.e. plugins). Libraries loaded by a ClassLoader are only accessible within scope of that ClassLoader object.
 */
class ClassLoader
{

  private:
    template <class Base>    
    void onPluginDeletion(Base* obj)
    {
      std::cout << "Calling onPluginDeletion() for obj ptr = " << obj << std::endl;
      if(obj)
      {
        boost::mutex::scoped_lock lock(plugin_ref_count_mutex_);

        delete(obj);
        plugin_ref_count_ = plugin_ref_count_ - 1;
        assert(plugin_ref_count_ >= 0);
        if(plugin_ref_count_ == 0 && isOnDemandLoadUnloadEnabled())
          unloadLibrary();
      }
    }

  public:
    /**
     * @brief  Constructor for ClassLoader
     */
    ClassLoader(const std::string& library_path, bool enable_ondemand_loadunload = false);

    /**
     * @brief  Destructor for ClassLoader. All libraries opened by this ClassLoader are unloaded automatically.
     */
    virtual ~ClassLoader();

    /**
     * @brief  Indicates which classes (i.e. plugins) that can be loaded by this object
     * @return vector of strings indicating names of instantiable classes derived from <Base>
     */
    template <class Base>
    std::vector<std::string> getAvailableClasses()
    {
      return(plugins::plugins_private::getAvailableClasses<Base>(this));
    }

    /**
     * @brief Gets the full-qualified path and name of the library associated with this class loader
     */
    std::string getLibraryPath(){return(library_path_);}

    /**
     * @brief  Generates an instance of loadable classes (i.e. plugins)
     * @param  derived_class_name The name of the class we want to create (@see getAvailableClasses())
     * @return A boost::shared_ptr<Base> to newly created plugin object
     */
    template <class T>    
    boost::shared_ptr<T> createInstance(const std::string& derived_class_name)
    {
      if(!isLibraryLoaded())
        loadLibrary();

      T* obj = plugins::plugins_private::createInstance<T>(derived_class_name, this);

      boost::mutex::scoped_lock lock(plugin_ref_count_mutex_);
      if(obj)
        plugin_ref_count_ = plugin_ref_count_ + 1;

      boost::shared_ptr<T> smartObj(obj, boost::bind(&(ClassLoader::onPluginDeletion<T>), this, _1));
      return(smartObj);
    }

    /**
     * @brief  Indicates if a library is loaded within the scope of this ClassLoader. Note that the library may already be loaded internally through another ClassLoader, but until loadLibrary() method is called, the ClassLoader cannot create objects from said library. If we want to see if the library has been opened by somebody else, @see isLibraryLoadedByExternalObject()
     * @param  library_path The path to the library to load
     * @return true if library is loaded within this ClassLoader object's scope, otherwise false
     */
    bool isLibraryLoaded();

    /**
     * @brief  Indicates if a library is loaded by some entity in the plugin system (another ClassLoader), but not necessarily loaded by this ClassLoader
     * @return true if library is loaded within the scope of the plugin system, otherwise false
     */
    bool isLibraryLoadedByAnyClassloader();

    /**
     * @brief 
     */
    bool isOnDemandLoadUnloadEnabled(){return(enable_ondemand_loadunload_);}

     /**
     * @brief  Attempts to load a library on behalf of the ClassLoader. If the library is already opened, this method has no effect. If the library has been already opened by some other entity (i.e. another ClassLoader or global interface), this object is given permissions to access any plugin classes loaded by that other entity.
     * @param  library_path The path to the library to load
     */
    void loadLibrary();

     /**
     * @brief  Attempts to unload a library loaded within scope of the ClassLoader. If the library is not opened, this method has no effect. If the library is opened by other entities (i.e. another ClassLoader or global interface), the library will NOT be unloaded internally -- however this ClassLoader will no longer be able to instantiate plugins bound to that library.
     */
    void unloadLibrary();


  private:
    bool        enable_ondemand_loadunload_;
    std::string library_path_;  
    int         plugin_ref_count_;
    boost::mutex plugin_ref_count_mutex_;
};

};


#endif
