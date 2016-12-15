#ifndef BASE_H
#define BASE_H

#include "../GenericEntity.h"
#include "Vector3.h"
#include "MyMath.h"

class Mesh;

class Base : public GenericEntity
{
private:
	Vector3 direction;
    int m_HP;

public:
	Base(void);
	virtual ~Base(void);

	void Init(void);
	void Update(double dt);
	void Render(void);

    // HP
    void SetHP(int m_HP);
    int GetHP(void);
};

#endif