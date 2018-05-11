#include "Game.h"
#include <algorithm>
#include "Actor.h"
#include <fstream>
#include "Renderer.h"
#include "Block.h"
#include "LevelLoader.h"
#include "Checkpoint.h"
#include "Arrow.h"
#include <SDL/SDL_ttf.h>
#include "HUD.h"

Game::Game()
:mIsRunning(true)
{
	
}

bool Game::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mRenderer = new Renderer(this);
	if (!mRenderer->Initialize(1024.0f, 768.0f))
	{
		SDL_Log("Failed to start renderer");
		return false;
	}

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    
    TTF_Init();

	LoadData();

	mTicksCount = SDL_GetTicks();

	SDL_SetRelativeMouseMode(SDL_TRUE); // Enable relative mouse mode
	SDL_GetRelativeMouseState(nullptr, nullptr); // Clear any saved values
	
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
		if (!mNextLevel.empty())
		{
			LoadNextLevel();
		}
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	for (auto actor : mActors)
	{
		actor->ProcessInput(state);
	}
}

void Game::UpdateGame()
{
    // Compute delta time
    Uint32  tickNow = SDL_GetTicks();
    // Wait until 16ms has elapsed since last frame
    while (tickNow - mTicksCount < 16)
    {
        tickNow = SDL_GetTicks();
    }
    
    // Get deltaTime in seconds
    float deltaTime = (tickNow - mTicksCount) / 1000.0f;
    // Don't let deltaTime be greater than 0.05f (50 ms)
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }
    mTicksCount = tickNow;

    // Make copy of actor vector
	// (iterate over this in case new actors are created)
	std::vector<Actor*> copy = mActors;
	// Update all actors
	for (auto actor : copy)
	{
		actor->Update(deltaTime);
	}

	// Add any dead actors to a temp vector
	std::vector<Actor*> deadActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == Actor::EDead)
		{
			deadActors.emplace_back(actor);
		}
	}

	// Delete any of the dead actors (which will
	// remove them from mActors)
	for (auto actor : deadActors)
	{
		delete actor;
	}
    mHUD->Update(deltaTime);
}

void Game::GenerateOutput()
{
	mRenderer->Draw();
}

void Game::LoadData()
{
	LoadSound("Assets/Sounds/Coin.wav");
	LoadSound("Assets/Sounds/Checkpoint.wav");
	LoadSound("Assets/Sounds/Running.wav");
	LoadSound("Assets/Sounds/Jump.wav");
	LoadSound("Assets/Sounds/Land.wav");
	LoadSound("Assets/Sounds/Music.ogg");
	Matrix4 proj = Matrix4::CreatePerspectiveFOV(1.22f,
		1024.0f, 768.0f, 10.0f, 10000.0f);
	Matrix4 view = Matrix4::CreateLookAt(Vector3::Zero, Vector3::UnitX, Vector3::UnitZ);
	mRenderer->SetProjectionMatrix(proj);
	mRenderer->SetViewMatrix(view);
	//LoadLevel("Assets/Level.txt");
	LevelLoader::Load(this, "Assets/Tutorial.json");
	AdvanceCheckpoint();
	new Arrow(this);
    mHUD = new HUD(this);
	Mix_PlayChannel(-1, GetSound("Assets/Sounds/Music.ogg"), -1);
}

void Game::LoadLevel(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("Failed to load level: %s", fileName.c_str());
	}
	
	const float topLeftX = -512.0f + 32.0f;
	const float topLeftY = -384.0f + 32.0f;
	size_t row = 0;
	std::string line;
	while (!file.eof())
	{
		std::getline(file, line);
		for (size_t col = 0; col < line.size(); col++)
		{
			// Calculate position at this row/column
			Vector3 pos;
			pos.x = topLeftX + 64.0f * col;
			pos.y = topLeftY + 64.0f * row;
		
			if (line[col] == 'B')
			{
				Block* b = new Block(this);
				b->SetPosition(pos);
			}
		}
		row++;
	}
}

void Game::LoadNextLevel()
{
	// Delete all actors
	while (!mActors.empty())
	{
		delete mActors.back();
	}
	
	while (!mCheckpoints.empty())
	{
		mCheckpoints.pop();
	}

	// Load the next level
	LevelLoader::Load(this, mNextLevel);
	AdvanceCheckpoint();
	new Arrow(this);

	mNextLevel.clear();
}

void Game::AdvanceCheckpoint()
{
	if (!mCheckpoints.empty())
	{
		mCurrentCheckpoint = mCheckpoints.front();
		mCheckpoints.pop();
		mCurrentCheckpoint->Activate();
	}
	else
	{
		mCurrentCheckpoint = nullptr;
	}
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	// Destroy textures
	for (auto i : mTextures)
	{
		SDL_DestroyTexture(i.second);
	}
	mTextures.clear();

	// Destroy sounds
	for (auto s : mSounds)
	{
		Mix_FreeChunk(s.second);
	}
	mSounds.clear();
}

void Game::LoadSound(const std::string& fileName)
{
	Mix_Chunk* chunk = Mix_LoadWAV(fileName.c_str());
	if (!chunk)
	{
		SDL_Log("Failed to load sound file %s", fileName.c_str());
		return;
	}

	mSounds.emplace(fileName, chunk);
}

Mix_Chunk* Game::GetSound(const std::string& fileName)
{
	Mix_Chunk* chunk = nullptr;
	auto iter = mSounds.find(fileName);
	if (iter != mSounds.end())
	{
		chunk = iter->second;
	}
	return chunk;
}

void Game::Shutdown()
{
	UnloadData();
	Mix_CloseAudio();
	mRenderer->Shutdown();
	delete mRenderer;
	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor* actor)
{
	auto iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		auto iter2 = mActors.end() - 1;
		std::iter_swap(iter, iter2);
		mActors.pop_back();
	}
}
