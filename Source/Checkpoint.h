#pragma once
#include "Actor.h"
#include <string>
class Checkpoint : public Actor
{
public:
	Checkpoint(class Game* game);
	void UpdateActor(float deltaTime);
	void Activate();
	void SetLevelString(const std::string& str) { mLevelString = str; }
    std::string text;
private:
	bool mIsActive = false;
	std::string mLevelString;
};
