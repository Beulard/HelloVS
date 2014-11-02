// HelloVS.cpp : Defines the entry point for the console application.
#include <cstdio>
#include <cstdlib>
#include "SDL/SDL.h"
#include "GL/glew.h"
#include "game.hpp"

int main(int argc, char* argv[])
{
	if(!game::init())
		return -1;
	
	game::run();

	game::destroy();

	system("PAUSE");
	return 0;
}

