#pragma once

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <cmath>
#include <vector>
#include <iostream>



static const int WINDOW_WIDTH = 1850;
static const int WINDOW_HEIGHT = 800;
static const float STAR_MASS = 1000000.f;   
static const float G = 1e-3;                
static const float PIXELS_PER_METER = 30.f;


enum CollisionGroups {
	GROUP_STAR = 0x00000001,
	GROUP_PLANET = 0x00000002
};
