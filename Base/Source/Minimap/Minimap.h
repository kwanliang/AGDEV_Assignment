#ifndef MINIMAP_H
#define MINIMAP_H

//#include "EntityBase.h"
#include "../SpriteEntity.h"
#include "Vector3.h"
#include "Vertex.h"
#include <string>

class Mesh;

class Minimap : public EntityBase
{
private:
	Mesh* Background;
	Mesh* MinimapView;
	Mesh* Avatar;
	Mesh* Enemy;
	Mesh* Border;

	Vector3 viewVec;

	float angle;

public:
	Minimap();
	virtual ~Minimap();

	void Init(void);
	void Update(double dt);
	void RenderUI(void);
};

#endif