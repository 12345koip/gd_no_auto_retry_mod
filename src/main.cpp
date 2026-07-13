#include <Geode/Geode.hpp>
#include "DataManagement/DataManager.hpp"

$on_mod(Loaded) {
    AutoPauseMod::DataManagement::DataManager::GetSingleton();
}