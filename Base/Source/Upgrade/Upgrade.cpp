#include "Upgrade.h"


CUpgrade::CUpgrade()
    : m_Mesh(NULL)
    , m_SpriteEntity(NULL)
    , m_Scrap(0)
    , m_NumFighter(0)
    , TranslateRight(0)
{

}

CUpgrade::~CUpgrade()
{

}

void CUpgrade::Init(std::string _meshName)
{ 
    m_Mesh = MeshBuilder::GetInstance()->GetMesh(_meshName);
    if (m_Mesh == nullptr)
        return;

    //m_SpriteEntity = new SpriteEntity(m_Mesh);
    //m_SpriteEntity->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
    //m_SpriteEntity->SetScale(Vector3(10.0f, 10.0f, 10.0f));
    //m_SpriteEntity->SetTextRenderMode(SpriteEntity::MODE_2D);
    
    position.Set(0.0f, 0.0f, 0.0f);
    scale.Set(10.0f, 10.0f, 10.0f);
    EntityManager::GetInstance()->AddEntity(this, false);
}

// Scraps
int CUpgrade::GetScrap(void)
{
    return m_Scrap;
}

void CUpgrade::SetScrap(int m_Scrap)
{
    this->m_Scrap = m_Scrap;
}

void CUpgrade::IncreaseScrap(int m_Scrap)
{
    this->m_Scrap += m_Scrap;
}

void CUpgrade::DecreaseScrap(int m_Scrap)
{
    this->m_Scrap -= m_Scrap;
}

// Number of Fighters
int CUpgrade::GetNumFighter(void)
{
    return m_NumFighter;
}

void CUpgrade::SetNumFighter(int m_NumFighter)
{
    this->m_NumFighter = m_NumFighter;
}

void CUpgrade::Update(double dt)
{
    TranslateRight += dt * 100;
}

void CUpgrade::RenderUI(void)
{
    MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
    modelStack.PushMatrix();
    modelStack.Translate(position.x + TranslateRight, position.y, position.z);
    modelStack.Scale(scale.x, scale.y, scale.z);
    // cout << theDetailLevel << endl;
    RenderHelper::RenderMesh(m_Mesh);
    modelStack.PopMatrix();
}