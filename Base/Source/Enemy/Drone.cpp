#include "Drone.h"
#include "../EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"

CDrone::CDrone(void)
    : GenericEntity(NULL)
    , LockedOn(NULL)
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
    , m_dFollowSpeed(0.0)
    , m_dSpeed(0.0)
    , m_HP(1)
{

}

CDrone::~CDrone(void)
{

}

void CDrone::Init(void)
{
    // Set Entity Type
    SetEntityType(EntityBase::ENEMY);
    // Damaged
    SetDamaged(false);
    // Set the default values
    defaultPosition.Set(0, 0, 0);
    defaultTarget.Set(0, 0, 0);
    defaultUp.Set(0, 1, 0);
    // Set the current values
    position.Set(0.0f, 0.0f, 0.0f);
    target.Set(0.0f, 0.0f, 0.0f);
    up.Set(0.0f, 1.0f, 0.0f);
    // Set Boundary
    maxBoundary.Set(1, 1, 1);
    minBoundary.Set(-1, -1, -1);
    // Set mass
    m_fMass = 10.f;
    // Set speed
    m_dFollowSpeed = 40.0;
    m_dSpeed = 20.0;
    // Set force
    force.Set(0, 0, 0);
    // Set acceleration
    acceleration.Set(0, 0, 0);
    // Initialise the LOD meshes
    InitLOD("DDrone", "DDrone", "DDrone");
    // Initialise the Collider
    this->SetCollider(true);
    this->SetAABB(Vector3(1, 1, 1), Vector3(-1, -1, -1));
    // Add to EntityManager
    EntityManager::GetInstance()->AddEntity(this, true);
}

// Reset this player instance to default
void CDrone::Reset(void)
{
    // Set the current values to default values
    position = defaultPosition;
    target = defaultTarget;
    up = defaultUp;
}

// HP
void CDrone::SetHP(int m_HP)
{
    this->m_HP = m_HP;
}

int CDrone::GetHP(void)
{
    return this->m_HP;
}

// Set position
void CDrone::SetPos(const Vector3& pos)
{
    position = pos;
}

// Set target
void CDrone::SetTarget(const Vector3& target)
{
    this->target = target;
}

// Set Up
void CDrone::SetUp(const Vector3& up)
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
Vector3 CDrone::GetPos(void) const
{
    return this->position;
}

// Get target
Vector3 CDrone::GetTarget(void) const
{
    return this->target;
}

// Get Up
Vector3 CDrone::GetUp(void) const
{
    return this->up;
}

//// Get the terrain for the player info
//GroundEntity* CFighter::GetTerrain(void)
//{
//    return this->m_pTerrain;
//}

// Detect enemy
void CDrone::Detect(void)
{
    ////for (auto it : CGrid::GetListOfObject())
    //for (auto it : EntityManager::GetInstance()->entityList)
    //{
    //    if (it->GetEntityType() == EntityBase::ENEMY)
    //    {
    //        //cout << (it->GetPosition() - this->position).LengthSquared() << endl;
    //        if ((it->GetPosition() - this->position).LengthSquared() < 500.f)
    //        {
    //            LockedOn = it;
    //            break;
    //        }
    //        else if (LockedOn != NULL)
    //        {

    //        }
    //    }
    //}
}

// Go to base
void CDrone::GoToTarget(double dt)
{
    position += (target - position).Normalized() * m_dSpeed * dt;
}

// Aim at locked on enemy
void CDrone::Aim(void)
{
    target = LockedOn->GetPosition() - position;
}

// Follow locked on enemy
void CDrone::Follow(double dt)
{
    position += (target - position).Normalized() * m_dFollowSpeed * dt;
}

void CDrone::Preconditions(double dt)
{
    m_StateList.push_back(CDrone::DETECT);
    if (LockedOn == NULL)
    {
        m_StateList.push_back(CDrone::GOTOTARGET);
    }
    else
    {
        m_StateList.push_back(CDrone::FOLLOW);
        m_StateList.push_back(CDrone::AIM);
    }
}

// Update
void CDrone::Update(double dt)
{
    Preconditions(dt);

    for (auto it : m_StateList)
    {
        if (it == CDrone::DETECT)
        {
            Detect();
        }
        if (it == CDrone::GOTOTARGET)
        {
            GoToTarget();
        }
        if (it == CDrone::AIM)
        {
            Aim();
        }
        if (it == CDrone::FOLLOW)
        {
            Follow();
        }
        if (it == CDrone::SHOOT)
        {

        }
    }

    ClearList();

    if (GetDamaged())
        m_HP--;

    if (m_HP <= 0)
        SetIsDone(true);
}

// Constrain the position within the borders
void CDrone::Constrain(void)
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
void CDrone::Render(void)
{

    MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
    modelStack.PushMatrix();
    modelStack.Translate(position.x, position.y, position.z);
    modelStack.Rotate(-90, 0, 1, 0);

    if (LockedOn)
    {
        float angle2 = Math::RadianToDegree(atan2(target.z, target.x));
        modelStack.Rotate(-angle2, 0, 1, 0);
    }
    else
    {
        float angle = Math::RadianToDegree(atan2(position.z, position.x));
        modelStack.Rotate(-angle, 0, 1, 0);
    }
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
}