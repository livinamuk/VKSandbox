#pragma once
#include "WeaponCommon.h"

namespace WeaponManager {
    void Init();
    void SortList();
    int GetWeaponCount();
    int GetAmmoTypeCount();
    //void PreLoadWeaponPickUpConvexHulls();
    WeaponInfo* GetWeaponInfoByName(std::string name);
    WeaponInfo* GetWeaponInfoByIndex(int index);
    AmmoInfo* GetAmmoInfoByName(std::string name);
    AmmoInfo* GetAmmoInfoByIndex(int index);
}