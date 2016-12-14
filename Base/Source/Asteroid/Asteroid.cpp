#include "Asteroid.h"
#include "../EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "MeshBuilder.h"

Asteroid::Asteroid(void) : GenericEntity(NULL)
{
}

Asteroid::~Asteroid(void)
{
}

void Asteroid::Init(void)
{
	position.Set(Math::RandFloatMinMax(-400, 400), Math::RandFloatMinMax(-400, 400), Math::RandFloatMinMax(-400, 400));
	direction.SetZero();

	if (direction == Vector3(0, 0, 0))
	{
		direction.Set(Math::RandIntMinMax(-1, 1), Math::RandIntMinMax(-1, 1), Math::RandIntMinMax(-1, 1));
	}

	speed = (double)Math::RandFloatMinMax(-15, 15);

	small = false;

	ShowAABB = true;
	SetCollider(true);

	switch (Math::RandIntMinMax(1, 2))
	{
	case 1:
		InitLOD("Asteroid1", "Asteroid1", "Asteroid1");
		scale.Set(4, 4, 4);
		SetAABB(Vector3(18, 18, 18), Vector3(-18, -15, -18));
		break;
	case 2:
		InitLOD("Asteroid2", "Asteroid2", "Asteroid2");
		scale.Set(6, 6, 6);
		SetAABB(Vector3(25, 25, 25), Vector3(-25, -25, -25));
		break;
	}

	m_eEntityType = EntityBase::ASTEROID;
	EntityManager::GetInstance()->AddEntity(this, true);
}

void Asteroid::Update(double dt)
{
	position += direction * speed * dt;
}

void Asteroid::Render(void)
{
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
	modelStack.Translate(position.x, position.y, position.z);
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
void Asteroid::Split(void)
{
	int fragments = Math::RandIntMinMax(1, 3);

	for (int i = 0; i < fragments; i++)
	{
		position.Set(position.x + Math::RandFloatMinMax(-30, 30), position.y + Math::RandFloatMinMax(-30, 30), position.z + Math::RandFloatMinMax(-30, 30));
		EntityManager::GetInstance()->spawnPos.push_back(position);
	}

	return;
}