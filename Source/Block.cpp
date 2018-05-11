#include "Block.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "CollisionComponent.h"
#include <algorithm>

Block::Block(class Game* game)
:Actor(game)
{
	mMesh = new MeshComponent(this);
	mMesh->SetMesh(game->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));
	mScale = 64.0f;

	mCollision = new CollisionComponent(this);
	mCollision->SetSize(1.0f, 1.0f, 1.0f);

	game->mBlocks.emplace_back(this);
}

Block::~Block()
{
	auto iter = std::find(GetGame()->mBlocks.begin(), GetGame()->mBlocks.end(), this);
	GetGame()->mBlocks.erase(iter);
}
