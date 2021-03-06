#include "SDL/SDL.h"
#include "GL/glew.h"
#include <stdexcept>
#include <cstdio>
#include "game.hpp"
#include "resource.hpp"

//	TODO Remove
#include "array.hpp"

namespace game {

	enum state {
		loading,
		main_menu,
		gameplay,
		menu
	};

	static SDL_Window* window = NULL;
	static SDL_GLContext context = NULL;
	static SDL_Event e;
	static bool exit = false;
	static state gamestate = state::loading;

	bool init() {
		if(SDL_Init(SDL_INIT_VIDEO) < 0) {
			printf("Couldn't init SDL!");
			return false;
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		//	AAx8
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

		window = SDL_CreateWindow("HelloVS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if(window == NULL){
			printf("Couldn't open window!");
			return false;
		}

		context = SDL_GL_CreateContext(window);
		if(context == NULL) {
			printf("Couldn't create OpenGL context!");
			return false;
		}

		SDL_GL_SetSwapInterval(1);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		return true;
	}

	void run() {
		resource::add_image_png("data/image/yellow.png");
 		resource::add_image_png("data/image/colors.png");
 		resource::add_image_png("data/image/colors2.png");
 		resource::add_image_png("data/image/colors3.png");
		resource::add_shader("data/shader/sprite.vs");
		resource::add_shader("data/shader/sprite.fs");


		while(!exit) {
			while(SDL_PollEvent(&e) != 0) {
				if(e.type == SDL_QUIT)
					exit = true;
			}

			switch(gamestate) {
			case loading:
				if(!resource::is_loading())
					resource::loading_start();

				resource::loading_update();

				if(resource::is_done_loading())
					gamestate = state::main_menu;
				break;

			case main_menu:
				gamestate = state::gameplay;
				break;

			case gameplay:
				glClear(GL_COLOR_BUFFER_BIT);

				//	Draw stuff
				glBegin(GL_TRIANGLES);
				glColor3f(0, 0, 1.f);
				glVertex2f(-.5f, -.5f);
				glColor3f(1.f, 0, 0);
				glVertex2f(0, .5f);
				glColor3f(0, 1.f, 0);
				glVertex2f(.5f, -.5f);
				glEnd();

				break;

			case menu:
				break;
			}

			SDL_GL_SwapWindow(window);
		}
	}

	void destroy() {
		resource::destroy();
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

}