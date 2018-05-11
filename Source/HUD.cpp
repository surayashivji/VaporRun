#include "HUD.h"
#include "Texture.h"
#include "Shader.h"
#include "Game.h"
#include "Renderer.h"
#include "Font.h"
#include <sstream>
#include <iomanip>
#include "Checkpoint.h"

HUD::HUD(Game* game)
	:mGame(game)
	,mFont(nullptr)
{
	// Load font
	mFont = new Font();
	mFont->Load("Assets/CircularStd-Black.otf");
    mTimerText = mFont->RenderText("00:00.00");
    mCoinText = mFont->RenderText("0/55");
}

HUD::~HUD()
{
	// Get rid of font
	if (mFont)
	{
		mFont->Unload();
		delete mFont;
	}
}

void HUD::Update(float deltaTime)
{
    mTime += deltaTime;
    
    if (mTimerText) {
        mTimerText->Unload();
        delete mTimerText;
    }
    
    if (mCheckpointTextTimer) {
        mCheckpointTextTimer -= deltaTime;
    }
    
    int minutes = 0;
    int seconds = 0;
    int fractional = 0;
    
    if (mTime > 0)
    {
        minutes = mTime/60;
        seconds = static_cast<int>(mTime)%60;
        fractional = static_cast<int>((mTime*100.0))%100;
    }
    
    char text[100];
    sprintf(text, "%d:%02d.%02d", minutes, seconds, fractional);
    std::string some(text);
    mTimerText = mFont->RenderText(some);
}

void HUD::UdpateCoinText()
{
    char text[100];
    sprintf(text, "%d/55", mGame->mNumCoins);
    std::string some(text);
    mCoinText = mFont->RenderText(some);
}

void HUD::UdpateCheckpointText()
{
    mCheckpointTextTimer = 5.0f;
    if (mGame->mCurrentCheckpoint) {
        std::string cpText = mGame->mCurrentCheckpoint->text;
        mCheckpointText = mFont->RenderText(cpText);
    }
}

void HUD::Draw(Shader* shader)
{
	DrawTexture(shader, mTimerText, Vector2(-420.0f, -325.0f));
    DrawTexture(shader, mCoinText, Vector2(-420.0f, -275.0f));
    
    if (mCheckpointTextTimer > 0) {
        DrawTexture(shader, mCheckpointText,  Vector2::Zero);
    }
}

void HUD::DrawTexture(class Shader* shader, class Texture* texture,
				 const Vector2& offset, float scale)
{
	// Scale the quad by the width/height of texture
	Matrix4 scaleMat = Matrix4::CreateScale(
		static_cast<float>(texture->GetWidth()) * scale,
		static_cast<float>(texture->GetHeight()) * scale,
		1.0f);
	// Translate to position on screen
	Matrix4 transMat = Matrix4::CreateTranslation(
		Vector3(offset.x, offset.y, 0.0f));	
	// Set world transform
	Matrix4 world = scaleMat * transMat;
	shader->SetMatrixUniform("uWorldTransform", world);
	// Set current texture
	texture->SetActive();
	// Draw quad
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
