//created by katie
//main file... who would've guessed?

#include <Geode/Geode.hpp>
#include "DataManagement/DataManager.hpp"
using namespace geode::prelude;

$on_mod(Loaded) {
    AutoPauseMod::DataManagement::DataManager::GetSingleton();
}