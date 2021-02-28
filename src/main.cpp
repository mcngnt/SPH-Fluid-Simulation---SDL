#include <iostream>
#include <stdio.h>
#include <SDL.h>
// #include <SDL_image.h>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>



#include "RenderWindow.hpp"

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGTH 720
#define MAIN_TIME_STEP 20
#define FLUID_DRAW_FACTOR 2
#define CELL_SIZE 10
#define GRID_CELL_W (SCREEN_WIDTH - SCREEN_WIDTH % CELL_SIZE)/CELL_SIZE + 1
#define GRID_CELL_H (SCREEN_HEIGTH - SCREEN_HEIGTH % CELL_SIZE)/CELL_SIZE + 1

struct Particule {
	double xPos;
	double yPos;
	double density;
	int wallflag;
	double xForce;
	double yForce;
	double xVelocity;
	double yVelocity;
	int gridX;
	int gridY;
};

int	totalOfParticules;
int gridPosX, gridPosY;
double xParticuleDistance, yParticuleDistance;
double particulesInteraction;
double particulesDistance;
double xScreenPos, yScreenPos;
double xParticuleDrawSize, yParticuleDrawSize;
int drawRed, drawGreen, drawBlue;
double densityDrawGradient, velocityDrawGradient;

int mousePosX, mousePosY;
std::string inputFile;
std::string mainFileName = " ";


double zoomFactor = 4;
double timeStep = MAIN_TIME_STEP;
double gravity = 0.3, pressure = 4, viscosity = 7;
unsigned long frameNumber;

// int gridWidth = ;
// int gridHeight = (SCREEN_HEIGTH - SCREEN_HEIGTH % 100)/100;

std::vector<int> gridCell[GRID_CELL_W][GRID_CELL_H];

std::vector<Particule> particules(0);

void console(std::string pMessage)
{
	std::cout << std::endl << "CONSOLE SAYING -> " << pMessage;
}

void openGame(std::string pInput)
{
	std::string fileName("saves\\");
	fileName.append(pInput);

	// console("check1");
	std::ifstream file;
	// console("check2");
	file.open(fileName);
	std::string line;

    if (file.is_open())
    {
    	particules.resize(0);
		getline(file,line);
		totalOfParticules = std::stoi(line);
		getline(file,line);
		zoomFactor = std::stod(line);
		getline(file,line);
		frameNumber = std::stoi(line);
    	while (getline(file,line))
    	{
    	    std::vector<std::string> stringParts;
    	    stringParts.push_back("");
    	    for(std::string::size_type i = 0; i < line.size(); ++i)
    	    {
   				if (line[i] == ' ')
   				{
   					stringParts.push_back("");
   				}
   				stringParts[stringParts.size()-1].append(1, line[i]);
			}

			Particule part;
			part.xPos = std::stod(stringParts[0]);
			part.yPos = std::stod(stringParts[1]);
			part.density = std::stod(stringParts[2]);
			part.wallflag = std::stod(stringParts[3]);
			part.xVelocity = std::stod(stringParts[4]);
			part.yVelocity = std::stod(stringParts[5]);

			particules.push_back(part);
    	}
    }
    else
    {
    	console("Can't open file !");
    }
}

std::pair<int, int> get_neighbour_pos(int cellX, int cellY, int neighbourID)
{
	std::pair<int, int> res;
	if (neighbourID == 1)
	{
		res.first = cellX-1;
		res.second = cellY-1;
	}
	if (neighbourID == 2)
	{
		res.first = cellX;
		res.second = cellY-1;
	}
	if (neighbourID == 3)
	{
		res.first = cellX+1;
		res.second = cellY-1;
	}
	if (neighbourID == 4)
	{
		res.first = cellX-1;
		res.second = cellY;
	}
	if (neighbourID == 5)
	{
		res.first = cellX;
		res.second = cellY;
	}
	if (neighbourID == 6)
	{
		res.first = cellX+1;
		res.second = cellY;
	}
	if (neighbourID == 7)
	{
		res.first = cellX-1;
		res.second = cellY+1;
	}
	if (neighbourID == 8)
	{
		res.first = cellX;
		res.second = cellY+1;
	}
	if (neighbourID == 9)
	{
		res.first = cellX+1;
		res.second = cellY+1;
	}
	res.first = fmax(fmin(res.first, GRID_CELL_W-1), 0);
	res.second = fmax(fmin(res.second, GRID_CELL_H-1), 0);
	return res;
}

void saveGame(std::string pFileName)
{
	std::string fileName = "saves/";
	fileName.append(pFileName);
	fileName.append(".sfs");

	std::string content = std::to_string(totalOfParticules);
	content.append("\n");
	content.append(std::to_string(zoomFactor));
	content.append("\n");
	content.append(std::to_string(frameNumber));
	content.append("\n");
	

	for (int i = 0; i < totalOfParticules; ++i)
	{
		Particule myPart = particules[i];
		std::string info = "";
		info.append(std::to_string(myPart.xPos));
		info.append(" ");
		info.append(std::to_string(myPart.yPos));
		info.append(" ");
		info.append(std::to_string(myPart.density));
		info.append(" ");
		info.append(std::to_string(myPart.wallflag));
		info.append(" ");
		info.append(std::to_string(myPart.xVelocity));
		info.append(" ");
		info.append(std::to_string(myPart.yVelocity));
		info.append("\n");
		content.append(info);
	}

	// console(fileName);
	std::ofstream file;
	file.open(fileName);
	if (file.is_open())
	{
		file << content;
		std::string message = "The save ";
		message.append(pFileName);
		message.append(".sfs was save in saves folder");
		console(message);
	}
	else
	{
		console("Can't save file !");
	}
	
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

	std::string message = "Screenshot ";
	message.append(std::to_string(pFramesNumber));
	message.append(".bmp saved in images folder");

	console(message);
}

int main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) > 0)
	{
		std::cout << "SDL_INIT has failed. Error -> " << SDL_GetError() << std::endl;
	}
	// if (!IMG_Init(IMG_INIT_PNG))
	// {
	// 	std::cout << "IMG_INIT has failed. Error -> " << SDL_GetError() << std::endl;
	// }


	RenderWindow window("Game", SCREEN_WIDTH, SCREEN_HEIGTH);

	bool gameRunning = true;
	SDL_Event event;
	double currentTimeStep = 0;
	bool isSimRunning = false;
	int currentType = 0;
	bool canOverlapParts = false;
	bool isPlayingOneStep = false;
	bool screenshotMode = false;
	bool hasSimThisFrame = false;
	int pencilSize = 1;
	bool drawParticuleDensity = true;



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
					particulesDistance = pow(xParticuleDistance,2.0) + pow(yParticuleDistance,2.0);
					particulesInteraction = particulesDistance / 2.0 - 1.0;
	
					if (particulesDistance < pow(5+pencilSize/25, 2)){
						particules.erase(particules.begin() + i);
						totalOfParticules--;
					}
				}
			}
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				int xPos = floor(mousePosX/zoomFactor);
				int yPos = floor(mousePosY/zoomFactor);

				for (int xi = xPos-pencilSize; xi < xPos+pencilSize; ++xi)
				{
					for (int yi = yPos-pencilSize; yi < yPos+pencilSize; ++yi)
					{
						if(pow(xi-xPos,2.0) + pow(yi-yPos,2.0) >= pencilSize)
						{
							continue;
						}

						bool isCreatePart = true;

						for (int i = 0; i < totalOfParticules; ++i)
						{
							xParticuleDistance = xi - particules[i].xPos;
							yParticuleDistance = yi - particules[i].yPos;
							particulesDistance = pow(xParticuleDistance,2.0) + pow(yParticuleDistance,2.0);

							if (particulesDistance < 1 and !canOverlapParts){
								isCreatePart = false;
								break;
							}
						}
	
						if (isCreatePart)
						{
							Particule part;
							particules.push_back(part);
							particules[totalOfParticules].xPos = xi;
							particules[totalOfParticules].yPos = yi;
							particules[totalOfParticules].wallflag = currentType;
							totalOfParticules++;
						}
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
							timeStep = 0;
							if (mainFileName == " ")
							{
								console("Enter the save name : ");
								std::cin >> mainFileName;
							}
						}
						else
						{
							console("Screenshot mode disable");
							timeStep = MAIN_TIME_STEP;
						}
						break;
					}
					case SDLK_i:
					{
						std::string s = "There is ";
						s.append(std::to_string(totalOfParticules));
						s.append(" particules");
						console(s);
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
						pencilSize *= 2;
						if (pencilSize > 2000)
						{
							pencilSize = 1;
						}
						std::string s = "Pencil size changed to ";
						s.append(std::to_string(pencilSize));
						console(s);
						break;
					}
					case SDLK_n:
					{
						saveGame(std::to_string(frameNumber));
						break;
					}
					case SDLK_l:
					{
						console("Enter the save name : ");
						std::string fileName;
						std::cin >> fileName;
						openGame(fileName);
						break;
					}
					case SDLK_u:
					{
						drawParticuleDensity = !drawParticuleDensity;
						break;
					}
				}
			}
		}

		window.Clear(0,10,15);
		if (SDL_GetTicks() - currentTimeStep > timeStep or isPlayingOneStep)
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

			// std::cout <<  "---" << std::endl;

			for (int x = 0; x < GRID_CELL_W; ++x)
			{
				for (int y = 0; y < GRID_CELL_H; ++y)
				{
					gridCell[x][y].clear();
				}
			}

			// std::cout <<  "clear" << std::endl;

			for (int particulesCursor = 0; particulesCursor < totalOfParticules; particulesCursor++)
			{
				// x = round(particules[particulesCursor].xPos);
				// y = round(particules[particulesCursor].yPos);
				gridPosX = floor(particules[particulesCursor].xPos);
				gridPosY = floor(particules[particulesCursor].yPos);
				gridPosX = fmin( (gridPosX - gridPosX%CELL_SIZE)/CELL_SIZE, GRID_CELL_W-1 );
				gridPosY = fmin( (gridPosY - gridPosY%CELL_SIZE)/CELL_SIZE, GRID_CELL_H-1 );

				particules[particulesCursor].gridX = gridPosX;
				particules[particulesCursor].gridY = gridPosY;
				// std:: cout << gridPosX << " " << gridPosY << std::endl;

				gridCell[gridPosX][gridPosY].push_back(particulesCursor);

			}
			// std::cout <<  "setup" << std::endl;

			for (particulesCursor = 0; particulesCursor < totalOfParticules; particulesCursor++){
				particules[particulesCursor].density = particules[particulesCursor].wallflag * 9;
				std::pair<int, int> centralGridCoord;
				centralGridCoord.first = particules[particulesCursor].gridX;
				centralGridCoord.second = particules[particulesCursor].gridY;
				for (int i = 1; i < 10; ++i)
				{
					std::pair<int, int> gridCellCoord = get_neighbour_pos(centralGridCoord.first, centralGridCoord.second, i);
					if (gridCellCoord == centralGridCoord and i != 5)
					{
						continue;
					}
					std::vector<int> cellParticulesID = gridCell[gridCellCoord.first][gridCellCoord.second];
					for (unsigned i = 0; i < cellParticulesID.size(); ++i)
					{
						// cellParticulesID[i] = fmin(cellParticulesID[i], totalOfParticules);
						// if (cellParticulesID[i] >= totalOfParticules-1)
						// {
						// 	std::cout <<  "meh" << std::endl;
						// }
						xParticuleDistance = particules[particulesCursor].xPos - particules[cellParticulesID[i]].xPos;
						yParticuleDistance = particules[particulesCursor].yPos - particules[cellParticulesID[i]].yPos;
						particulesDistance = xParticuleDistance*xParticuleDistance + yParticuleDistance*yParticuleDistance;
	
						if (particulesDistance < 4){
							particulesDistance = sqrt(particulesDistance);
							particulesInteraction = particulesDistance / 2.0 - 1.0;
							particules[particulesCursor].density += particulesInteraction * particulesInteraction;
						}

					}
				}

				// for (particulesCursor2 = 0; particulesCursor2 < totalOfParticules; particulesCursor2++){
	
				// 	xParticuleDistance = particules[particulesCursor].xPos - particules[particulesCursor2].xPos;
				// 	yParticuleDistance = particules[particulesCursor].yPos - particules[particulesCursor2].yPos;
				// 	particulesDistance = xParticuleDistance*xParticuleDistance + yParticuleDistance*yParticuleDistance;
	
				// 	if (particulesDistance < 4){
				// 		particulesDistance = sqrt(particulesDistance);
				// 		particulesInteraction = particulesDistance / 2.0 - 1.0;
				// 		particules[particulesCursor].density += particulesInteraction * particulesInteraction;
				// 	}

				// }
			}
			// std::cout <<  "a" << std::endl;
	
				for (particulesCursor = 0; particulesCursor < totalOfParticules; particulesCursor++)
				{
					particules[particulesCursor].yForce = gravity;
					particules[particulesCursor].xForce = 0;
	
					std::pair<int, int> centralGridCoord;
					centralGridCoord.first = particules[particulesCursor].gridX;
					centralGridCoord.second = particules[particulesCursor].gridY;
					for (int i = 1; i < 10; ++i)
					{
						std::pair<int, int> gridCellCoord = get_neighbour_pos(centralGridCoord.first, centralGridCoord.second, i);
						if (gridCellCoord == centralGridCoord and i != 5)
						{
							continue;
						}
						std::vector<int> cellParticulesID = gridCell[gridCellCoord.first][gridCellCoord.second];
						for (unsigned i = 0; i < cellParticulesID.size(); ++i)
						{
							// cellParticulesID[i] = fmin(cellParticulesID[i], totalOfParticules);
							// if (cellParticulesID[i] >= totalOfParticules-1)
							// {
							// 	std::cout <<  "meh" << std::endl;
							// }
							xParticuleDistance = particules[particulesCursor].xPos - particules[cellParticulesID[i]].xPos;
							yParticuleDistance = particules[particulesCursor].yPos - particules[cellParticulesID[i]].yPos;
							particulesDistance = xParticuleDistance*xParticuleDistance + yParticuleDistance*yParticuleDistance;
	
							if (particulesDistance < 4)
							{
								particulesDistance = sqrt(particulesDistance);
								particulesInteraction = particulesDistance / 2.0 - 1.0;
								particules[particulesCursor].xForce += particulesInteraction * (xParticuleDistance * (3 - particules[particulesCursor].density - particules[cellParticulesID[i]].density) * pressure + particules[particulesCursor].xVelocity * viscosity - particules[cellParticulesID[i]].xVelocity * viscosity) / particules[particulesCursor].density;
								particules[particulesCursor].yForce += particulesInteraction * (yParticuleDistance * (3 - particules[particulesCursor].density - particules[cellParticulesID[i]].density) * pressure + particules[particulesCursor].yVelocity * viscosity - particules[cellParticulesID[i]].yVelocity * viscosity) / particules[particulesCursor].density;
							}

						}
					}
				}

				// std::cout <<  "b" << std::endl;
	
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
			// std::cout <<  "c" << std::endl;
		}
		endSim:
		
		for (int i = 0; i < totalOfParticules; ++i)
		{
			xScreenPos = particules[i].xPos * zoomFactor;
			yScreenPos = particules[i].yPos * zoomFactor;

			if (yScreenPos > SCREEN_HEIGTH-0 || yScreenPos < 0 || xScreenPos < 0 || xScreenPos > SCREEN_WIDTH-0)
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
				if (drawParticuleDensity)
				{
					densityDrawGradient = particules[i].density / 5;
					velocityDrawGradient = (abs(particules[i].xVelocity) + abs(particules[i].yVelocity))/5;
					if (densityDrawGradient > 1)
					{
						densityDrawGradient = 1;
					}
					if (velocityDrawGradient > 1)
					{
						velocityDrawGradient = 1;
					}
					drawRed = 255 * densityDrawGradient;
					drawGreen = 255 * velocityDrawGradient;
				}
			}

			window.DrawRect(xScreenPos, yScreenPos, xParticuleDrawSize, yParticuleDrawSize, drawRed, drawGreen, drawBlue);
		}

		// std::cout <<  "d" << std::endl;

		// console(std::to_string(totalOfParticules).c_str());

		window.Display();

		if (screenshotMode and hasSimThisFrame)
		{
			saveScreenshot(window.renderer, frameNumber);
			saveGame(mainFileName);
		}
		hasSimThisFrame = false;
	}

	window.CleanUp();
	SDL_Quit();

	return 0;
}

