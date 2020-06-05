#ifndef JAGUAR_HPP
#define JAGUAR_HPP

#include "Warrior.hpp"
#include <Pluma/Connector.hpp>

/////////////////////////////////////////////////////////
/// A Jaguar is a Warrior
/////////////////////////////////////////////////////////
class Jaguar: public Warrior{
public:
    std::string getDescription();
};

/////////////////////////////////////////////////////////
/// A JaguarProvider is a WarriorProvider,
/// provide warriors of type Jaguar
/////////////////////////////////////////////////////////
PLUMA_INHERIT_PROVIDER(Jaguar, Warrior);

PLUMA_CONNECTOR bool connect(pluma::Host& host){ host.add( new JaguarProvider() ); return true;}

#endif // JAGUAR_HPP
