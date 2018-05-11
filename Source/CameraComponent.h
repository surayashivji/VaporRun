#pragma once
#include "Component.h"

class CameraComponent : public Component
{
public:
	CameraComponent(class Actor* owner);
	void Update(float deltaTime) override;

	void SetPitchSpeed(float speed) { mPitchSpeed = speed; }
	float GetPitchSpeed() const { return mPitchSpeed; }
private:
	float mPitchAngle = 0.0f;
	float mPitchSpeed = 0.0f;
};
