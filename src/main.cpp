#include <iostream>
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>

#include "RenderWindow.hpp"
#include "Vector2D.hpp"

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGTH 720
#define TIME_STEP 20
#define ZOOM_VAL 4
#define PARTICULE_SIZE_DRAW 2

Vector2D C_RED(255, 0, 0, 255);
Vector2D C_BLUE(0, 0, 255, 255);

// int xSandboxAreaScan = 0, ySandboxAreaScan = 0;

struct Particule {
	double xPos;
	double yPos;
	double density;
	int wallflag;
	double xForce;
	double yForce;
	double xVelocity;
	double yVelocity;
	int isNotDefined;
};

double xParticuleDistance, yParticuleDistance;
double particulesInteraction;
double particulesDistance;

int	x, y, totalOfParticules;
double gravity = .3, pressure = 4, viscosity = 7;

// char input[] =
// " ";
// int inputSize = sizeof(input);
std::vector<Particule> particules(1);

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
	bool isSimRunning = true;
	int currentType = 0;
	bool canOverlapParts = false;

	// int test;
	// test << std::cin;

	// int particulesCounter = 0;
	// for (int i = 0; i < inputSize; ++i)
	// {
	// 	char x = input[i];
	// 	// continue;
	// 	switch (x) {
	// 		case '\n':
	// 			ySandboxAreaScan += 1;
	// 			xSandboxAreaScan = -1;
	// 			break;
	// 		case ' ':
	// 			particules[particulesCounter].isNotDefined = 1;
	// 			break;
	// 		case '#':
	// 			particules[particulesCounter].wallflag = 1;
	// 		default:

	// 			particules[particulesCounter].isNotDefined = 0;
	// 			particules[particulesCounter].xPos = xSandboxAreaScan;
	// 			particules[particulesCounter].yPos = ySandboxAreaScan;

	// 			particulesCounter += 1;
	// 			totalOfParticules = particulesCounter;
	// 	}
	// 	xSandboxAreaScan += 1;
	// }


	while (gameRunning)
	{
		SDL_GetMouseState(&mousePosX,&mousePosY);

		while (SDL_PollEvent(&event))
		{
			// std::cout<<event.type<<std::endl;
			// std::cout<<event.button.state<<std::endl;
			if (event.type == SDL_QUIT)
			{
				gameRunning = false;
			}
			if (event.button.button == SDL_BUTTON_RIGHT or event.button.button == SDL_BUTTON_MIDDLE)
			{
				int xPos = floor(mousePosX/ZOOM_VAL);
				int yPos = floor(mousePosY/ZOOM_VAL);

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
				int xPos = floor(mousePosX/ZOOM_VAL);
				int yPos = floor(mousePosY/ZOOM_VAL);

				bool isCreatePart = true;

				if (true)
				{
					for (int i = 0; i < totalOfParticules; ++i)
					{
						xParticuleDistance = xPos - particules[i].xPos;
						yParticuleDistance = yPos - particules[i].yPos;
						particulesDistance = sqrt( pow(xParticuleDistance,2.0) + pow(yParticuleDistance,2.0));
		
						if (particulesDistance < 1){
							isCreatePart = false;
							break;
						}
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
			}
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_SPACE:
						isSimRunning = !isSimRunning;
						break;
					case SDLK_x:
						currentType = !currentType;
					case SDLK_s:
						canOverlapParts = !canOverlapParts;
				}
			}
		}

		window.Clear();
		//Game logic
		if (SDL_GetTicks() - currentTimeStep > TIME_STEP)
		{
			// std::cout<<SDL_GetTicks() - currentTimeStep<<std::endl;
			// if (SDL_GetTicks() - currentTimeStep > 22)
			// {
			// 	std::cout<<totalOfParticules<<std::endl;
			// }
			currentTimeStep = SDL_GetTicks();

			if (!isSimRunning)
			{
				goto endSim;
			}

			// Particule part;
			// particules.push_back(part);
			// particules[totalOfParticules].xPos = 50;
			// particules[totalOfParticules].yPos = 20;
			// particules[totalOfParticules].xVelocity = 1;
			// // particules[totalOfParticules].wallflag = currentType;
			// totalOfParticules++;
			// std::cout<<totalOfParticules<<std::endl;



			int particulesCursor, particulesCursor2;

			// Iterate over every pair of particules to calculate the densities
			for (particulesCursor = 0; particulesCursor < totalOfParticules; particulesCursor++){
				// density of "wall" particules is high, other particules will bounce off them.
				particules[particulesCursor].density = particules[particulesCursor].wallflag * 9;
	
					for (particulesCursor2 = 0; particulesCursor2 < totalOfParticules; particulesCursor2++){
	
						xParticuleDistance = particules[particulesCursor].xPos - particules[particulesCursor2].xPos;
						yParticuleDistance = particules[particulesCursor].yPos - particules[particulesCursor2].yPos;
						particulesDistance = sqrt( pow(xParticuleDistance,2.0) + pow(yParticuleDistance,2.0));
						particulesInteraction = particulesDistance / 2.0 - 1.0;
	
						if (floor(1.0 - particulesInteraction) > 0){
							particules[particulesCursor].density += particulesInteraction * particulesInteraction;
						}
					}
					// std::cout<<particules[particulesCursor].density<<std::endl;
				}
	
				// Iterate over every pair of particules to calculate the forces
				for (particulesCursor = 0; particulesCursor < totalOfParticules; particulesCursor++){
					particules[particulesCursor].yForce = gravity;
					particules[particulesCursor].xForce = 0;
	
					for (particulesCursor2 = 0; particulesCursor2 < totalOfParticules; particulesCursor2++){
	
						xParticuleDistance = particules[particulesCursor].xPos - particules[particulesCursor2].xPos;
						yParticuleDistance = particules[particulesCursor].yPos - particules[particulesCursor2].yPos;
						particulesDistance = sqrt( pow(xParticuleDistance,2.0) + pow(yParticuleDistance,2.0));
						particulesInteraction = particulesDistance / 2.0 - 1.0;
	
						// force is updated only if particules are close enough
						if (floor(1.0 - particulesInteraction) > 0){
							particules[particulesCursor].xForce += particulesInteraction * (xParticuleDistance * (3 - particules[particulesCursor].density - particules[particulesCursor2].density) * pressure + particules[particulesCursor].xVelocity *
							  viscosity - particules[particulesCursor2].xVelocity * viscosity) / particules[particulesCursor].density;
							particules[particulesCursor].yForce += particulesInteraction * (yParticuleDistance * (3 - particules[particulesCursor].density - particules[particulesCursor2].density) * pressure + particules[particulesCursor].yVelocity *
							  viscosity - particules[particulesCursor2].yVelocity * viscosity) / particules[particulesCursor].density;
						}
					}
				}
	
				for (particulesCursor = 0; particulesCursor < totalOfParticules; particulesCursor++) {
	
					if (!particules[particulesCursor].wallflag) {
						
						if ( sqrt( pow(particules[particulesCursor].xForce ,2.0) + pow(particules[particulesCursor].yForce,2.0)) < 4.2) {
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
			if (particules[i].isNotDefined != 1)
			{
				Vector2D pos(particules[i].xPos * ZOOM_VAL, particules[i].yPos * ZOOM_VAL);
				if (pos.y > SCREEN_HEIGTH)
				{
					particules.erase(particules.begin()+i);
					totalOfParticules--;
				}
				Vector2D width(ZOOM_VAL,ZOOM_VAL);
				Vector2D color = C_BLUE;
				if (particules[i].wallflag == 1)
				{
					color = C_RED;
				}
				else
				{
					width = width.Mult(PARTICULE_SIZE_DRAW);
					double gradientVal = particules[i].density / 5;
					color = color.Add(C_RED.Mult(gradientVal));

				}
				window.DrawRect(pos, width, color);
			}
			
		}
		// std::cout << "run" << std::endl;

		window.Display();
	}

	window.CleanUp();
	SDL_Quit();

	return 0;
}