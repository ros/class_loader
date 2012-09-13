#include "plugins.h"
#include <Poco/SharedLibrary.h>

namespace plugins
{

std::string systemLibrarySuffix()
{
  return(Poco::SharedLibrary::suffix());
}

};

