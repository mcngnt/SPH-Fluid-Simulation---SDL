#include <iostream>
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <cmath>

#include "RenderWindow.hpp"

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGTH 720
#define TIME_STEP 20
#define FLUID_DRAW_FACTOR 2

struct Particule {
	double xPos;
	double yPos;
	double density;
	int wallflag;
	double xForce;
	double yForce;
	double xVelocity;
	double yVelocity;
};

int	x, y, totalOfParticules;
double xParticuleDistance, yParticuleDistance;
double particulesInteraction;
double particulesDistance;
double xScreenPos, yScreenPos;
double xParticuleDrawSize, yParticuleDrawSize;
int drawRed, drawGreen, drawBlue;
double particuleDrawGradient;


double zoomFactor = 4;
double gravity = .3, pressure = 4, viscosity = 7;


std::vector<Particule> particules(0);

void console(const char* pMessage)
{
	std::cout << "CONSOLE SAYING -> " << pMessage << std::endl;
}

void saveScreenshot(SDL_Renderer* pRenderer, long pFramesNumber)
{
	SDL_Surface *sshot = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGTH, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(pRenderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);

	std::string s = "images/";
	s.append(std::to_string(pFramesNumber));
	s.append(".bmp");
	char const *fileName = s.c_str();

	if (SDL_SaveBMP(sshot, fileName) != 0)
	{
		printf("SDL_SaveBMP failed: %s\n", SDL_GetError());
	}
	SDL_FreeSurface(sshot);

	console("Screenshot saved in images folder");
}

int main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) > 0)
	{
		std::cout << "SDL_INIT has failed. Error -> " << SDL_GetError() << std::endl;
	}
	if (!IMG_Init(IMG_INIT_PNG))
	{
		std::cout << "IMG_INIT has failed. Error -> " << SDL_GetError() << std::endl;
	}


	RenderWindow window("Game", SCREEN_WIDTH, SCREEN_HEIGTH);

	bool gameRunning = true;
	SDL_Event event;
	int mousePosX;
	int mousePosY;
	double currentTimeStep = 0;
	bool isSimRunning = false;
	int currentType = 0;
	bool canOverlapParts = false;
	bool isPlayingOneStep = false;
	long frameNumber = 0;
	bool screenshotMode = false;
	bool hasSimThisFrame = false;
	int pencilSize = 1;


	while (gameRunning)
	{
		SDL_GetMouseState(&mousePosX,&mousePosY);

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				gameRunning = false;
			}
			if (event.button.button == SDL_BUTTON_RIGHT or event.button.button == SDL_BUTTON_MIDDLE)
			{
				int xPos = floor(mousePosX/zoomFactor);
				int yPos = floor(mousePosY/zoomFactor);

				for (int i = 0; i < totalOfParticules; ++i)
				{
					xParticuleDistance = xPos - particules[i].xPos;
					yParticuleDistance = yPos - particules[i].yPos;
					particulesDistance = sqrt( pow(xParticuleDistance,2.0) + pow(yParticuleDistance,2.0));
					particulesInteraction = particulesDistance / 2.0 - 1.0;
	
					if (particulesDistance < 5){
						particules.erase(particules.begin() + i);
						totalOfParticules--;
					}
				}
			}
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				int xPos = floor(mousePosX/zoomFactor);
				int yPos = floor(mousePosY/zoomFactor);

				for (int i = 0; i < pencilSize; ++i)
				{
					bool isCreatePart = true;
							
					for (int i = 0; i < totalOfParticules; ++i)
					{
						xParticuleDistance = xPos - particules[i].xPos;
						yParticuleDistance = yPos - particules[i].yPos;
						particulesDistance = sqrt( pow(xParticuleDistance,2.0) + pow(yParticuleDistance,2.0));
				
						if (particulesDistance < 1 and !canOverlapParts){
							isCreatePart = false;
							break;
						}
					}
	
					if (isCreatePart)
					{
						Particule part;
						particules.push_back(part);
						particules[totalOfParticules].xPos = xPos;
						particules[totalOfParticules].yPos = yPos;
						particules[totalOfParticules].wallflag = currentType;
						totalOfParticules++;
					}

					if (i == 0)
					{
						xPos = floor(mousePosX/zoomFactor) + 1;
						yPos = floor(mousePosY/zoomFactor);
					}
					if (i == 1)
					{
						xPos = floor(mousePosX/zoomFactor) - 1;
						yPos = floor(mousePosY/zoomFactor);
					}
					if (i == 2)
					{
						xPos = floor(mousePosX/zoomFactor);
						yPos = floor(mousePosY/zoomFactor) + 1;
					}
					if (i == 3)
					{
						xPos = floor(mousePosX/zoomFactor);
						yPos = floor(mousePosY/zoomFactor) - 1;
					}
				}
			}
			if(event.type == SDL_MOUSEWHEEL)
    		{
    		    if(event.wheel.y > 0)
    		    {
    		         zoomFactor+=.5;
    		    }
    		    else if(event.wheel.y < 0)
    		    {
    		         zoomFactor-=.5;
    		         if (zoomFactor < 1)
    		         {
    		         	zoomFactor = 1;
    		         }
    		    }
    		}
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_SPACE:
					{
						isSimRunning = !isSimRunning;
						if (isSimRunning)
						{
							console("Sim continue");
						}
						else
						{
							console("Sim pause");
						}
						break;
					}
					case SDLK_x:
					{
						currentType = !currentType;
						console("Change particule's type");
						break;
					}
					case SDLK_s:
					{
						canOverlapParts = !canOverlapParts;
						if (canOverlapParts)
						{
							console("Particule placement authorize overlap");
						}
						else
						{
							console("Particule placement don't authorize overlap");
						}
						break;
					}
					case SDLK_RIGHT:
					{
						if (!isSimRunning)
						{
							isPlayingOneStep = true;
							console("Sim plays one step");
						}
						break;
					}
					case SDLK_t:
					{
						saveScreenshot(window.renderer, frameNumber);
						break;
					}
					case SDLK_f:
					{
						screenshotMode = !screenshotMode;
						if (screenshotMode)
						{
							console("Screenshot mode enable");
						}
						else
						{
							console("Screenshot mode disable");
						}
						break;
					}
					case SDLK_i:
					{
						std::string s = "There is ";
						s.append(std::to_string(totalOfParticules));
						s.append(" particules");
						char const *message = s.c_str();
						console(message);
						break;
					}
					case SDLK_p:
					{
						particules.resize(0);
						totalOfParticules = 0;
						break;
					}
					case SDLK_h:
					{
						pencilSize *= 5;
						if (pencilSize > 5)
						{
							pencilSize = 1;
						}
						std::string s = "Pencil size changed to ";
						s.append(std::to_string(pencilSize));
						char const *message = s.c_str();
						console(message);
						break;
					}
				}
			}
		}

		window.Clear(0,10,15);
		if (SDL_GetTicks() - currentTimeStep > TIME_STEP or isPlayingOneStep)
		{

			currentTimeStep = SDL_GetTicks();

			if (!isSimRunning and !isPlayingOneStep)
			{
				goto endSim;
			}
			isPlayingOneStep = false;
			frameNumber++;
			hasSimThisFrame = true;


			int particulesCursor, particulesCursor2;

			for (particulesCursor = 0; particulesCursor < totalOfParticules; particulesCursor++){
				particules[particulesCursor].density = particules[particulesCursor].wallflag * 9;
	
					for (particulesCursor2 = 0; particulesCursor2 < totalOfParticules; particulesCursor2++){
	
						xParticuleDistance = particules[particulesCursor].xPos - particules[particulesCursor2].xPos;
						yParticuleDistance = particules[particulesCursor].yPos - particules[particulesCursor2].yPos;
						particulesDistance = xParticuleDistance*xParticuleDistance + yParticuleDistance*yParticuleDistance;
	
						if (particulesDistance < 4){
							particulesDistance = sqrt(particulesDistance);
							particulesInteraction = particulesDistance / 2.0 - 1.0;
							particules[particulesCursor].density += particulesInteraction * particulesInteraction;
						}

					}
				}
	
				for (particulesCursor = 0; particulesCursor < totalOfParticules; particulesCursor++){
					particules[particulesCursor].yForce = gravity;
					particules[particulesCursor].xForce = 0;
	
					for (particulesCursor2 = 0; particulesCursor2 < totalOfParticules; particulesCursor2++){
	
						xParticuleDistance = particules[particulesCursor].xPos - particules[particulesCursor2].xPos;
						yParticuleDistance = particules[particulesCursor].yPos - particules[particulesCursor2].yPos;
						particulesDistance = xParticuleDistance*xParticuleDistance + yParticuleDistance*yParticuleDistance;
	
						if (particulesDistance < 4){
							particulesDistance = sqrt(particulesDistance);
							particulesInteraction = particulesDistance / 2.0 - 1.0;
							particules[particulesCursor].xForce += particulesInteraction * (xParticuleDistance * (3 - particules[particulesCursor].density - particules[particulesCursor2].density) * pressure + particules[particulesCursor].xVelocity * viscosity - particules[particulesCursor2].xVelocity * viscosity) / particules[particulesCursor].density;
							particules[particulesCursor].yForce += particulesInteraction * (yParticuleDistance * (3 - particules[particulesCursor].density - particules[particulesCursor2].density) * pressure + particules[particulesCursor].yVelocity * viscosity - particules[particulesCursor2].yVelocity * viscosity) / particules[particulesCursor].density;
						}
					}
				}
	
				for (particulesCursor = 0; particulesCursor < totalOfParticules; particulesCursor++) {
	
					if (!particules[particulesCursor].wallflag) {
						
						if (particules[particulesCursor].xForce*particules[particulesCursor].xForce + particules[particulesCursor].yForce*particules[particulesCursor].yForce < 4.2*4.2) {
							particules[particulesCursor].xVelocity += particules[particulesCursor].xForce / 10;
							particules[particulesCursor].yVelocity += particules[particulesCursor].yForce / 10;
						}
						else {
							particules[particulesCursor].xVelocity += particules[particulesCursor].xForce / 11;
							particules[particulesCursor].yVelocity += particules[particulesCursor].yForce / 11;
						}
	
						particules[particulesCursor].xPos += particules[particulesCursor].xVelocity;
						particules[particulesCursor].yPos += particules[particulesCursor].yVelocity;
	
				}
			}
		}
		endSim:
		
		for (int i = 0; i < totalOfParticules; ++i)
		{
			xScreenPos = particules[i].xPos * zoomFactor;
			yScreenPos = particules[i].yPos * zoomFactor;

			if (yScreenPos > SCREEN_HEIGTH)
			{
				particules.erase(particules.begin()+i);
				totalOfParticules--;
			}
			xParticuleDrawSize = zoomFactor;
			yParticuleDrawSize = zoomFactor;

			drawRed = 0;
			drawGreen = 0;
			drawBlue = 255;
			if (particules[i].wallflag == 1)
			{
				drawRed = 255;
				drawBlue = 0;
			}
			else
			{
				xParticuleDrawSize *= FLUID_DRAW_FACTOR;
				yParticuleDrawSize *= FLUID_DRAW_FACTOR;
				particuleDrawGradient = particules[i].density / 5;
				//particuleDrawGradient = (abs(particules[i].xVelocity) + abs(particules[i].yVelocity))/5;
				if (particuleDrawGradient > 1)
				{
					particuleDrawGradient = 1;
				}
				drawRed = 255 * particuleDrawGradient;

			}

			window.DrawRect(xScreenPos, yScreenPos, xParticuleDrawSize, yParticuleDrawSize, drawRed, drawGreen, drawBlue);
		}

		window.Display();

		if (screenshotMode and hasSimThisFrame)
		{
			saveScreenshot(window.renderer, frameNumber);
		}
		hasSimThisFrame = false;
	}

	window.CleanUp();
	SDL_Quit();

	return 0;
}

