#include <stdio.h>
#include <SDL.h>
#include <cmath>
#include <iostream>


#include "Vector2D.hpp"


Vector2D::Vector2D()
{
}

Vector2D::Vector2D(double pX, double pY)
:x(pX), y(pY)
{
}

Vector2D::Vector2D(double pX, double pY, double pZ, double pW)
:x(pX), y(pY), z(pZ), w(pW)
{

}

Vector2D Vector2D::Add(double pX, double pY)
{
	return Vector2D(x+pX, y+pY);
}

Vector2D Vector2D::Add(Vector2D pPos)
{
	return Vector2D(x+pPos.x,y+pPos.y,z+pPos.z,w+pPos.w);
}

Vector2D Vector2D::Mult(double pK)
{
	return Vector2D(x * pK, y * pK);
}

bool Vector2D::IsEqual(double pX, double pY)
{
	return x == pX and y == pY;
}

bool Vector2D::IsEqual(Vector2D pPos)
{
	return x == pPos.x and y == pPos.y;
}

Vector2D Vector2D::Invert()
{
	return Vector2D(-x,-y);
}

double Vector2D::Distance(Vector2D pGoalGlobalPos)
{	
	return sqrt(pow(x - pGoalGlobalPos.x, 2) + pow(y - pGoalGlobalPos.y, 2));
}

void Vector2D::Print()
{
	std::cout << "(" << x << " , " << y << ")" << std::endl;
}
