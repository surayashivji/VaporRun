#include "Arrow.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "Player.h"
#include "Checkpoint.h"

Arrow::Arrow(class Game* game)
	:Actor(game)
{
	mMesh = new MeshComponent(this);
	mMesh->SetMesh(game->GetRenderer()->GetMesh("Assets/Arrow.gpmesh"));

	SetScale(0.15f);
}

void Arrow::UpdateActor(float deltaTime)
{
	// Update rotation of arrow
	Checkpoint* check = GetGame()->mCurrentCheckpoint;
	Player* player = GetGame()->mPlayer;
	if (check != nullptr)
	{
		Vector3 dir = check->GetPosition() - player->GetPosition();
		dir.Normalize();
		float angle = Math::Acos(Vector3::Dot(Vector3::UnitX, dir));
		if (!Math::NearZero(angle))
		{
			Vector3 axis = Vector3::Cross(Vector3::UnitX, dir);
			axis.Normalize();
			SetQuat(Quaternion(axis, angle));
		}
		else
		{
			SetQuat(Quaternion::Identity);
		}
	}
	else
	{
		SetQuat(Quaternion::Identity);
	}

	// Update position
	Vector3 pos = GetGame()->GetRenderer()->Unproject(Vector3(0.0f, 250.0f, 0.1f));
	SetPosition(pos);
}
