#include "base.h"
#include <iostream>
#include <plugins.h>

class Robot : public Base
{
  public:
    virtual void saySomething(){std::cout << "Beep boop" << std::endl;}
};

class Alien : public Base
{
  public:
    virtual void saySomething(){std::cout << "Znornoff!!!" << std::endl;}
};

class Monster : public Base
{
  public:
    virtual void saySomething(){std::cout << "BEAAAHHHH" << std::endl;}
};

class Zombie : public Base
{
  public:
    virtual void saySomething(){std::cout << "Brains!!!" << std::endl;}
};


PLUGINS_REGISTER_CLASS(Robot, Base);
PLUGINS_REGISTER_CLASS(Alien, Base);
PLUGINS_REGISTER_CLASS(Monster, Base);
PLUGINS_REGISTER_CLASS(Zombie, Base);

