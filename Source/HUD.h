#pragma once
#include "Math.h"
#include <string>

class HUD
{
public:
	HUD(class Game* game);
	~HUD();
	// UIScreen subclasses can override these
	virtual void Update(float deltaTime);
	virtual void Draw(class Shader* shader);
    void UdpateCoinText();
    void UdpateCheckpointText();
    float mCheckpointTextTimer;
    
protected:
	// Helper to draw a texture
	void DrawTexture(class Shader* shader, class Texture* texture,
					 const Vector2& offset = Vector2::Zero,
					 float scale = 1.0f);
	class Game* mGame;
	
	class Font* mFont;
    class Texture* mTimerText;
    class Texture* mCoinText;
    class Texture* mCheckpointText;
    float mTime;
};
