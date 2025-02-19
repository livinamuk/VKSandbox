#include "World.h"

#define WORLD_SECTOR_WIDTH 5
#define WORLD_SECTOR_HEIGHT 20

namespace World {

    Sector g_sectors[WORLD_SECTOR_WIDTH][WORLD_SECTOR_HEIGHT];

    void ResetSectorData();

    void NewCampainWorld() {
        ResetSectorData();   
    }

    void LoadDeathMatchMap() {
        ResetSectorData();
    }

    void ResetSectorData() {
        for (int x = 0; x < WORLD_SECTOR_WIDTH; x++) {
            for (int y = 0; y < WORLD_SECTOR_HEIGHT; y++) {
                g_sectors[x][y].ResetData();
            }
        }
    }
}