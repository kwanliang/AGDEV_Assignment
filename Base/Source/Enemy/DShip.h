#pragma once

#include "../GenericEntity.h"
#include "../GroundEntity.h"
#include "../GOAP/GOAP.h"
#include "../Projectile/Projectile.h"

class Mesh;

class CDShip : public GenericEntity, public CGOAP
{
protected:

    Vector3 defaultPosition, defaultTarget, defaultUp;
    Vector3 target, up, right, velocity, force, acceleration;
    Vector3 maxBoundary, minBoundary;

    float m_fMass;

    double m_dSpeed;

    float store;

    double bob;

    int m_HP;

    GenericEntity* m_dshipSphere[8];

public:
    enum STATES
    {
        FORWARD,
        ROTATELEFT,
        ROTATERIGHT,
        SHOOT,
        DODGE,
        NUM_STATES,
    }state;

    CDShip(void);
    virtual ~CDShip(void);

    void Init(void);

    Vector3 Forward(double dt = 0.0333f);
    void Shoot(Vector3 position, Vector3 target, CDShip* _source);
    void Dodge(void);

    //void SetSpherePosition(void);

    void Preconditions(double dt = 0.0333f);

    // Reset this player instance to default
    void Reset(void);

    // HP
    void SetHP(int m_HP);
    int GetHP(void);

    // Set position
    void SetPos(const Vector3& pos);
    // Set target
    void SetTarget(const Vector3& target);
    // Set Up
    void SetUp(const Vector3& up);
    //// Set the boundary for the player info
    //void SetBoundary(Vector3 max, Vector3 min);

    // Get position
    Vector3 GetPos(void) const;
    // Get target
    Vector3 GetTarget(void) const;
    // Get Up
    Vector3 GetUp(void) const;
    //// Get the terrain for the player info
    //GroundEntity* GetTerrain(void);

    // Update
    void Update(double dt = 0.0333f);

    // Constrain the position within the borders
    void Constrain(void);
    // Render
    void Render(void);
};