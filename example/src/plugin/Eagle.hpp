#ifndef EAGLE_HPP
#define EAGLE_HPP

#include "Warrior.hpp"
#include <Pluma/Connector.hpp>

/////////////////////////////////////////////////////////
/// An Eagle is a Warrior
/////////////////////////////////////////////////////////
class Eagle: public Warrior{
public:
    std::string getDescription();
};

/////////////////////////////////////////////////////////
/// An EagleProvider is a WarriorProvider,
/// provide warriors of type Eagle
/////////////////////////////////////////////////////////
PLUMA_INHERIT_PROVIDER(Eagle, Warrior);

PLUMA_CONNECTOR bool connect(pluma::Host& host){ host.add( new EagleProvider() ); return true;}

#endif // EAGLE_HPP
