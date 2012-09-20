#include <class_loader.h>
#include "base.h"
#include <gtest/gtest.h>

TEST(PluginsTest, basicLoad)
{
  plugins::ClassLoader loader1("libTestPlugins1.so", false);
  
  try
  {
    loader1.createInstance<Base>("Cat")->saySomething(); //See if lazy load works
  }
  catch(plugins::PluginException& e)
  {
    FAIL() << "PluginException: " << e.what() << "\n";
  }

  SUCCEED();
}

// Run all the tests that were declared with TEST()
int main(int argc, char **argv){
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


