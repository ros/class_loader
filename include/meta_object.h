//Note: This header defines a simplication of Poco::MetaObject that allows us to tag MetaObjects with an associated library name.

#ifndef PLUGINS_PRIVATE_META_OBJECT_DEFINED
#define PLUGINS_PRIVATE_META_OBJECT_DEFINED

#include <iostream>
#include <vector>

namespace plugins
{

class ClassLoader; //Forward declaration

namespace plugins_private 
{

/**
 * @class AbstractMetaObjectBase
 * @brief A base class for MetaObjects that excludes a polymorphic type parameter. Subclasses are class templates though.
 */
class AbstractMetaObjectBase
{
  typedef std::vector<plugins::ClassLoader*> ClassLoaderVector;

  public:

    /**
     * @brief Constructor for the class
     */
    AbstractMetaObjectBase() :  
    associated_library_path_("Unknown")
    {
    }

    /**
     * @brief Destructor for the class
     */
    virtual ~AbstractMetaObjectBase()
    {
    } 

    /**
     * @brief Gets the path to the library associated with this factory
     * @return Library path as a std::string
     */
    std::string getAssociatedLibraryPath(){return(associated_library_path_);}

    /**
     * @brief Sets the path to the library associated with this factory
     */
    void setAssociatedLibraryPath(std::string library_path){associated_library_path_ = library_path;}

    /**
     * @brief Associates a ClassLoader owner with this factory,
     * @param loader Handle to the owning ClassLoader.
     */
    void addOwningClassLoader(ClassLoader* loader);

    /**
     * @brief Removes a ClassLoader that is an owner of this factory
     * @param loader Handle to the owning ClassLoader. 
     */
    void removeOwningClassLoader(const ClassLoader* loader);

    /**
     * @brief Indicates if the factory is within the usable scope of a ClassLoader
     * @param loader Handle to the owning ClassLoader. 
     */
    bool isOwnedBy(const ClassLoader* loader);

    /**
     * @brief Indicates if the factory is within the usable scope of any ClassLoader
     */
    bool isOwnedByAnybody();

  private:
    ClassLoaderVector associated_class_loaders_;    
    std::string associated_library_path_;
};

/**
 * @class AbstractMetaObject
 * @brief Abstract base class for factories where polymorphic type variable indicates base class for plugin interface.
 * @parm B The base class interface for the plugin
 */
template <class B>
class AbstractMetaObject : public AbstractMetaObjectBase
{
  public:
    /**
     * @brief A constructor for this class
     * @param name The literal name of the class.
     */
    AbstractMetaObject(const char* name) : name_(name)
    {
    }

    /**
     * @brief Desonstructor for this class
     */
    virtual ~AbstractMetaObject()
    {
    }

    /**
     * @brief Gets the literal name of the class.
     * @return The literal name of the class as a C-string.
     */
    const char* name() const{return name_;}

    /**
     * @brief Defines the factory interface that the MetaObject must implement.
     * @return A pointer of parametric type B to a newly created object.
     */
    virtual B* create() const = 0;
	    /// Create a new instance of a class.
	    /// Cannot be used for singletons.

  private:
    AbstractMetaObject();
    AbstractMetaObject(const AbstractMetaObject&);
    AbstractMetaObject& operator = (const AbstractMetaObject&);

    const char* name_;
};

/**
 * @class MetaObject
 * @brief The actual factory. 
 * @parm C The derived class (the actual plugin)
 * @parm B The base class interface for the plugin
 */
template <class C, class B>
class MetaObject: public AbstractMetaObject<B>
{
  public:
    /**
     * @brief Constructor for the class
     */
    MetaObject(const char* name): AbstractMetaObject<B>(name)
    {
    }

    /**
     * @brief Destructor for the class
     */
    virtual ~MetaObject()
    {
      std::cout << "MetaObject being destroyed for class type = " << (this->name()) << std::endl;
    }

    /**
     * @brief The factory interface to generate an object. The object has type C in reality, though a pointer of the base class type is returned.
     * @return A pointer to a newly created plugin with the base class type (type parameter B)
     */
    B* create() const
    {
	    return new C;
    }
};

} // End namespace plugins_private
} // End namespace plugins

#endif 
