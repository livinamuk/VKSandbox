#pragma once
#include "WeaponCommon.h"

namespace WeaponManager {
    void Init();
    void SortList();
    int GetWeaponCount();
    int GetAmmoTypeCount();
    //void PreLoadWeaponPickUpConvexHulls();
    WeaponInfo* GetWeaponInfoByName(const std::string& name);
    WeaponInfo* GetWeaponInfoByIndex(int index);
    AmmoInfo* GetAmmoInfoByName(const std::string& name);
    AmmoInfo* GetAmmoInfoByIndex(int index);
}