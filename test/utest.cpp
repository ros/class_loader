#include <class_loader.h>
#include "base.h"
#include <gtest/gtest.h>

/*****************************************************************************/
TEST(PluginsTest, basicLoad)
{
  try
  {
    plugins::ClassLoader loader1("libTestPlugins1.so", false);
    loader1.createInstance<Base>("Cat")->saySomething(); //See if lazy load works
  }
  catch(plugins::PluginException& e)
  {
    FAIL() << "PluginException: " << e.what() << "\n";
  }

  SUCCEED();
}

/*****************************************************************************/
TEST(PluginsTest, correctNonLazyLoadUnload)
{
  try
  {
    const std::string library_name = "libTestPlugins1.so";

    ASSERT_FALSE(plugins::plugins_private::isLibraryLoadedByAnybody(library_name));
    plugins::ClassLoader loader1(library_name, false);
    ASSERT_TRUE(plugins::plugins_private::isLibraryLoadedByAnybody(library_name));
    ASSERT_TRUE(loader1.isLibraryLoaded());
    loader1.unloadLibrary();
    ASSERT_FALSE(plugins::plugins_private::isLibraryLoadedByAnybody(library_name));
    ASSERT_FALSE(loader1.isLibraryLoaded());
    return;
  }
  catch(plugins::PluginException& e)
  {
    FAIL() << "PluginException: " << e.what() << "\n";
  }
  catch(...)
  {
    FAIL() << "Unhandled exception";
  }
}

/*****************************************************************************/
TEST(PluginsTest, correctLazyLoadUnload)
{
  const std::string library_name = "libTestPlugins1.so";
  try
  {
    ASSERT_FALSE(plugins::plugins_private::isLibraryLoadedByAnybody(library_name));
    plugins::ClassLoader loader1(library_name, true);
    ASSERT_FALSE(plugins::plugins_private::isLibraryLoadedByAnybody(library_name));
    ASSERT_FALSE(loader1.isLibraryLoaded());

    {
      boost::shared_ptr<Base> obj = loader1.createInstance<Base>("Cat");
      ASSERT_TRUE(plugins::plugins_private::isLibraryLoadedByAnybody(library_name));
      ASSERT_TRUE(loader1.isLibraryLoaded());
    }

    //The library will unload automatically when the only plugin object left is destroyed
    ASSERT_FALSE(plugins::plugins_private::isLibraryLoadedByAnybody(library_name));
    return;
  }
  catch(plugins::PluginException& e)
  {
    FAIL() << "PluginException: " << e.what() << "\n";
  }
  catch(...)
  {
    FAIL() << "Unhandled exception";
  }
}

/*****************************************************************************/

TEST(PluginsTest, nonExistentPlugin)
{
  plugins::ClassLoader loader1("libTestPlugins1.so", false);

  try
  {
    boost::shared_ptr<Base> obj = loader1.createInstance<Base>("Bear");
    if(obj == NULL)
      FAIL() << "Null object being returned instead of exception thrown.";

    obj->saySomething();
  }
  catch(const plugins::CreateClassException& e)
  {
    SUCCEED();
    return;
  }
  catch(...)
  {
    FAIL() << "Unknown exception caught.\n";
  }

  FAIL() << "Did not throw exception as expected.\n";
}

/*****************************************************************************/
TEST(PluginsTest, nonExistentLibrary)
{
  try
  {
    plugins::ClassLoader loader1("libDoesNotExist.so", false);
  }
  catch(const plugins::LibraryLoadException& e)
  {
    SUCCEED();
    return;
  }
  catch(...)
  {
    FAIL() << "Unknown exception caught.\n";
  }

  FAIL() << "Did not throw exception as expected.\n";
}

/*****************************************************************************/

class InvalidBase
{
};

TEST(PluginsTest, invalidBase)
{
  try
  {
    plugins::ClassLoader loader1("libTestPlugins1.so", false);
    if(loader1.isClassAvailable<InvalidBase>("Cat"))
    {
      FAIL() << "Cat should not be available for InvalidBase";
    }
    else if(loader1.isClassAvailable<Base>("Cat"))
    {
      SUCCEED();
      return;
    }
    else
      FAIL() << "Class not available for correct base class.";
  }
  catch(const plugins::LibraryLoadException& e)
  {
    FAIL() << "Unexpected exception";
  }
  catch(...)
  {
    FAIL() << "Unexpected and unknown exception caught.\n";
  }
}

/*****************************************************************************/


// Run all the tests that were declared with TEST()
int main(int argc, char **argv){
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


