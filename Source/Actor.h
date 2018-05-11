#pragma once
#include <vector>
#include <SDL/SDL_stdinc.h>
#include "Math.h"
class Actor
{
public:
	enum State
	{
		EActive,
		EPaused,
		EDead
	};
	
	Actor(class Game* game);
	virtual ~Actor();

	// Update function called from Game (not overridable)
	void Update(float deltaTime);
	// Any actor-specific update code (overridable)
	virtual void UpdateActor(float deltaTime);
	// ProcessInput function called from Game (not overridable)
	void ProcessInput(const Uint8* keyState);
	// Any actor-specific update code (overridable)
	virtual void ActorInput(const Uint8* keyState);

	// Getters/setters
	const Vector3& GetPosition() const { return mPosition; }
	void SetPosition(const Vector3& pos) { mPosition = pos; }
	float GetScale() const { return mScale; }
	void SetScale(float scale) { mScale = scale; }
	float GetRotation() const { return mRotation; }
	void SetRotation(float rotation) { mRotation = rotation; }
	const Quaternion& GetQuat() const { return mQuat; }
	void SetQuat(const Quaternion& quat) { mQuat = quat; }

	Vector3 GetForward() const { return Vector3(Math::Cos(mRotation), Math::Sin(mRotation), 0.0f); }
	Vector3 GetRight() const
	{
		float angle = mRotation + Math::PiOver2;
		return Vector3(Math::Cos(angle), Math::Sin(angle), 0.0f);
	}
	
	State GetState() const { return mState; }
	void SetState(State state) { mState = state; }

	class Game* GetGame() { return mGame; }

	class MoveComponent* GetMove() { return mMove; }
	class CollisionComponent* GetCollision() { return mCollision; }
	class MeshComponent* GetMesh() { return mMesh; }
	class CameraComponent* GetCamera() { return mCamera; }

	const Matrix4& GetWorldTransform() const { return mWorldTransform; }
protected:
	class Game* mGame;
	// Actor's state
	State mState;

	// Transform
	Matrix4 mWorldTransform;
	Vector3 mPosition;
	Quaternion mQuat;
	float mScale;
	float mRotation;
	
	// Components
	class MoveComponent* mMove;
	class CollisionComponent* mCollision;
	class MeshComponent* mMesh;
	class CameraComponent* mCamera;
};
