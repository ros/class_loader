#include "meta_object.h"
#include "class_loader.h"

namespace plugins
{
namespace plugins_private
{

void AbstractMetaObjectBase::addOwningClassLoader(ClassLoader* loader)
{
  ClassLoaderVector& v = associated_class_loaders_;
  if(std::find(v.begin(), v.end(), loader) == v.end())
    v.push_back(loader);
}

void AbstractMetaObjectBase::removeOwningClassLoader(const ClassLoader* loader)
{
  ClassLoaderVector& v = associated_class_loaders_;
  ClassLoaderVector::iterator itr = std::find(v.begin(), v.end(), loader);
  if(itr != v.end())
    v.erase(itr);
}

bool AbstractMetaObjectBase::isOwnedBy(const ClassLoader* loader)
{
  ClassLoaderVector& v = associated_class_loaders_;
  ClassLoaderVector::iterator itr = std::find(v.begin(), v.end(), loader);
  return(itr != v.end());
}

bool AbstractMetaObjectBase::isOwnedByAnybody()
{
  return(associated_class_loaders_.size() > 0);
}

}

}
