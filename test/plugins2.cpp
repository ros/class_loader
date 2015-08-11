#include <iostream>

#include <class_loader/class_loader.h>

#include "base.h"

class Robot : public Base
{
public:
  void saySomething()
  {
    printf("Beep boop\n");
  }
};

class Alien : public Base
{
public:
  void saySomething()
  {
    printf("Znornoff!!!\n");
  }
};

class Monster : public Base
{
public:
  void saySomething()
  {
    printf("BEAAAHHHH\n");
  }
};

class Zombie : public Base
{
public:
  void saySomething()
  {
    printf("Brains!!!\n");
  }
};

CLASS_LOADER_REGISTER_CLASS(Robot, Base);
CLASS_LOADER_REGISTER_CLASS(Alien, Base);
CLASS_LOADER_REGISTER_CLASS(Monster, Base);
CLASS_LOADER_REGISTER_CLASS(Zombie, Base);
