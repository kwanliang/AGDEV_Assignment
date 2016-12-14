#ifndef MACHINEGUN_H
#define MACHINEGUN_H

#include "WeaponInfo.h"

class MachineGun :
	public CWeaponInfo
{
public:
	MachineGun();
	virtual ~MachineGun();

	// Initialise this instance to default values
	void Init(void);
};

#endif