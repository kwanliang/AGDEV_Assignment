#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "SingletonTemplate.h"
#include <irrKlang.h>
#include <string>
using std::string;

#pragma comment(lib, "irrKlang.lib")

class SoundManager : public Singleton<SoundManager>
{
	friend Singleton<SoundManager>;

public:
	SoundManager(void);
	virtual ~SoundManager(void);

	void playMusic(string Music);
	void stopMusic();
	void playSoundEffect2D(string Music);

private:
	irrklang::ISoundEngine* musicEngine;
	irrklang::ISound* BGM;
	irrklang::ISound* SoundEffect2D;
};

#endif