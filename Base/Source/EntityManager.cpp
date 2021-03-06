#include "EntityManager.h"
#include "EntityBase.h"
#include "Collider/Collider.h"
#include "../Source/WeaponInfo/Laser.h"
#include "Asteroid\Asteroid.h"
#include "../Source/Sound/SoundManager.h"

#include <iostream>
using namespace std;

// Update all entities
void EntityManager::Update(double _dt)
{
    // Update all entities
    std::list<EntityBase*>::iterator it, end;
    end = entityList.end();
    for (it = entityList.begin(); it != end; ++it)
    {
        (*it)->Update(_dt);
    }

    // Update the Scene Graph
    CSceneGraph::GetInstance()->Update();

    // Render the Spatial Partition
    if (theSpatialPartition)
        theSpatialPartition->Update();

    // Check for Collision amongst entities with collider properties
    CheckForCollision();

	//Spawns smaller asteroid
	while (spawnPos.size() > 0)
	{
		Vector3 currPos = spawnPos.back();
		spawnPos.pop_back();
		SpawnAss(currPos);
	}

	//SpawnAss();
    // Clean up entities that are done
    it = entityList.begin();
    while (it != end)
    {
        if ((*it)->IsDone())
        {
            // Delete if done
            delete *it;
            it = entityList.erase(it);
        }
        else
        {
            // Move on otherwise
            ++it;
        }
    }
}

// Render all entities
void EntityManager::Render()
{
	// Render all entities
	std::list<EntityBase*>::iterator it, end;
	end = entityList.end();
	for (it = entityList.begin(); it != end; ++it)
	{
		(*it)->Render();
	}

    // Render the Scene Graph
    CSceneGraph::GetInstance()->Render();

    // Render the Spatial Partition
    if (theSpatialPartition)
        theSpatialPartition->Render();
}

// Render the UI entities
void EntityManager::RenderUI()
{
	// Render all entities UI
	std::list<EntityBase*>::iterator it, end;
	end = entityList.end();
	for (it = entityList.begin(); it != end; ++it)
	{
		(*it)->RenderUI();
	}
}

// Add an entity to this EntityManager
void EntityManager::AddEntity(EntityBase* _newEntity, bool bAddToSpatialPartition)
{
    entityList.push_back(_newEntity);

    // Add to the Spatial Partition
    if (theSpatialPartition && bAddToSpatialPartition)
        theSpatialPartition->Add(_newEntity);
}

// Remove an entity from this EntityManager
bool EntityManager::RemoveEntity(EntityBase* _existingEntity)
{
	// Find the entity's iterator
	std::list<EntityBase*>::iterator findIter = std::find(entityList.begin(), entityList.end(), _existingEntity);

	// Delete the entity if found
	if (findIter != entityList.end())
	{
		delete *findIter;
		findIter = entityList.erase(findIter);

        // Remove from SceneNode too
        if (CSceneGraph::GetInstance()->DeleteNode(_existingEntity))
        {
            cout << "EntityManager::RemoveEntity: Unable to remove entity";
        }
        else
        {
            // Remove from the Spatial Partition
            if (theSpatialPartition)
                theSpatialPartition->Remove(_existingEntity);
        }

		return true;	
	}
	// Return false if not found
	return false;
}

// Mark an entity for deletion
bool EntityManager::MarkForDeletion(EntityBase* _existingEntity)
{
    // Find the entity's iterator
    std::list<EntityBase*>::iterator findIter = std::find(entityList.begin(), entityList.end(), _existingEntity);

    // Delete the entity if found
    if (findIter != entityList.end())
    {
        (*findIter)->SetIsDone(true);
        return true;
    }
    // Return false if not founds
    return false;
}

// Set a handle to the Spatial Partition to this class
void EntityManager::SetSpatialPartition(CSpatialPartition* theSpatialPartition)
{
    this->theSpatialPartition = theSpatialPartition;
}

// Constructor
EntityManager::EntityManager()
{
}

// Destructor
EntityManager::~EntityManager()
{
}

// Check for overlap
bool EntityManager::CheckOverlap(Vector3 thisMinAABB, Vector3 thisMaxAABB, Vector3 thatMinAABB, Vector3 thatMaxAABB)
{	
    if (((thatMinAABB >= thisMinAABB) && (thatMinAABB <= thisMaxAABB))
        ||
        ((thatMaxAABB >= thisMinAABB) && (thatMaxAABB <= thisMaxAABB)))
    {
        return true;
    }

    if (((thisMinAABB >= thatMinAABB) && (thisMinAABB <= thatMaxAABB))
        ||
        ((thisMaxAABB >= thatMinAABB) && (thisMaxAABB <= thatMaxAABB)))
    {
        return true;
    }

    if (((thisMinAABB >= thatMinAABB) && (thisMaxAABB <= thatMaxAABB))
        &&
        ((thisMaxAABB >= thatMinAABB) && (thisMaxAABB <= thatMaxAABB)))
    {
        return true;
    }

    if (((thatMinAABB >= thisMinAABB) && (thatMinAABB <= thisMaxAABB))
        &&
        ((thatMaxAABB >= thisMinAABB) && (thatMaxAABB <= thisMaxAABB)))
    {
        return true;
    }

	return false;
}

// Check if this entity's bounding sphere collided with that entity's bounding sphere 
bool EntityManager::CheckSphereCollision(EntityBase *ThisEntity, EntityBase *ThatEntity)
{
    // Get the colliders for the 2 entities
    CCollider *thisCollider = dynamic_cast<CCollider*>(ThisEntity);
    CCollider *thatCollider = dynamic_cast<CCollider*>(ThatEntity);

    // Get the minAABB and maxAABB for each entity
    Vector3 thisMinAABB = ThisEntity->GetPosition() + thisCollider->GetMinAABB();
    Vector3 thisMaxAABB = ThisEntity->GetPosition() + thisCollider->GetMaxAABB();
    Vector3 thatMinAABB = ThatEntity->GetPosition() + thatCollider->GetMinAABB();
    Vector3 thatMaxAABB = ThatEntity->GetPosition() + thatCollider->GetMaxAABB();

    // if Radius of bounding sphere of ThisEntity plus Radius of bounding sphere of ThatEntity is
    // greater than the distance squared between the 2 reference points of the 2 entities
    // then it could mean that they are colliding with each other
    if (DistanceSquaredBetween(thisMinAABB, thisMaxAABB) + DistanceSquaredBetween(thatMinAABB, thatMaxAABB) >
        DistanceSquaredBetween(ThisEntity->GetPosition(), ThatEntity->GetPosition()) * 2.0)
    {
        return true;
    }

	return false;
}

// Check if this entity collided with another entity, but both must have collider
bool EntityManager::CheckAABBCollision(EntityBase *ThisEntity, EntityBase *ThatEntity)
{
    // Get the colliders for the 2 entities
    CCollider *thisCollider = dynamic_cast<CCollider*>(ThisEntity);
    CCollider *thatCollider = dynamic_cast<CCollider*>(ThatEntity);

    // Get the minAABB and maxAABB for each entity
    Vector3 thisMinAABB = ThisEntity->GetPosition() + thisCollider->GetMinAABB();
    Vector3 thisMaxAABB = ThisEntity->GetPosition() + thisCollider->GetMaxAABB();
    Vector3 thatMinAABB = ThatEntity->GetPosition() + thatCollider->GetMinAABB();
    Vector3 thatMaxAABB = ThatEntity->GetPosition() + thatCollider->GetMaxAABB();

    // Check for overlap
    if (CheckOverlap(thisMinAABB, thisMaxAABB, thatMinAABB, thatMaxAABB))
        return true;

    // If AABB collision check fails, then we need to check the other corners of the bounding boxes to
    // do more collision checks with other points on each bounding box
    Vector3 altThisMinAABB = Vector3(thisMinAABB.x, thisMinAABB.y, thisMaxAABB.z);
    Vector3 altThisMaxAABB = Vector3(thisMaxAABB.x, thisMaxAABB.y, thisMinAABB.z);

    // Check for overlap
    if (CheckOverlap(altThisMinAABB, altThisMaxAABB, thatMinAABB, thatMaxAABB))
        return true;

	return false;
}

// Check for intersection between a line segment and a plane
bool EntityManager::GetIntersection(const float fDst1, const float fDst2, Vector3 P1, Vector3 P2, Vector3 &Hit)
{
    if ((fDst1 * fDst2) >= 0.0f)
        return false;
    if (fDst1 == fDst2)
        return false;
    Hit = P1 + (P2 - P1) * (-fDst1 / (fDst2 - fDst1));
    return true;
}

// Check for intersection between a line segment and a plane
bool EntityManager::CheckLineSegmentPlane(Vector3 line_start, Vector3 line_end,
    Vector3 minAABB, Vector3 maxAABB,
    Vector3 &Hit)
{
    if ((GetIntersection(line_start.x - minAABB.x, line_end.x - minAABB.x,
        line_start, line_end, Hit) && InBox(Hit, minAABB, maxAABB, 1))
        || (GetIntersection(line_start.y - minAABB.y, line_end.y -
        minAABB.y, line_start, line_end, Hit) && InBox(Hit, minAABB, maxAABB, 2))
        || (GetIntersection(line_start.z - minAABB.z, line_end.z -
        minAABB.z, line_start, line_end, Hit) && InBox(Hit, minAABB, maxAABB, 3))
        || (GetIntersection(line_start.x - maxAABB.x, line_end.x -
        maxAABB.x, line_start, line_end, Hit) && InBox(Hit, minAABB, maxAABB, 1))
        || (GetIntersection(line_start.y - maxAABB.y, line_end.y -
        maxAABB.y, line_start, line_end, Hit) && InBox(Hit, minAABB, maxAABB, 2))
        || (GetIntersection(line_start.z - maxAABB.z, line_end.z -
        maxAABB.z, line_start, line_end, Hit) && InBox(Hit, minAABB, maxAABB, 3)))
        return true;

    return false;
}

// Check two positions are within a box region
bool EntityManager::InBox(Vector3 Hit, Vector3 B1, Vector3 B2, const int Axis)
{
    if (Axis == 1 && Hit.z > B1.z && Hit.z < B2.z && Hit.y > B1.y && Hit.y < B2.y) return true;
    if (Axis == 2 && Hit.z > B1.z && Hit.z < B2.z && Hit.x > B1.x && Hit.x < B2.x) return true;
    if (Axis == 3 && Hit.x > B1.x && Hit.x < B2.x && Hit.y > B1.y && Hit.y < B2.y) return true;
    return false;
}

// Check if any Collider is colliding with another Collider
bool EntityManager::CheckForCollision(void)
{
    // Check for Collision
    std::list<EntityBase*>::iterator colliderThis, colliderThisEnd;
    std::list<EntityBase*>::iterator colliderThat, colliderThatEnd;

    colliderThisEnd = entityList.end();
    for (colliderThis = entityList.begin(); colliderThis != colliderThisEnd; ++colliderThis)
    {
        // Check if this entity is a CLaser type
        if ((*colliderThis)->GetIsLaser())
        {
            // Dynamic cast it to a CLaser class
            CLaser* thisEntity = dynamic_cast<CLaser*>(*colliderThis);

            // Check for collision with another collider class
            colliderThatEnd = entityList.end();
            int counter = 0;
            for (colliderThat = entityList.begin(); colliderThat != colliderThatEnd; ++colliderThat)
            {
                if (colliderThat == colliderThis)
                    continue;

                if ((*colliderThat)->HasCollider())
                {
                    Vector3 hitPosition = Vector3(0, 0, 0);

                    // Get the minAABB and maxAABB for (*colliderThat)
                    CCollider *thatCollider = dynamic_cast<CCollider*>(*colliderThat);
                    Vector3 thatMinAABB = (*colliderThat)->GetPosition() + thatCollider->GetMinAABB();
                    Vector3 thatMaxAABB = (*colliderThat)->GetPosition() + thatCollider->GetMaxAABB();

                    if (CheckLineSegmentPlane(thisEntity->GetPosition(),
                        thisEntity->GetPosition() - thisEntity->GetDirection() * thisEntity->GetLength(),
                        thatMinAABB, thatMaxAABB,
                        hitPosition) == true)
                    {
                        (*colliderThis)->SetIsDone(true);
                        (*colliderThat)->SetIsDone(true);

                        // remove from Scene Graph
                        if (CSceneGraph::GetInstance()->DeleteNode(*colliderThis) == true)
                        {
                            cout << "*** This Entity removed ***" << endl;
                        }

                        // remove from Scene Graph
                        if (CSceneGraph::GetInstance()->DeleteNode(*colliderThat) == true)
                        {
                            cout << "*** That Entity removed ***" << endl;
                        }
                    }

                }
            }
        }
        else if ((*colliderThis)->HasCollider())
        {
            // This object was derived from a CCollider class, then it will have Collision Detection methods
            // CCollider *thisCollider = dynamic_cast<CCollider*>(*colliderThis);
            EntityBase *thisEntity = dynamic_cast<EntityBase*>(*colliderThis);

            // Check for collision with another collider class
            colliderThatEnd = entityList.end();
            int counter = 0;	
            for (colliderThat = colliderThis; colliderThat != colliderThatEnd; ++colliderThat)
            {
                if (colliderThat == colliderThis)
                    continue;

                if ((*colliderThat)->HasCollider())
                {
                    EntityBase *thatEntity = dynamic_cast<EntityBase*>(*colliderThat);

                    //Bullet to Enemy
                    if (thisEntity->GetEntityType() == EntityBase::ENEMY)
                    {
                        if (CheckSphereCollision(thisEntity, thatEntity) == true)
                        {
                            if (CheckAABBCollision(thisEntity, thatEntity) == true)
                            {
                                thatEntity->SetIsDone(true);
                                thisEntity->SetDamaged(true);
                            }
                        }                  
                    }

                    if (CheckSphereCollision(thisEntity, thatEntity) == true)
                    {
                        if (CheckAABBCollision(thisEntity, thatEntity) == true)
                        {
							//Bullet to asteroid
							if (thisEntity->GetEntityType() == EntityBase::ENTITY_TYPE::ASTEROID && thatEntity->GetEntityType() != EntityBase::ENTITY_TYPE::ASTEROID)
							{
								if (CheckSphereCollision(thisEntity, thatEntity) == true)
								{
									if (CheckAABBCollision(thisEntity, thatEntity) == true)
									{
										Asteroid *thisAsteroid = dynamic_cast<Asteroid*>(*colliderThis);

										//HIT BIG ASTEROID
										if (!thisAsteroid->mini)
										{
											//Split stores position of big asteroid to spawnPos;
											thisAsteroid->Split();
											thatEntity->SetIsDone(true);
											thisEntity->SetIsDone(true);
										}

										//HIT SMALL ASTEROID
										if (thisAsteroid->mini)
										{
											thatEntity->SetIsDone(true);
											thisEntity->SetIsDone(true);
										}

										return true;
									}
								}
							}
							//Asteroid to Asteroid collision 
							else if (thisEntity->GetEntityType() == EntityBase::ENTITY_TYPE::ASTEROID &&
								thatEntity->GetEntityType() == EntityBase::ENTITY_TYPE::ASTEROID)
							{
								if (CheckSphereCollision(thisEntity, thatEntity) == true)
								{
									if (CheckAABBCollision(thisEntity, thatEntity) == true)
									{
										Asteroid *thisAsteroid = dynamic_cast<Asteroid*>(*colliderThis);
										Asteroid *thatAsteroid = dynamic_cast<Asteroid*>(*colliderThat);

										//Two small asteroid collide = Both destroyed
										if (thisAsteroid->mini && thatAsteroid->mini)
										{
											thisEntity->SetIsDone(true);
											thatEntity->SetIsDone(true);
										}
										//Small asteroid collide with Big asteroid = Small destroyed
										else if (thisAsteroid->mini && !thatAsteroid->mini)
										{
											thisEntity->SetIsDone(true);
										}
										//Small asteroid collide with Big asteroid = Small destroyed
										else if (!thisAsteroid->mini && thatAsteroid->mini)
										{
											thatEntity->SetIsDone(true);
										}
										//Two Big asteroid collide = Both Split
										if (!thisAsteroid->mini && !thatAsteroid->mini)
										{
											thisAsteroid->Split();
											thatAsteroid->Split();
											thisEntity->SetIsDone(true);
											thatEntity->SetIsDone(true);
										}
										//thisAsteroid->direction = -thisAsteroid->direction;
										//thatAsteroid->direction = -thatAsteroid->direction;
										return true;
									}
								}
							}
								
							////Bullet to Something / Something to Bullet
							//thisEntity->SetIsDone(true);
							//thatEntity->SetIsDone(true);

							//// remove from Scene Graph
							//if (CSceneGraph::GetInstance()->DeleteNode(*colliderThis) == true)
							//{
							//	cout << "*** This Entity removed ***" << endl;
							//}

							//// remove from Scene Graph
							//if (CSceneGraph::GetInstance()->DeleteNode(*colliderThat) == true)
							//{
							//	cout << "*** That Entity removed ***" << endl;
							//}
                        }
                    }
                }
            }
        }
    }
	return false;
}

//Spawn new asteroid based on pos
void EntityManager::SpawnAss(Vector3 pos)
{
	Asteroid * new_asteroid = new Asteroid();

	new_asteroid->mini = true;

	new_asteroid->SetPosition(pos);
	new_asteroid->direction.SetZero();
	if (new_asteroid->direction == Vector3(0, 0, 0))
	{
		new_asteroid->direction.Set(Math::RandIntMinMax(-1, 1), Math::RandIntMinMax(-1, 1), Math::RandIntMinMax(-1, 1));
	}
	new_asteroid->speed = (double)Math::RandFloatMinMax(-7, 7);

	new_asteroid->ShowAABB = true;
	new_asteroid->SetCollider(true);
	new_asteroid->InitLOD("Asteroid1", "Asteroid1", "Asteroid1");
	new_asteroid->SetScale(Vector3(2, 2, 2));
	new_asteroid->SetAABB(Vector3(9, 9, 9), Vector3(-9, -7.5f, -9));
	new_asteroid->SetEntityType(EntityBase::ASTEROID);
	EntityManager::GetInstance()->AddEntity(new_asteroid, true);
}