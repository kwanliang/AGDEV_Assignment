#include "Minimap.h"
#include "MeshBuilder.h"
#include "../EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "../PlayerInfo/PlayerInfo.h"

Minimap::Minimap()
{
	this->Background = MeshBuilder::GetInstance()->GetMesh("Minimap");
	this->MinimapView = MeshBuilder::GetInstance()->GetMesh("Enemy");
	this->Avatar = MeshBuilder::GetInstance()->GetMesh("Player");
	this->Enemy = MeshBuilder::GetInstance()->GetMesh("Enemy");
	this->Border = MeshBuilder::GetInstance()->GetMesh("Border");
}

Minimap::~Minimap()
{
}

void Minimap::Init(void)
{
	this->position.Set(335, 235, 1);
	this->scale.Set(100, 100, 100);
	this->viewVec.SetZero();
	angle = 0.0f;

	EntityManager::GetInstance()->AddEntity(this, true);
}

void Minimap::Update(double dt)
{
	viewVec = CPlayerInfo::GetInstance()->GetTarget() - CPlayerInfo::GetInstance()->GetPos();
	angle = Math::RadianToDegree(atan2(-viewVec.z, viewVec.x));
}

void Minimap::RenderUI(void)
{
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();

	//STENCILNG STUFF
	glEnable(GL_STENCIL_TEST);
	
	glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF); // Write to stencil buffer
	glDepthMask(GL_FALSE); // Don't write to depth buffer
	glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)
	
	//What is visible
	modelStack.PushMatrix();
	modelStack.Translate(position.x, position.y, position.z);
	modelStack.Scale(125, 125, 125);
	RenderHelper::RenderMesh(MinimapView);
	modelStack.PopMatrix();
	
	glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
	glStencilMask(0x00); // Don't write anything to stencil buffer
	glDepthMask(GL_TRUE); // Write to depth buffer
	
	//What is going to be seen
	modelStack.PushMatrix();
	modelStack.Translate(position.x, position.y, position.z);
	modelStack.Scale(8, 8, 8);
	RenderHelper::RenderMesh(Avatar);
	modelStack.PopMatrix();
	
	std::list<EntityBase*>::iterator entityThis, entityThisEnd;
	std::list<EntityBase*>::iterator entityThat, entityThatEnd;
	
	entityThisEnd = EntityManager::GetInstance()->entityList.end();
	for (entityThis = EntityManager::GetInstance()->entityList.begin(); entityThis != entityThisEnd; ++entityThis)
	{
		EntityBase *thisEntity = dynamic_cast<EntityBase*>(*entityThis);
		if (thisEntity->IsDone())
		{
			continue;
		}
		//if (thisEntity->GetEntityType() != EntityBase::ENTITY_TYPE::ENEMY)
		if (thisEntity->GetEntityType() != EntityBase::ENTITY_TYPE::ASTEROID)
		{
			continue;
		}
		modelStack.PushMatrix();
		modelStack.Translate(position.x, position.y, position.z);
		modelStack.Rotate(angle - 90, 0, 0, 1);
		modelStack.Translate(-thisEntity->GetPosition().x / 5 + CPlayerInfo::GetInstance()->GetPos().x / 5, -thisEntity->GetPosition().z / 5 + CPlayerInfo::GetInstance()->GetPos().z / 5, 0);
		modelStack.Scale(8, 8, 8);
		RenderHelper::RenderMesh(Enemy);
	
		modelStack.PopMatrix();
	}
	
	modelStack.PushMatrix();
	modelStack.Translate(position.x, position.y, position.z);
	modelStack.Rotate(angle - 90, 0, 0, 1);
	modelStack.Translate(-CPlayerInfo::GetInstance()->GetPos().x / 5, -CPlayerInfo::GetInstance()->GetPos().z / 5, 0);
	modelStack.Scale(400, 400, 400);
	RenderHelper::RenderMesh(Background);
	modelStack.PopMatrix();
	
	glDisable(GL_STENCIL_TEST);
	
	modelStack.PushMatrix();
	modelStack.Translate(position.x, position.y, position.z);
	modelStack.Scale(125, 125, 125);
	RenderHelper::RenderMesh(Border);
	modelStack.PopMatrix();
}