#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(class Actor* owner)
:Component(owner)
,mAngularSpeed(0.0f)
,mForwardSpeed(0.0f)
,mStrafeSpeed(0.0f)
{
	
}

void MoveComponent::Update(float deltaTime)
{
	if (!Math::NearZero(mAngularSpeed))
	{
		float rot = mOwner->GetRotation();
		rot += mAngularSpeed * deltaTime;
		mOwner->SetRotation(rot);
	}
	
	if (!Math::NearZero(mForwardSpeed))
	{
		Vector3 pos = mOwner->GetPosition();
		pos += mOwner->GetForward() * mForwardSpeed * deltaTime;		
		mOwner->SetPosition(pos);
	}

	if (!Math::NearZero(mStrafeSpeed))
	{
		Vector3 pos = mOwner->GetPosition();
		Vector3 right = mOwner->GetRight();
		pos += right * mStrafeSpeed * deltaTime;
		mOwner->SetPosition(pos);
	}
}
