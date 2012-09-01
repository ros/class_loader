#ifndef PLUGINS_H_DEFINED
#define PLUGINS_H_DEFINED

#include "plugin_proxy.h"
#include <boost/shared_ptr.hpp>

/* Note: This is the global function interface to the plugin system. Users have two options when utilizing plugins:
1) Use the functions listed below
2) Utilize the ClassLoader class for finer granularity control

Both interfaces can be used side-by-side.
*/

namespace plugins
{

/**
 * @brief  Creates a plugin with base class <Base> given derived class name and returns a handle to the object in the form of a boost::shared_ptr
 * @param  derived_class_name The name of the class derived from Base we want to create (@see getAvailableClasses())
 * @return A boost::shared_ptr<Base> to newly created plugin object
 */
template <typename Base> boost::shared_ptr<Base> createInstance(const std::string& derived_class_name)
{
  return boost::shared_ptr<Base>(plugins_private::createInstance<Base>(derived_class_name, NULL));
}

/**
 * @brief  Gets a list of subclasses of Base that were registered during library load time and can be created via createInstance()
 * @return Returns a vector<string> representing available derived classes
 */
template <typename Base> std::vector<std::string> getAvailableClasses()
{
  return plugins_private::getAvailableClasses<Base>(NULL);
}

/**
 * @brief  Indicates if a library is loaded within the scope of the global function interface. Note that the library may already be loaded internally by another entity (i.e. a ClassLoader), but that cannot be seen within the scope of the global interface. To test this, @see isLibraryLoadedViaClassLoader()
 * @param  library_path The path to the library to load
 * @return True if the global function interface has the library loaded, otherwise false
 */
bool isLibraryLoaded(const std::string& library_path);

/**
 * @brief  Indicates if library is already loaded within the scope of an existing ClassLoader object external to the global interface provided by this header.
 * @param  library_path The path to the library to load
 * @return True if some ClassLoader has already loaded this library, otherwise false.
 */
bool isLibraryLoadedViaClassLoader(const std::string& library_path);

/**
 * @brief  Loads a library into memory if it has not already been done so. Attempting to load an already loaded library has no effect.
 * @param  library_path The path to the library to load 
 */
void loadLibrary(const std::string& library_path);


/**
 * @brief  Unloads a library if it loaded within the scope of the global interface. If it is not loaded, the function has no effect. If the library is in use by an external entity (i.e. a ClassLoader), the library will not be unloaded until all users are done.
 * @param  library_path The path to the library to unload
 */
void unloadLibrary(const std::string& library_path);

/**
 * @brief Unloads all libraries within the scope of the global interface. You should call this when done with the plugin system to guarantee unloading of libraries and cleanup of memory.
 */
void unloadAllLibraries();

}


#endif
