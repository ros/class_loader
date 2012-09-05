#include "base.h"
#include <plugins.h>
#include <vector>
#include <string>
#include <iostream>
using std::string;
using std::vector;
using std::cout;
using std::endl;

int main()
{
  plugins::ClassLoader loader1("libPlugins1.so", true);
  plugins::ClassLoader loader2("libPlugins2.so");

  cout << "Making a Cat from lib1 to see if lazy load works..." << endl;
  (loader1.createInstance<Base>("Cat"))->saySomething(); //See if lazy load works

  cout << "Now explicitly loading lib1..." << endl;
  loader1.loadLibrary(); //load it again!
  vector<string> availableClasses1 = loader1.getAvailableClasses<Base>();

  for(unsigned int c = 0; c < availableClasses1.size(); c++)
  {
    string current = availableClasses1.at(c);
    cout << "Creating " << current << "..., ptr = ";
    boost::shared_ptr<Base> obj = loader1.createInstance<Base>(current);
    cout << obj << endl;

    loader1.unloadLibrary();
  
    obj->saySomething();
  }

  vector<string> availableClasses2 = loader2.getAvailableClasses<Base>();
  for(unsigned int c = 0; c < availableClasses2.size(); c++)
  {
    string current = availableClasses2.at(c);
    cout << "Creating " << current << "..., ptr = ";
    boost::shared_ptr<Base> obj = loader2.createInstance<Base>(current);
    cout << obj << endl;
    obj->saySomething();  
  }


};
