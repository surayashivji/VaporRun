#include "CameraComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"

CameraComponent::CameraComponent(class Actor* owner)
:Component(owner)
{
	
}

void CameraComponent::Update(float deltaTime)
{
	mPitchAngle += mPitchSpeed * deltaTime;
	mPitchAngle = Math::Clamp(mPitchAngle, -Math::Pi / 4.0f, Math::Pi / 4.0f);
	// Calculate look at
	Vector3 eye = mOwner->GetPosition();

	// Calculate camera forward
	Matrix4 pitchMat = Matrix4::CreateRotationY(mPitchAngle);
	Matrix4 yawMat = Matrix4::CreateRotationZ(mOwner->GetRotation());
	Matrix4 combined = pitchMat * yawMat;

	Vector3 cameraForward = Vector3::Transform(Vector3::UnitX, combined);

	Vector3 target = mOwner->GetPosition() + cameraForward * 10.0f;
	Matrix4 lookAt = Matrix4::CreateLookAt(eye, target, Vector3::UnitZ);

	mOwner->GetGame()->GetRenderer()->SetViewMatrix(lookAt);
}
