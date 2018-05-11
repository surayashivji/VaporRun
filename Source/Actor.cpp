#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "MeshComponent.h"
#include "CameraComponent.h"

Actor::Actor(Game* game)
	:mGame(game)
	,mState(EActive)
	,mPosition(Vector3::Zero)
	,mScale(1.0f)
	,mRotation(0.0f)
	,mMove(nullptr)
	,mCollision(nullptr)
	,mMesh(nullptr)
	,mCamera(nullptr)
{
	mGame->AddActor(this);
}

Actor::~Actor()
{
	delete mMove;
	delete mCollision;
	delete mMesh;
	delete mCamera;
	mGame->RemoveActor(this);
}

void Actor::Update(float deltaTime)
{
	if (mState == EActive)
	{
		// Update move component
		if (mMove)
		{
			mMove->Update(deltaTime);
		}
		if (mCamera)
		{
			mCamera->Update(deltaTime);
		}
		if (mCollision)
		{
			mCollision->Update(deltaTime);
		}
		if (mMesh)
		{
			mMesh->Update(deltaTime);
		}
		UpdateActor(deltaTime);
		mWorldTransform = Matrix4::CreateScale(mScale) *
			Matrix4::CreateRotationZ(mRotation) *
			Matrix4::CreateFromQuaternion(mQuat) *
			Matrix4::CreateTranslation(mPosition);
	}
}

void Actor::UpdateActor(float deltaTime)
{
}

void Actor::ProcessInput(const Uint8* keyState)
{
	if (mState == EActive)
	{
		// Process input for move component
		if (mMove)
		{
			mMove->ProcessInput(keyState);
		}
		if (mCamera)
		{
			mCamera->ProcessInput(keyState);
		}
		if (mCollision)
		{
			mCollision->ProcessInput(keyState);
		}
		if (mMesh)
		{
			mMesh->ProcessInput(keyState);
		}
		// Process actor input
		ActorInput(keyState);
	}
}

void Actor::ActorInput(const Uint8* keyState)
{
	
}
