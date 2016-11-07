#ifndef DIAMONDSQUARE_H_
#define DIAMONDSQUARE_H_

#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define ROUGHNESS 0.7 //0.6 //the amount of noise on the first step

#include "diamondsquare.h"

void diamondSquare(double* map, int width, int height);
int isPowerOfTwo(int x);
void initMap(double* map);
void diamondSquareRecurse(double* map, int x, int y, int w, int h, double noiseScale);
double randomDouble();
int max(int a, int b);
double get(double* map, int y, int x);
void set(double* map, int y, int x, double set);

#endif
