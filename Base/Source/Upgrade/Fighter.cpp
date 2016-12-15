#include "Fighter.h"
#include "../EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"

CFighter::CFighter(void)
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
    , store(0)
    , bob(0.0)
{

}

CFighter::~CFighter(void)
{

}

void CFighter::Init(void)
{
    // Set Entity Type
    SetEntityType(EntityBase::UPGRADE);
    // Set the default values
    defaultPosition.Set(0, 0, 10);
    defaultTarget.Set(0, 0, 0);
    defaultUp.Set(0, 1, 0);
    // Set the current values
    position.Set(35.0f, 10.0f, 0.0f);
    target.Set(0.0f, 0.0f, 0.0f);
    up.Set(0.0f, 1.0f, 0.0f);
    // Set Boundary
    maxBoundary.Set(1, 1, 1);
    minBoundary.Set(-1, -1, -1);
    // Set mass
    m_fMass = 10.f;
    // Set speed
    m_dSpeed = 5.0;
    // Set force
    force.Set(0, 0, 0);
    // Set acceleration
    acceleration.Set(0, 0, 0);
    // Initialise the LOD meshes
    InitLOD("Fighter", "Fighter", "Fighter");
    // Initialise the Collider
    this->SetCollider(true);
    this->SetAABB(Vector3(1, 1, 1), Vector3(-1, -1, -1));
    // Add to EntityManager
    EntityManager::GetInstance()->AddEntity(this, true);
}

Vector3 CFighter::Forward(double dt)
{
    Vector3 viewVector = (target - position);
    //position += viewVector * (float)m_dSpeed * (float)dt;
    //front.Set(position.x + 10, position.y, position.z);
    return viewVector;
}

void CFighter::Shoot(Vector3 position, Vector3 target, CFighter* _source)
{

}

void CFighter::Dodge(void)
{

}

// Reset this player instance to default
void CFighter::Reset(void)
{
    // Set the current values to default values
    position = defaultPosition;
    target = defaultTarget;
    up = defaultUp;
}

void CFighter::SetHP(int m_HP)
{
    this->m_HP = m_HP;
}

int CFighter::GetHP(void)
{
    return this->m_HP;
}


// Set position
void CFighter::SetPos(const Vector3& pos)
{
    position = pos;
}

// Set target
void CFighter::SetTarget(const Vector3& target)
{
    this->target = target;
}

// Set Up
void CFighter::SetUp(const Vector3& up)
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
Vector3 CFighter::GetPos(void) const
{
    return this->position;
}

// Get target
Vector3 CFighter::GetTarget(void) const
{
    return this->target;
}

// Get Up
Vector3 CFighter::GetUp(void) const
{
    return this->up;
}

//// Get the terrain for the player info
//GroundEntity* CFighter::GetTerrain(void)
//{
//    return this->m_pTerrain;
//}

void CFighter::Preconditions(double dt)
{
    float len = (target - position).LengthSquared();
    Vector3 test = position;
    float test1 = (target - (test + Forward() * (float)m_dSpeed * (float)dt)).LengthSquared();
    if (len >= test1)
        m_StateList.push_back(CFighter::FORWARD);
}

// Update
void CFighter::Update(double dt)
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
        if (it == CFighter::FORWARD)
        {
            force += Forward().Normalized() * (float)m_dSpeed;
        }
        if (it == CFighter::SHOOT)
        {

        }
    }

    //bob += dt;
    //if (bob >= 5.0)
    //{
    //    target = Vector3(Math::RandFloatMinMax(-400, 400), 0, Math::RandFloatMinMax(-400, 400));
    //    bob = 0.0;
    //}

    ClearList();

    // Constrain the position
    //Constrain();

    // Update the target
    if (position.z > 400.0f)
        target.z = position.z * -1;
    else if (position.z < -400.0f)
        target.z = position.z * -1;
}

// Constrain the position within the borders
void CFighter::Constrain(void)
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
void CFighter::Render(void)
{
    float angle = Math::RadianToDegree(atan2(velocity.z, velocity.x));
    MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
    modelStack.PushMatrix();
    modelStack.Translate(position.x, position.y, position.z);
    modelStack.Rotate(90, 0, 1, 0);
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
}