#ifndef PLUGINS_CLASS_LOADER_H_DEFINED
#define PLUGINS_CLASS_LOADER_H_DEFINED

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include "plugin_proxy.h"

/* Note: This is the ClassLoader interface to the plugin system. Users have two options when utilizing plugins:
1) Utilize the global functions in "plugins.h"
2) Utilize the ClassLoader class for finer granularity control (defined below)

Both interfaces can be used side-by-side.
*/

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
    ClassLoader();

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
     * @brief  Generates an instance of loadable classes (i.e. plugins)
     * @param  derived_class_name The name of the class we want to create (@see getAvailableClasses())
     * @return A boost::shared_ptr<Base> to newly created plugin object
     */
    template <class Base>    
    boost::shared_ptr<Base> createInstance(const std::string& derived_class_name)
    {
      return(plugins::plugins_private::createInstance<Base>(derived_class_name, this));
    }

    /**
     * @brief  Indicates if a library is loaded within the scope of this ClassLoader. Note that the library may already be loaded internally either via the global interface or another ClassLoader, but until loadLibrary() method is called, the ClassLoader cannot create objects from said library. If we want to see if the library has been opened by somebody else, @see isLibraryLoadedByExternalObject()
     * @param  library_path The path to the library to load
     * @return true if library is loaded within this ClassLoader object's scope, otherwise false
     */
    bool isLibraryLoaded(const std::string& library_path);

    /**
     * @brief  Indicates if a library is loaded by some entity in the plugin system (either global function interface or another ClassLoader), but not necessarily loaded by this ClassLoader
     * @param  library_path The path to the library to load
     * @return true if library is loaded within the scope of the plugin system, otherwise false
     */
    bool isLibraryLoadedByExternalObject(const std::string& library_path);

     /**
     * @brief  Attempts to load a library on behalf of the ClassLoader. If the library is already opened, this method has no effect. If the library has been already opened by some other entity (i.e. another ClassLoader or global interface), this object is given permissions to access any plugin classes loaded by that other entity.
     * @param  library_path The path to the library to load
     */
    void loadLibrary(const std::string& library_path);

     /**
     * @brief  Attempts to unload a library loaded within scope of the ClassLoader. If the library is not opened, this method has no effect. If the library is opened by other entities (i.e. another ClassLoader or global interface), the library will NOT be unloaded internally -- however this ClassLoader will no longer be able to instantiate plugins bound to that library.
     * @param  library_path The path to the library to load
     */
    void unloadLibrary(const std::string& library_path);

 private:
     /**
     * @brief Unloads all libraries that are in use by this ClassLoader. This is done during destruction time.
     */
    void unloadAllLibrariesInUseByMe();
};

};


#endif
