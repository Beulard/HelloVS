#include "resource.hpp"
#include "array.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "pthread.h"
#include <string>

//	TODO Remove
#include <vector>
#include <windows.h>
//

namespace resource {
	struct image_loader_thread {
		pthread_t id;
		pthread_mutex_t mutex;
		bool working;
		std::string filename;
	};

	image_loader_thread create_image_loader_thread(entrypoint func, void* arg, const std::string& filename) {
		pthread_t tid;
		if(pthread_create(&tid, NULL, func, arg))
			printf("Couldn't create thread\n");
		image_loader_thread thread;
		thread.id = tid;
		thread.mutex = PTHREAD_MUTEX_INITIALIZER;
		thread.working = false;
		thread.filename = filename;
		return thread;
	}

	static bool done = false;
	static bool loading = false;
	static std::vector<std::string> image_files;
	static array::array image_threads;


	void _load_image_png(void* thread_data) {
		image_loader_thread* thread = (image_loader_thread*)thread_data;
		pthread_mutex_lock(&thread->mutex);
		thread->working = true;
		int x, y, n;
		//	We pass 4 as the fourth argument because we want to force RGBA format
		u8* data = stbi_load(thread->filename.c_str(), &x, &y, &n, 4);
		Sleep(500);
		printf("%d says done\n", pthread_self());
		pthread_mutex_unlock(&thread->mutex);
		//	Return image data
		pthread_exit(&data);
	}

	void add_image_png(const char* filename) {
		image_files.push_back(filename);
	}

	void loading_start() {
		loading = true;
		//	Initialize image threads
		image_threads = array::create(sizeof(image_loader_thread), image_files.size());
		array::zero_all(&image_threads);

		for(u32 i = 0; i < get_item_count(&image_threads); ++i) {
			image_loader_thread* thread = (image_loader_thread*)array::at(&image_threads, i);
			*thread = create_image_loader_thread((entrypoint)_load_image_png, (void*)thread, image_files[i].c_str());
		}
	}

	void loading_update() {
		static u32 done_image_threads = 0;
		for(u32 i = 0; i < get_item_count(&image_threads); ++i) {
			image_loader_thread* thread = (image_loader_thread*)array::at(&image_threads, i);
			//	Check if the thread is done working
			if(pthread_mutex_trylock(&thread->mutex) == 0) {
				if(thread->working) {
					pthread_mutex_unlock(&thread->mutex);
					u8* data = NULL;
					pthread_join(thread->id, (void**)&data);
					printf("joined thread!\n");
					thread->working = false;
					//	Add 1 to the number of threads that are done
					done_image_threads++;
				}
				else {
					pthread_mutex_unlock(&thread->mutex);
					printf("not working\n");
				}
			}
		}

		printf("main thread waiting...\n");

		if(done_image_threads >= get_item_count(&image_threads)) {
			printf("Images all done !");
			done = true;
		}

	}

	bool is_loading() {
		return loading;
	}

	bool is_done_loading() {
		return done;
	}

	void destroy() {
		array::destroy(&image_threads);
	}

}