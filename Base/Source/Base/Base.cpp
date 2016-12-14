#include "Base.h"
#include "../EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"

Base::Base(void) : GenericEntity(NULL)
{
}

Base::~Base(void)
{
}

void Base::Init(void)
{
	position.Set(0, 0, 0);
	direction.Set(0, 0, 1);

	InitLOD("Base1", "Base1", "Base1");
	scale.Set(10, 10, 10);

	m_eEntityType = EntityBase::BASE;
	EntityManager::GetInstance()->AddEntity(this, true);
}

void Base::Update(double dt)
{
}

void Base::Render(void)
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
}