#include "Spawn.h"

CSpawn::CSpawn()
    : m_Wave(1)
    , m_EntityCount(0)
    , m_bWaveCleared(true)
{
}

CSpawn::~CSpawn()
{

}

int CSpawn::GetWave(void)
{
    return this->m_Wave;
}

void CSpawn::SetWave(int m_Wave)
{
    this->m_Wave = m_Wave;
}

bool CSpawn::GetIsWaveCleared(void)
{
    return this->m_bWaveCleared;
}

void CSpawn::SetIsWaveCleared(bool m_bWaveCleared)
{
    this->m_bWaveCleared = m_bWaveCleared;
}

int CSpawn::GetEntityCount(void)
{
    return this->m_EntityCount;
}

void CSpawn::SetEntityCount(int m_EntityCount)
{
    this->m_EntityCount = m_EntityCount;
}

void CSpawn::IncreaseEntityCount(void)
{
    this->m_EntityCount += 1;
}

void CSpawn::DecreaseEntityCount(void)
{
    this->m_EntityCount -= 1;
}

void CSpawn::Spawner(void)
{
    if (m_Wave < 5)
    {

        //CFighter* fighter = new CFighter();
        //fighter->Init();  

        for (int i = 0; i < 20; i++)
        {
            CDrone* drone = new CDrone();
            SpawnLocation[0] = Vector3(Math::RandFloatMinMax(800, 1000), Math::RandFloatMinMax(-500, 500), Math::RandFloatMinMax(-500, 500));
            drone->Init();
            drone->SetPosition(SpawnLocation[0]);
        }

        CDShip* dship = new CDShip();
        SpawnLocation[0] = Vector3(Math::RandFloatMinMax(800, 1000), Math::RandFloatMinMax(-500, 500), Math::RandFloatMinMax(-500, 500));
        dship->Init();
        dship->SetPosition(SpawnLocation[0]);
    }
    else
    {

    }
    m_bWaveCleared = false;
}

