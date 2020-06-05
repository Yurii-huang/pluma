#include <Pluma/Pluma.hpp>
#include "Warrior.hpp"
#include <iostream>

int main(int argc,char* argv[])
{
    pluma::Pluma pluma;
    pluma.acceptProviderType<WarriorProvider>();

//    // Add a default warrior provider
//    pluma.addProvider(new SimpleWarriorProvider() );

    // Load libraries
    std::cout<<"load plugin number:"<< pluma.loadFromFolder("./") << std::endl;

    // Get warrior providers into a vector
    std::vector<WarriorProvider*> providers;
    pluma.getProviders(providers);

    // Create a Warrior from each provider
    std::vector<WarriorProvider*>::iterator it;
    for (it = providers.begin() ; it != providers.end() ; ++it){
        // Create a warrior
        Warrior* warrior = (*it)->create();
        // Display it's description
        std::cout << warrior->getDescription() << std::endl;
        // (...)
        // don't forget to delete it
        delete warrior;
    }
    pluma.unloadAll();

    return 0;
}
