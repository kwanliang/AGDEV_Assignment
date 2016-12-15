#include "DShip.h"
#include "../EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"

CDShip::CDShip(void)
    : GenericEntity(NULL)
    , defaultPosition(Vector3(0.0f, 0.0f, 0.0f))
    , defaultTarget(Vector3(0.0f, 0.0f, 0.0f))
    , defaultUp(Vector3(0.0f, 0.0f, 0.0f))
    , target(Vector3(0.0f, 0.0f, 0.0f))
    , up(Vector3(0.0f, 0.0f, 0.0f))
    , velocity(Vector3(0.0f, 0.0f, 0.0f))
    , force(Vector3(0.0f, 0.0f, 0.0f))
    , acceleration(Vector3(0.0f, 0.0f, 0.0f))
    , maxBoundary(Vector3(0.0f, 0.0f, 0.0f))
    , minBoundary(Vector3(0.0f, 0.0f, 0.0f))
    , m_fMass(0.f)
    , m_dSpeed(0.0)
    , m_HP(30)
{

}

CDShip::~CDShip(void)
{

}

void CDShip::Init(void)
{
    ShowAABB = true;
    // Set Entity Type
    SetEntityType(EntityBase::ENEMY);
    // Damaged
    SetDamaged(false);
    // Set the default values
    defaultPosition.Set(0, 0, 10);
    defaultTarget.Set(0, 0, 0);
    defaultUp.Set(0, 1, 0);
    // Set the current values
    position.Set(-35.0f, 10.0f, 0.0f);
    target.Set(10.0f, 0.0f, 450.0f);
    up.Set(0.0f, 1.0f, 0.0f);
    // Set Boundary
    maxBoundary.Set(1, 1, 1);
    minBoundary.Set(-1, -1, -1);
    // Set mass
    m_fMass = 100.f;
    // Set speed
    m_dSpeed = 5.0;
    // Set force
    force.Set(0, 0, 0);
    // Set acceleration
    acceleration.Set(0, 0, 0);
    // Initialise the LOD meshes
    InitLOD("DShip", "DShip", "DShip");
    // Set scale
    scale.Set(3.f, 3.f, 3.f);
    // Initialise the Collider
    this->SetCollider(true);
    this->SetAABB(Vector3(10.f, 10.f, 10.f), Vector3(-10.f, -10.f, -10.f));
    CDShip* result = this;
    // Add to EntityManager
    EntityManager::GetInstance()->AddEntity(result, true);
    CSceneNode* MainNode = CSceneGraph::GetInstance()->AddNode(result);
    if (MainNode == NULL)
        cout << "EntityManager::AddEntity: Unable to add to scene graph!" << endl;

    //for (int i = 0; i < 8; ++i)
    //{
    //    m_dshipSphere[i] = Create::Entity("sphere", Vector3(0.f, 0.f, 0.f));
    //    m_dshipSphere[i]->SetCollider(true);
    //    m_dshipSphere[i]->SetAABB(Vector3(1.0f, 1.0f, 1.0f), Vector3(-1.0f, -1.0f, -1.0f));
    //    CSceneNode* SphereNode = MainNode->AddChild(m_dshipSphere[i]);
    //    if (SphereNode == NULL)
    //        cout << "EntityManager::AddEntity: Unable to add to scene graph!" << endl;
    //}
}

Vector3 CDShip::Forward(double dt)
{
    Vector3 viewVector = (target - position);
    //position += viewVector * (float)m_dSpeed * (float)dt;
    //front.Set(position.x + 10, position.y, position.z);
    return viewVector;
}

void CDShip::Shoot(Vector3 position, Vector3 target, CDShip* _source)
{

}

void CDShip::Dodge(void)
{

}

// Reset this player instance to default
void CDShip::Reset(void)
{
    // Set the current values to default values
    position = defaultPosition;
    target = defaultTarget;
    up = defaultUp;
}

void CDShip::SetHP(int m_HP)
{
    this->m_HP = m_HP;
}

int CDShip::GetHP(void)
{
    return this->m_HP;
}

// Set position
void CDShip::SetPos(const Vector3& pos)
{
    position = pos;
}

// Set target
void CDShip::SetTarget(const Vector3& target)
{
    this->target = target;
}

// Set Up
void CDShip::SetUp(const Vector3& up)
{
    this->up = up;
}

//// Set the boundary for the player info
//void CFighter::SetBoundary(Vector3 max, Vector3 min)
//{
//    maxBoundary = max;
//    minBoundary = min;
//}

//// Set the terrain for the player info
//void CFighter::SetTerrain(GroundEntity* m_pTerrain)
//{
//    if (m_pTerrain != NULL)
//    {
//        this->m_pTerrain = m_pTerrain;
//
//        SetBoundary(this->m_pTerrain->GetMaxBoundary(), this->m_pTerrain->GetMinBoundary());
//    }
//       
//}

// Get position
Vector3 CDShip::GetPos(void) const
{
    return this->position;
}

// Get target
Vector3 CDShip::GetTarget(void) const
{
    return this->target;
}

// Get Up
Vector3 CDShip::GetUp(void) const
{
    return this->up;
}

//// Get the terrain for the player info
//GroundEntity* CFighter::GetTerrain(void)
//{
//    return this->m_pTerrain;
//}

void CDShip::Preconditions(double dt)
{
    float len = (target - position).LengthSquared();
    Vector3 test = position;
    float test1 = (target - (test + Forward() * (float)m_dSpeed * (float)dt)).LengthSquared();
    if (len >= test1)
        m_StateList.push_back(CDShip::FORWARD);
}

// Update
void CDShip::Update(double dt)
{
    Preconditions(dt);
    //front = position + (target - position).Normalized();

    acceleration = force * (1.0f / m_fMass);
    position += velocity * dt;
    velocity += acceleration * dt * m_dSpeed;
    acceleration.SetZero();
    force.SetZero();
    for (auto it : m_StateList)
    {
        if (it == CDShip::FORWARD)
        {
            force += Forward().Normalized() * (float)m_dSpeed;
        }
        if (it == CDShip::SHOOT)
        {

        }
    }

    bob += dt;
    if (bob >= 5.0)
    {
        target = Vector3(Math::RandFloatMinMax(-400, 400), 0, Math::RandFloatMinMax(-400, 400));
        bob = 0.0;
    }

    ClearList();

    if (GetDamaged())
    {
        m_HP--;
        SetDamaged(false);
    }

    if (m_HP <= 0)
    {
        // remove from Scene Graph
        if (CSceneGraph::GetInstance()->DeleteNode(this) == true)
        {
        	cout << "*** This Entity removed ***" << endl;
        }
        SetIsDone(true);
    }

    //SetSpherePosition();

    // Constrain the position
    //Constrain();

    // Update the target
    if (position.z > 400.0f)
        target.z = position.z * -1;
    else if (position.z < -400.0f)
        target.z = position.z * -1;
}

//void CDShip::SetSpherePosition(void)
//{
//    if (!m_dshipSphere[0]->IsDone())
//        m_dshipSphere[0]->SetPosition(Vector3(position.x, position.y + 12.5f, position.z));
//    if (!m_dshipSphere[1]->IsDone())
//        m_dshipSphere[1]->SetPosition(Vector3(position.x + 10.f, position.y + 10.f, position.z));
//    if (!m_dshipSphere[2]->IsDone())
//        m_dshipSphere[2]->SetPosition(Vector3(position.x + 12.5f, position.y, position.z));
//    if (!m_dshipSphere[3]->IsDone())
//        m_dshipSphere[3]->SetPosition(Vector3(position.x + 10.f, position.y - 10.f, position.z));
//    if (!m_dshipSphere[4]->IsDone())
//        m_dshipSphere[4]->SetPosition(Vector3(position.x, position.y - 12.5f, position.z));
//    if (!m_dshipSphere[5]->IsDone())
//        m_dshipSphere[5]->SetPosition(Vector3(position.x - 10.f, position.y - 10.f, position.z));
//    if (!m_dshipSphere[6]->IsDone())
//        m_dshipSphere[6]->SetPosition(Vector3(position.x - 12.5f, position.y, position.z));
//    if (!m_dshipSphere[7]->IsDone())
//        m_dshipSphere[7]->SetPosition(Vector3(position.x - 10.f, position.y + 10.f, position.z));
//
//}

// Constrain the position within the borders
void CDShip::Constrain(void)
{
    if (position.x > maxBoundary.x - 1.0f)
        position.x = maxBoundary.x - 1.0f;
    if (position.z > maxBoundary.z - 1.0f)
        position.z = maxBoundary.z - 1.0f;
    if (position.x < minBoundary.x + 1.0f)
        position.x = minBoundary.x + 1.0f;
    if (position.z < minBoundary.z + 1.0f)
        position.z = minBoundary.z + 1.0f;

    // if the y position is not equal to terrain height at that position;
    // then update y position to the terrain height
}

// Render
void CDShip::Render(void)
{
    float angle = Math::RadianToDegree(atan2(velocity.z, velocity.x));
    MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
    modelStack.PushMatrix();
    modelStack.Translate(position.x, position.y, position.z);
    modelStack.Rotate(-90, 0, 1, 0);
    modelStack.Rotate(-angle, 0, 1, 0);
    modelStack.Scale(scale.x, scale.y, scale.z);
    if (GetLODStatus() == true)
    {
        if (theDetailLevel != NO_DETAILS)
        {
            // cout << theDetailLevel << endl;
            RenderHelper::RenderMesh(GetLODMesh());
        }
    }
    modelStack.PopMatrix();

    if (ShowAABB)
    {
        //std::cout << "WORKED?" << std::endl;
        modelStack.PushMatrix();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        modelStack.Translate(position.x, position.y, position.z);
        modelStack.Scale(maxAABB.x + (-minAABB.x) + 0.1f, maxAABB.y + (-minAABB.y) + 0.1f, maxAABB.z + (-minAABB.z) + 0.1f);
        RenderHelper::RenderMesh(HITBOX);
        modelStack.PopMatrix();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}