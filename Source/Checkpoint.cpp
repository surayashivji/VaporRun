#include "Checkpoint.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "CollisionComponent.h"
#include "Player.h"
#include "HUD.h"

Checkpoint::Checkpoint(class Game* game)
	:Actor(game)
{
	mMesh = new MeshComponent(this);
	mMesh->SetMesh(game->GetRenderer()->GetMesh("Assets/Checkpoint.gpmesh"));
	mMesh->SetTextureIndex(1);

	mCollision = new CollisionComponent(this);
	mCollision->SetSize(25.0f, 25.0f, 25.0f);
}

void Checkpoint::UpdateActor(float deltaTime)
{
	if (mIsActive && mCollision->Intersect(GetGame()->mPlayer->GetCollision()))
	{
		Vector3 respawnPos = GetPosition();
		GetGame()->mPlayer->SetRespawnPos(respawnPos);
		SetState(EDead);
		mIsActive = false;
		GetGame()->AdvanceCheckpoint();
        GetGame()->mHUD->UdpateCheckpointText();
		Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/Checkpoint.wav"), 0);
		if (!mLevelString.empty())
		{
			GetGame()->mNextLevel = mLevelString;
		}
	}
}

void Checkpoint::Activate()
{
	mMesh->SetTextureIndex(0);
	mIsActive = true;
}
