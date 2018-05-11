#pragma once
#include "MoveComponent.h"
#include "Math.h"

class PlayerMove : public MoveComponent
{
public:
	enum MoveState
	{
		OnGround,
		Jump,
		Falling,
		WallClimb,
		WallRun
	};

	enum CollSide
	{
		None,
		Top,
		Bottom,
		SideX1,
		SideX2,
		SideY1,
		SideY2
	};
	PlayerMove(class Actor* owner);
	~PlayerMove();
	void Update(float deltaTime) override;
	void ProcessInput(const Uint8* keyState) override;
	void ChangeState(MoveState state);
protected:
	void Respawn();
	void PhysicsUpdate(float deltaTime);
	void FixXYVelocity();
	void AddForce(const Vector3& force);
	void UpdateOnGround(float deltaTime);
	void UpdateJump(float deltaTime);
	void UpdateFalling(float deltaTime);
	void UpdateWallClimb(float deltaTime);
	bool CanWallClimb(CollSide side);
	void UpdateWallRun(float deltaTime);
	bool CanWallRun(CollSide side);
	CollSide FixCollision(class CollisionComponent* self, class CollisionComponent* block);
	Vector3 GetNormalFromSide(CollSide side);
private:
	Vector3 mVelocity;
	Vector3 mAcceleration;
	Vector3 mPendingForces;
	float mMass = 1.0f;
	MoveState mCurrentState = Falling;
	Vector3 mGravity{ 0.0f, 0.0f, -980.0f };
	Vector3 mWallClimbForce{ 0.0f, 0.0f, 1800.0f };
	Vector3 mWallRunForce{ 0.0f, 0.0f, 1200.0f };
	Vector3 mJumpForce{ 0.0f, 0.0f, 35000.0f };
	float mMaxSpeed = 400.0f;
	float mXYForce = 700.0f;
	float mBrakingFactor = 0.9f;
	float mWallClimbTimer = 0.0f;
	float mMaxWallClimbTime = 0.4f;
	float mWallRunTimer = 0.0f;
	float mMaxWallRunTime = 0.4f;
	float mKillZ = -750.0f;
	int mRunningSFX = -1;
	bool mSpacePressed = false;
};
