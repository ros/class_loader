#ifndef PLUGINS_CLASS_LOADER_H_DEFINED
#define PLUGINS_CLASS_LOADER_H_DEFINED

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <string>
#include <console_bridge/console.h>
#include "plugins_core.h"

namespace plugins
{

/**
 * @class ClassLoader
 * @brief This class allows loading and unloading of dynamically linked libraries which contain class definitions from which objects can be created/destroyed during runtime (i.e. plugins). Libraries loaded by a ClassLoader are only accessible within scope of that ClassLoader object.
 */
class ClassLoader
{
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
     * @brief  Generates an instance of loadable classes (i.e. plugins). It is not necessary for the user to call loadLibrary() as it will be invoked automatically if the library is not yet loaded (which typically happens when in "On Demand Load/Unload" mode).
     * @param  derived_class_name The name of the class we want to create (@see getAvailableClasses())
     * @return A boost::shared_ptr<Base> to newly created plugin object
     */
    template <class T>    
    boost::shared_ptr<T> createInstance(const std::string& derived_class_name)
    {
      T* obj = createUnmanagedInstance<T>(derived_class_name);

      boost::mutex::scoped_lock lock(plugin_ref_count_mutex_);
      if(obj)
        plugin_ref_count_ = plugin_ref_count_ + 1;

      boost::shared_ptr<T> smart_obj(obj, boost::bind(&(ClassLoader::onPluginDeletion<T>), this, _1));
      return(smart_obj);
    }

    /**
     * @brief  Generates an instance of loadable classes (i.e. plugins). It is not necessary for the user to call loadLibrary() as it will be invoked automatically if the library is not yet loaded (which typically happens when in "On Demand Load/Unload" mode).
     * @param derived_class_name The name of the class we want to create (@see getAvailableClasses())
     * @return An unmanaged (i.e. not a shared_ptr) Base* to newly created plugin object.
     */
    template <class Base>
    Base* createUnmanagedInstance(const std::string& derived_class_name)
    {
      if(!isLibraryLoaded())
        loadLibrary();

      Base* obj = plugins::plugins_private::createInstance<Base>(derived_class_name, this);

      return(obj);
    }

    /**
     *
     */
    template <class Base>
    bool isClassAvailable(const std::string& class_name)
    {
      std::vector<std::string> available_classes = getAvailableClasses<Base>();
      return(std::find(available_classes.begin(), available_classes.end(), class_name) != available_classes.end());
    }

    /**
     * @brief  Indicates if a library is loaded within the scope of this ClassLoader. Note that the library may already be loaded internally through another ClassLoader, but until loadLibrary() method is called, the ClassLoader cannot create objects from said library. If we want to see if the library has been opened by somebody else, @see isLibraryLoadedByAnyClassloader()
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
     * @brief  Attempts to load a library on behalf of the ClassLoader. If the library is already opened, this method has no effect. If the library has been already opened by some other entity (i.e. another ClassLoader or global interface), this object is given permissions to access any plugin classes loaded by that other entity. This is
     * @param  library_path The path to the library to load
     */
    void loadLibrary();

     /**
     * @brief  Attempts to unload a library loaded within scope of the ClassLoader. If the library is not opened, this method has no effect. If the library is opened by other entities (i.e. another ClassLoader or global interface), the library will NOT be unloaded internally -- however this ClassLoader will no longer be able to instantiate plugins bound to that library. If there are plugin objects that exist in memory created by this classloader, a warning message will appear and the library will not be unloaded. If loadLibrary() was called multiple times (e.g. in the case of multiple threads or purposefully in a single thread), the user is responsible for calling unloadLibrary() the same number of times. The library will not be unloaded within the context of this classloader until the number of unload calls matches the number of loads.
     */
    void unloadLibrary();

  private:
    /**
     * @brief Callback method when a plugin created by this class loader is destroyed
     * @param obj - A pointer to the deleted object
     */
    template <class Base>    
    void onPluginDeletion(Base* obj)
    {
      logDebug("plugins::ClassLoader: Calling onPluginDeletion() for obj ptr = %p.\n", obj);
      if(obj)
      {
        boost::mutex::scoped_lock lock(plugin_ref_count_mutex_);
        delete(obj);
        plugin_ref_count_ = plugin_ref_count_ - 1;
        assert(plugin_ref_count_ >= 0);
        if(plugin_ref_count_ == 0 && isOnDemandLoadUnloadEnabled())
          unloadLibraryInternal(false);
      }
    }

  /**
   * @brief As the library may be unloaded in "on-demand load/unload" mode, unload maybe called from createInstance(). The problem is that createInstance() locks the plugin_ref_count as does unloadLibrary(). This method is the implementation of unloadLibrary but with a parameter to decide if plugin_ref_mutex_ should be locked
   * @param lock_plugin_ref_count - Set to true if plugin_ref_count_mutex_ should be locked, else false
   */
  void unloadLibraryInternal(bool lock_plugin_ref_count);

  private:
    bool enable_ondemand_loadunload_;
    std::string library_path_;
    int load_ref_count_;  
    boost::mutex load_ref_count_mutex_;
    int plugin_ref_count_;
    boost::mutex plugin_ref_count_mutex_;
};

};


#endif
