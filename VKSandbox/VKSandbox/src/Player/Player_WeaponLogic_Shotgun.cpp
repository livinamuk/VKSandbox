#include "Player.h"

void Player::UpdateShotgunGunLogic() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);
    AmmoInfo* ammoInfo = WeaponManager::GetAmmoInfoByName(weaponInfo->ammoType);
    AmmoState* ammoState = GetAmmoStateByName(weaponInfo->ammoType);
}
