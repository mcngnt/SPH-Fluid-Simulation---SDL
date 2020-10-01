#pragma once
#include <stdio.h>
#include <SDL.h>
#include <cmath>

class Vector2D
{
public:
	Vector2D();
	Vector2D(double pX, double pY);
	Vector2D(double pX, double pY, double pZ, double pW);

	Vector2D Add(double pX, double pY);
	Vector2D Add(Vector2D pPos);
	Vector2D Mult(double pK);
	bool IsEqual(double pX, double pY);
	bool IsEqual(Vector2D pPos);
	Vector2D Invert();
	double Distance(Vector2D pGoalGlobalPos);
	void Print();

	double x = 0;
	double y = 0;
	double z = 0;
	double w = 0;
	bool isNull = false;
};