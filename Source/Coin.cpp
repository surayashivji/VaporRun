#include "Coin.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "CollisionComponent.h"
#include "Player.h"
#include <SDL/SDL_mixer.h>
#include "HUD.h"

Coin::Coin(class Game* game)
	:Actor(game)
{
	mMesh = new MeshComponent(this);
	mMesh->SetMesh(game->GetRenderer()->GetMesh("Assets/Coin.gpmesh"));

	mCollision = new CollisionComponent(this);
	mCollision->SetSize(100.0f, 100.0f, 100.0f);
}

void Coin::UpdateActor(float deltaTime)
{
	if (mCollision->Intersect(GetGame()->mPlayer->GetCollision()))
	{
        GetGame()->mNumCoins += 1;
        GetGame()->mHUD->UdpateCoinText();
		Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/Coin.wav"), 0);
		SetState(EDead);
	}
	float rot = GetRotation();
	rot += Math::Pi * deltaTime;
	SetRotation(rot);
}
