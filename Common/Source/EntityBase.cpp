#include "EntityBase.h"

EntityBase::EntityBase()
    : position(0.0f, 0.0f, 0.0f)
    , scale(1.0f, 1.0f, 1.0f)
    , isDone(false)
    , m_bCollider(false)
    , m_bLaser(false)
    , m_eEntityType(DEFAULT)
{
}

EntityBase::~EntityBase()
{
}

void EntityBase::Update(double _dt)
{
}

void EntityBase::Render()
{
}

void EntityBase::RenderUI()
{
}

bool EntityBase::IsDone()
{
	return isDone;
}

void EntityBase::SetIsDone(bool _value)
{
	isDone = _value;
}

bool EntityBase::GetDamaged(void)
{
    return m_bDamaged;
}

void EntityBase::SetDamaged(bool m_bDamaged)
{
    this->m_bDamaged = m_bDamaged;
}

// Check if this entity has a collider class parent
bool EntityBase::HasCollider(void) const
{
	return m_bCollider;
}

// Set the flag to indicate if this entity has a collider class parent
void EntityBase::SetCollider(const bool m_bCollider)
{
    this->m_bCollider = m_bCollider;
}

// Set the flag, bLaser
void EntityBase::SetIsLaser(const bool m_bLaser)
{
    this->m_bLaser = m_bLaser;
}

// Get the flag, bLaser
bool EntityBase::GetIsLaser(void) const
{
    return m_bLaser;
}

// Set the entity type
void EntityBase::SetEntityType(ENTITY_TYPE m_eEntityType)
{
    this->m_eEntityType = m_eEntityType;
}

// Get the entity type
EntityBase::ENTITY_TYPE EntityBase::GetEntityType(void)
{
    return m_eEntityType;
}