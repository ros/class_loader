#include <iostream>

#include <class_loader/class_loader.h>

#include "base.h"

class Dog : public Base
{
public:
  void saySomething()
  {
    printf("Bark\n");
  }
};

class Cat : public Base
{
public:
  void saySomething()
  {
    printf("Meow\n");
  }
};

class Duck : public Base
{
public:
  void saySomething()
  {
    printf("Quack\n");
  }
};

class Cow : public Base
{
public:
  void saySomething()
  {
    printf("Moooo\n");
  }
};

class Sheep : public Base
{
public:
  void saySomething()
  {
    printf("Baaah\n");
  }
};

CLASS_LOADER_REGISTER_CLASS(Dog, Base);
CLASS_LOADER_REGISTER_CLASS(Cat, Base);
CLASS_LOADER_REGISTER_CLASS(Duck, Base);
CLASS_LOADER_REGISTER_CLASS(Cow, Base);
CLASS_LOADER_REGISTER_CLASS(Sheep, Base);
