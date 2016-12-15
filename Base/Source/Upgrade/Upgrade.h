#pragma once

#include "../EntityManager.h"
#include "../SpriteEntity.h"
#include "MeshBuilder.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"

class CUpgrade : public EntityBase
{
private:
    int m_Scrap;
    size_t m_NumFighter;
    Mesh* m_Mesh;
    SpriteEntity* m_SpriteEntity;

    // Animation
    float TranslateRight;

public:
    CUpgrade();
    ~CUpgrade();

    // Scraps
    int GetScrap(void);
    void SetScrap(int m_Scrap);
    void IncreaseScrap(int m_Scrap);
    void DecreaseScrap(int m_Scrap);

    // Number of Fighters
    int GetNumFighter(void);
    void SetNumFighter(int m_NumFighter);

    void Init(std::string _meshName);
    void Update(double dt = 0.0333f);
    void RenderUI(void);
};