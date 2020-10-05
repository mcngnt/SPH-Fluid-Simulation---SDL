#pragma once
#include <SDL.h>
#include <SDL_image.h>



class RenderWindow
{
public:
	RenderWindow(const char* p_title, int p_w, int p_h);
	SDL_Texture* LoadTexture(const char* p_FilePath);
	void CleanUp();
	void Clear();
	void Clear(int pRed, int pGreen, int pBlue);
	void RenderTex(SDL_Texture* p_tex,float p_x, float p_y, float p_sizeW, float p_sizeH);
	void Display();
	void DrawRect(double pPosLeft, double pPosTop, double pWidthWidth, double pWidthHeight, int pRed, int pGreen, int pBlue);
	SDL_Window* window;
	SDL_Renderer* renderer;
private:
};