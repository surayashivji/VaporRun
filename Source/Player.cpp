#include "Player.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "CollisionComponent.h"
#include "PlayerMove.h"
#include "CameraComponent.h"

Player::Player(class Game* game)
:Actor(game)
{
	mCollision = new CollisionComponent(this);
	mCollision->SetSize(50.0f, 175.0f, 50.0f);

	mMove = new PlayerMove(this);

	mCamera = new CameraComponent(this);
}
