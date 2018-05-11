#include "PlayerMove.h"
#include "Actor.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "Block.h"
#include <SDL/SDL.h>
#include "CameraComponent.h"
#include "Player.h"
#include "Game.h"
#include <SDL/SDL_mixer.h>

PlayerMove::PlayerMove(class Actor* owner)
:MoveComponent(owner)
{
	ChangeState(Falling);
	mRunningSFX = Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Running.wav"), -1);
	Mix_Pause(mRunningSFX);
}

PlayerMove::~PlayerMove()
{
	Mix_HaltChannel(mRunningSFX);
}

void PlayerMove::Update(float deltaTime)
{
	if (mOwner->GetPosition().z <= mKillZ)
	{
		Respawn();
	}

	switch (mCurrentState)
	{
	case OnGround:
		UpdateOnGround(deltaTime);
		break;
	case Jump:
		UpdateJump(deltaTime);
		break;
	case Falling:
		UpdateFalling(deltaTime);
		break;
	case WallClimb:
		UpdateWallClimb(deltaTime);
		break;
	case WallRun:
		UpdateWallRun(deltaTime);
		break;
	default:
		break;
	}

	if ((mCurrentState == OnGround && mVelocity.Length() > 50.0f) ||
		mCurrentState == WallRun || mCurrentState == WallClimb)
	{
		Mix_Resume(mRunningSFX);
	}
	else
	{
		Mix_Pause(mRunningSFX);
	}
}

void PlayerMove::ProcessInput(const Uint8* keyState)
{
	// Update forward force
	Vector3 forward = mOwner->GetForward();
	if (keyState[SDL_SCANCODE_W])
	{
		AddForce(forward * mXYForce);
	}
	if (keyState[SDL_SCANCODE_S])
	{
		AddForce(forward * -mXYForce);
	}

	// Update strafe speed
	Vector3 right = mOwner->GetRight();
	if (keyState[SDL_SCANCODE_D])
	{
		AddForce(right * mXYForce);
	}
	if (keyState[SDL_SCANCODE_A])
	{
		AddForce(right * -mXYForce);
	}

	int x, y;
	SDL_GetRelativeMouseState(&x, &y);
	float angularSpd = (x / 500.0f) * Math::Pi * 10.0f;
	SetAngularSpeed(angularSpd);
	float pitchSpd = (y / 500.0f) * Math::Pi * 10.0f;
	mOwner->GetCamera()->SetPitchSpeed(pitchSpd);

	// Jumping
	if (!mSpacePressed && keyState[SDL_SCANCODE_SPACE] && mCurrentState == OnGround)
	{
		AddForce(mJumpForce);
		ChangeState(Jump);
		Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Jump.wav"), 0);
	}

	mSpacePressed = keyState[SDL_SCANCODE_SPACE];
}

void PlayerMove::ChangeState(MoveState state)
{
	if (mCurrentState == Falling && state == OnGround)
	{
		Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Land.wav"), 0);
	}
	mCurrentState = state;
}

void PlayerMove::Respawn()
{
	ChangeState(Falling);
	mVelocity = Vector3::Zero;
	mPendingForces = Vector3::Zero;
	Player* me = static_cast<Player*>(mOwner);
	me->SetPosition(me->GetRespawnPos());
	me->SetRotation(0.0f);
}

void PlayerMove::PhysicsUpdate(float deltaTime)
{
	mAcceleration = mPendingForces * (1.0f / mMass);
	mVelocity += mAcceleration * deltaTime;

	FixXYVelocity();

	Vector3 pos = mOwner->GetPosition();
	pos += mVelocity * deltaTime;
	mOwner->SetPosition(pos);

	// Rotate still
	if (!Math::NearZero(GetAngularSpeed()))
	{
		float rot = mOwner->GetRotation();
		rot += GetAngularSpeed() * deltaTime;
		mOwner->SetRotation(rot);
	}

	// Clear pending forces
	mPendingForces = Vector3::Zero;
}

void PlayerMove::FixXYVelocity()
{
	// Make sure we can't exceed max speed
	Vector2 xyVelocity(mVelocity.x, mVelocity.y);
	if (xyVelocity.Length() > mMaxSpeed)
	{
		xyVelocity.Normalize();
		xyVelocity *= mMaxSpeed;
	}

	// If we're on ground, check if we need to apply braking
	if (mCurrentState == OnGround || mCurrentState == WallClimb)
	{
		if (Math::NearZero(mAcceleration.x) || (mAcceleration.x * xyVelocity.x) < 0.0f)
		{
			xyVelocity.x *= mBrakingFactor;
		}
		if (Math::NearZero(mAcceleration.y) || (mAcceleration.y * xyVelocity.y) < 0.0f)
		{
			xyVelocity.y *= mBrakingFactor;
		}
	}

	mVelocity.x = xyVelocity.x;
	mVelocity.y = xyVelocity.y;
}

void PlayerMove::AddForce(const Vector3& force)
{
	mPendingForces += force;
}

void PlayerMove::UpdateOnGround(float deltaTime)
{
	PhysicsUpdate(deltaTime);

	bool onBlock = false;
	for (Block* block : mOwner->GetGame()->mBlocks)
	{
		CollSide side = FixCollision(mOwner->GetCollision(), block->GetCollision());
		if (side == Top)
		{
			onBlock = true;
		}
		else if (side >= SideX1)
		{
			if (CanWallClimb(side))
			{
				ChangeState(WallClimb);
				mWallClimbTimer = 0.0f;
				return;
			}
		}
	}

	if (!onBlock)
	{
		ChangeState(Falling);
	}
}

void PlayerMove::UpdateJump(float deltaTime)
{
	AddForce(mGravity);
	PhysicsUpdate(deltaTime);

	for (Block* block : mOwner->GetGame()->mBlocks)
	{
		CollSide side = FixCollision(mOwner->GetCollision(), block->GetCollision());
		if (side == Bottom)
		{
			mVelocity.z = 0.0f;
		}
		else if (side >= SideX1)
		{
			if (CanWallClimb(side))
			{
				ChangeState(WallClimb);
				mWallClimbTimer = 0.0f;
				return;
			}
			else if (CanWallRun(side))
			{
				ChangeState(WallRun);
				mWallRunTimer = 0.0f;
				return;
			}
		}
	}

	if (mVelocity.z <= 0.0f)
	{
		ChangeState(Falling);
	}
}

void PlayerMove::UpdateFalling(float deltaTime)
{
	AddForce(mGravity);
	PhysicsUpdate(deltaTime);

	for (Block* block : mOwner->GetGame()->mBlocks)
	{
		CollSide side = FixCollision(mOwner->GetCollision(), block->GetCollision());
		if (side == Top)
		{
			mVelocity.z = 0.0f;
			ChangeState(OnGround);
		}
	}
}

void PlayerMove::UpdateWallClimb(float deltaTime)
{
	mWallClimbTimer += deltaTime;
	AddForce(mGravity);
	if (mWallClimbTimer <= mMaxWallClimbTime)
	{
		AddForce(mWallClimbForce);
	}
	PhysicsUpdate(deltaTime);

	bool collided = false;
	for (Block* block : mOwner->GetGame()->mBlocks)
	{
		CollSide side = FixCollision(mOwner->GetCollision(), block->GetCollision());
		if (side >= SideX1)
		{
			collided = true;
		}
	}

	// Are we falling?
	if (!collided || mVelocity.z <= 0.0f)
	{
		mVelocity.z = 0.0f;
		ChangeState(Falling);
	}
}

bool PlayerMove::CanWallClimb(CollSide side)
{
	bool retVal = false;

	// I can wall climb if:
	// I'm facing block
	// I'm moving in the direction of block
	// My xy velocity is high enough
	Vector3 sideVec = GetNormalFromSide(side);

	Vector3 velocityDir = Vector3(mVelocity.x, mVelocity.y, 0.0f);
	velocityDir.Normalize();
	Vector2 xyVelocity(mVelocity.x, mVelocity.y);
	float dotFwd = Vector3::Dot(sideVec, mOwner->GetForward());
	float dotVel = Vector3::Dot(sideVec, velocityDir);
	if (dotFwd < -0.9f &&
		dotVel < -0.9f &&
		xyVelocity.Length() > 350.0f)
	{
		retVal = true;
	}

	return retVal;
}

void PlayerMove::UpdateWallRun(float deltaTime)
{
	mWallRunTimer += deltaTime;
	AddForce(mGravity);
	if (mWallRunTimer <= mMaxWallRunTime)
	{
		AddForce(mWallRunForce);
	}
	PhysicsUpdate(deltaTime);

	for (Block* block : mOwner->GetGame()->mBlocks)
	{
		CollSide side = FixCollision(mOwner->GetCollision(), block->GetCollision());
	}

	// Are we falling?
	if (mVelocity.z <= 0.0f)
	{
		mVelocity.z = 0.0f;
		ChangeState(Falling);
	}
}

bool PlayerMove::CanWallRun(CollSide side)
{
	bool retVal = false;

	// I can wall run if:
	// I'm perpendicular to wall
	// My velocity is roughly forward
	// My xy velocity is high enough

	Vector3 sideVec = GetNormalFromSide(side);
	
	Vector3 velocityDir = Vector3(mVelocity.x, mVelocity.y, 0.0f);
	velocityDir.Normalize();
	Vector2 xyVelocity(mVelocity.x, mVelocity.y);
	float dotSide = Vector3::Dot(sideVec, mOwner->GetRight());
	float dotFwd = Vector3::Dot(mOwner->GetForward(), velocityDir);
	if (Math::Abs(dotSide) > 0.7f &&
		dotFwd > 0.9f &&
		xyVelocity.Length() > 350.0f)
	{
		retVal = true;
	}

	return retVal;
}

PlayerMove::CollSide PlayerMove::FixCollision(CollisionComponent* self, CollisionComponent* block)
{
	CollSide retVal = None;

	if (self->Intersect(block))
	{
		Vector3 pos = mOwner->GetPosition();
		// Get player min/max and block min/max
		Vector3 playerMin = self->GetMin();
		Vector3 playerMax = self->GetMax();
		Vector3 blockMin = block->GetMin();
		Vector3 blockMax = block->GetMax();

		// Figure out which side we are closest to
		float dx1 = blockMin.x - playerMax.x;
		float dx2 = blockMax.x - playerMin.x;
		float dy1 = blockMin.y - playerMax.y;
		float dy2 = blockMax.y - playerMin.y;
		float dz1 = blockMin.z - playerMax.z;
		float dz2 = blockMax.z - playerMin.z;

		float dx = 0.0f;
		if (Math::Abs(dx1) < Math::Abs(dx2))
		{
			dx = dx1;
		}
		else
		{
			dx = dx2;
		}

		float dy = 0.0f;
		if (Math::Abs(dy1) < Math::Abs(dy2))
		{
			dy = dy1;
		}
		else
		{
			dy = dy2;
		}

		float dz = 0.0f;
		if (Math::Abs(dz1) < Math::Abs(dz2))
		{
			dz = dz1;
		}
		else
		{
			dz = dz2;
		}

		// Figure out which side is closest
		if (Math::Abs(dy) < Math::Abs(dx) && Math::Abs(dy) < Math::Abs(dz))
		{
			pos.y += dy;
			// Add a normal force
			if (dy == dy1)
			{
				AddForce(Vector3(0.0f, -700.0f, 0.0f));
				retVal = SideY1;
			}
			else
			{
				AddForce(Vector3(0.0f, 700.0f, 0.0f));
				retVal = SideY2;
			}
		}
		else if (Math::Abs(dx) < Math::Abs(dy) && Math::Abs(dx) < Math::Abs(dz))
		{
			pos.x += dx;
			// Add a normal force
			if (dx == dx1)
			{
				AddForce(Vector3(-700.0f, 0.0f, 0.0f));
				retVal = SideX1;
			}
			else
			{
				AddForce(Vector3(700.0f, 0.0f, 0.0f));
				retVal = SideX2;
			}
		}
		else
		{
			pos.z += dz;
			if (dz >= 0.0f)
			{
				retVal = Top;
			}
			else
			{
				retVal = Bottom;
			}
		}

		mOwner->SetPosition(pos);
	}

	return retVal;
}

Vector3 PlayerMove::GetNormalFromSide(CollSide side)
{
	if (side == SideX1)
	{
		return Vector3::NegUnitX;
	}
	else if (side == SideX2)
	{
		return Vector3::UnitX;
	}
	else if (side == SideY1)
	{
		return Vector3::NegUnitY;
	}
	else if (side == SideY2)
	{
		return Vector3::UnitY;
	}
	else if (side == Top)
	{
		return Vector3::UnitZ;
	}
	else
	{
		return Vector3::NegUnitZ;
	}
}
