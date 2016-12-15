#ifndef ASTEROID_H
#define ASTEROID_H

#include "../GenericEntity.h"
#include "Vector3.h"
#include "MyMath.h"
//#include "MeshBuilder.h"

class Mesh;

class Asteroid : public GenericEntity
{
public:
	Asteroid(void);
	virtual ~Asteroid(void);

	void Init(void);
	void Update(double dt);
	void Render(void);
	void Split(void);

	Vector3 direction;
	
	bool mini;
	double speed;
	bool ShowAABB = false;
};

#endif