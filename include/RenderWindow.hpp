#pragma once
#include <SDL.h>
#include <SDL_image.h>

#include "Vector2D.hpp"


class RenderWindow
{
public:
	RenderWindow(const char* p_title, int p_w, int p_h);
	SDL_Texture* LoadTexture(const char* p_FilePath);
	void CleanUp();
	void Clear();
	void RenderTex(SDL_Texture* p_tex,float p_x, float p_y, float p_sizeW, float p_sizeH);
	void Display();
	void DrawRect(Vector2D pPos, Vector2D pWidth, Vector2D color);
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
};