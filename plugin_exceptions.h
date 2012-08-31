#include <exception>

namespace plugins
{

/**
 * @class pluginsException
 * @brief A base class for all plugins exceptions that inherits from std::runtime_exception
 */
class PluginException: public std::runtime_error
{
  public:
    PluginException(const std::string error_desc) : std::runtime_error(error_desc) {}
};

/**
 * @class LibraryLoadException
 * @brief An exception class thrown when plugins is unable to load the library associated with a given plugin
 */
class LibraryLoadException: public PluginException
{
  public:
    LibraryLoadException(const std::string error_desc) : PluginException(error_desc) {}
};

/**
 * @class LibraryUnloadException
 * @brief An exception class thrown when plugins is unable to unload the library associated with a given plugin
 */
class LibraryUnloadException: public PluginException
{
  public:
    LibraryUnloadException(const std::string error_desc) : PluginException(error_desc) {}
};

/**
 * @class CreateClassException
 * @brief An exception class thrown when plugins is unable to create the class associated with a given plugin
 */
class CreateClassException: public PluginException
{
  public:
    CreateClassException(const std::string error_desc) : PluginException(error_desc) {}
};

}
