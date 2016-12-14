#include "MachineGun.h"


MachineGun::MachineGun()
{
}


MachineGun::~MachineGun()
{
}

// Initialise this instance to default values
void MachineGun::Init(void)
{
	// Call the parent's Init method
	CWeaponInfo::Init();

	// The number of ammunition in a magazine for this weapon
	magRounds = 40;
	// The maximum number of ammunition for this magazine for this weapon
	maxMagRounds = 40;
	// The current total number of rounds currently carried by this player
	totalRounds = 4000000;
	// The max total number of rounds currently carried by this player
	maxTotalRounds = 4000000;

	// The time between shots
	timeBetweenShots = 0.1333;
	// The elapsed time (between shots)
	elapsedTime = 0.0;
	// Boolean flag to indicate if weapon can fire now
	bFire = true;
}
