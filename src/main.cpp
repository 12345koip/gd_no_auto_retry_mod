//created by katie
//main file... who would've guessed?

#include <Geode/Geode.hpp>
#include "DataManagement/DataManager.hpp"
using namespace geode::prelude;

$on_mod(Loaded) {
    /*
     *TODO:
     *- Load global waypoints
     *
     *Put data into a central temporary storage
     *Only load level waypoints lists as required
    */

    AutoPauseMod::DataManagement::DataManager::GetSingleton();
}