#include "resource.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "types.hpp"
#include "pthread.h"
#include <vector>
#include <string>
#include <ctime>
#include <random>
#include <windows.h>

namespace resource {
	struct image_loader_thread {
		pthread_t id;
		pthread_mutex_t mutex;
		bool working;
		std::string filename;
	};

	bool loading = false;
	std::vector<std::string> images;
	std::vector<pthread_t> threads;
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	bool done = false;
	bool working = false;


	void _load_image_png(void* filename) {
		pthread_mutex_lock(&mutex);
		working = true;
		int x, y, n;
		//	We pass 4 as the fourth argument because we want to force RGBA format
		u8* data = stbi_load((const char*)filename, &x, &y, &n, 4);
		Sleep(500);
		printf("%d says done\n", pthread_self());
		done = true;
		pthread_mutex_unlock(&mutex);
		//pthread_cond_signal(&cond);
		pthread_exit(&data);
	}

	void add_image_png(const char* filename) {
		images.push_back(filename);
	}

	void loading_start() {
		srand(24);
		loading = true;
		cond = PTHREAD_COND_INITIALIZER;
		mutex = PTHREAD_MUTEX_INITIALIZER;
		for(u32 i = 0; i < images.size(); ++i) {
			pthread_t thread;
			if(pthread_create(&thread, NULL, (entrypoint)_load_image_png, (void*)images[i].c_str()) != 0)
				printf("Couldn't create thread\n");
			threads.push_back(thread);
		}
	}

	void loading_update() {
		if(pthread_mutex_trylock(&mutex) == 0) {
			if(working) {
				pthread_mutex_unlock(&mutex);
				u8* data = NULL;
				pthread_join(threads.back(), (void**)&data);
				printf("joined thread!\n");
				working = false;
			}
			else {
				pthread_mutex_unlock(&mutex);
				printf("not working\n");
			}
		}
		printf("main thread waiting...\n");
	}

	bool is_loading() {
		return loading;
	}

}