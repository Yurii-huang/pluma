#ifndef SIMPLE_WARRIOR_HPP
#define SIMPLE_WARRIOR_HPP

#include "Warrior.hpp"
#include <Pluma/Connector.hpp>

/////////////////////////////////////////////////////////
/// Default Warrior
/////////////////////////////////////////////////////////
class SimpleWarrior: public Warrior{
public:
    std::string getDescription();
};

/////////////////////////////////////////////////////////
/// SimpleWarriorProvider provide default warriors
/////////////////////////////////////////////////////////
PLUMA_INHERIT_PROVIDER(SimpleWarrior, Warrior);

PLUMA_CONNECTOR bool connect(pluma::Host& host){ host.add( new SimpleWarriorProvider() ); return true;}

#endif // SIMPLE_WARRIOR_HPP
