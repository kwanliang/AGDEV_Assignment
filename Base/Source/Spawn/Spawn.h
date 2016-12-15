#pragma once

#include "../Enemy/DShip.h"
#include "../Enemy/Drone.h"
#include "../Asteroid/Asteroid.h"
#include "../Upgrade/Fighter.h"
#include "../Upgrade/Upgrade.h"

class CSpawn
{
private:
    int m_Wave;
    int m_EntityCount;
    bool m_bWaveCleared;

    Vector3 SpawnLocation[4];

public:
    CSpawn();
    ~CSpawn();

    int GetWave(void);
    void SetWave(int m_Wave);

    int GetEntityCount(void);
    void SetEntityCount(int m_EntityCount);
    void IncreaseEntityCount(void);
    void DecreaseEntityCount(void);

    bool GetIsWaveCleared(void);
    void SetIsWaveCleared(bool m_bWaveCleared);

    void Spawner(void);
};