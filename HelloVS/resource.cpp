#include "resource.hpp"
#include "array.hpp"
#include "file.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "pthread.h"
#include <string>
#include <vector>

namespace resource {

	struct loader_thread {
		pthread_t id;
		pthread_mutex_t mutex;
		bool working;
		std::string filename;
		void* data;
	};

	loader_thread create_loader_thread(entrypoint func, void* arg, const std::string& filename, void* data) {
		pthread_t tid;
		if(pthread_create(&tid, NULL, func, arg))
			printf("Couldn't create thread\n");
		loader_thread thread;
		thread.id = tid;
		thread.mutex = PTHREAD_MUTEX_INITIALIZER;
		thread.working = false;
		thread.filename = filename;
		thread.data = data;
		return thread;
	}

	static bool done = false;
	static bool loading = false;

	static std::vector<std::string> image_files;
	static std::vector<std::string> shader_files;

	static array::array image_threads;
	static array::array image_data_array;

	static array::array shader_threads;
	static array::array shader_data_array;


	void _load_image_png(void* thread_data) {
		loader_thread* thread = (loader_thread*)thread_data;
		pthread_mutex_lock(&thread->mutex);
		thread->working = true;
		image_data* data = (image_data*)thread->data;

		//	We pass 4 as the fourth argument because we want to force RGBA format
		data->bytes = stbi_load(thread->filename.c_str(), &data->x, &data->y, &data->n, 4);

		printf("%d says done\n", pthread_self());
		pthread_mutex_unlock(&thread->mutex);
		
		pthread_exit(NULL);
	}

	void _load_shader(void* thread_data) {
		loader_thread* thread = (loader_thread*)thread_data;
		pthread_mutex_lock(&thread->mutex);
		thread->working = true;
		shader_data* data = (shader_data*)thread->data;
		data->code = file::read_all(thread->filename.c_str());
		u32 name_length = thread->filename.length();
		if(thread->filename[name_length - 2] == 'v')
			data->type = ST_VS;
		else if(thread->filename[name_length - 2] == 'f')
			data->type = ST_FS;
		else
			printf("File name '%s' doesn't describe shader type!\n", thread->filename.c_str());

		pthread_mutex_unlock(&thread->mutex);
		
		pthread_exit(NULL);
	}

	void add_image_png(const char* filename) {
		image_files.push_back(filename);
	}

	void add_shader(const char* filename) {
		shader_files.push_back(filename);
	}


	void loading_start() {
		loading = true;
		//	Initialize image threads
		image_threads = array::create(sizeof(loader_thread), image_files.size());
		//array::zero_all(&image_threads);
		//Initialize image data array
		image_data_array = array::create(sizeof(image_data), image_files.size());

		for(u32 i = 0; i < get_item_count(&image_threads); ++i) {
			loader_thread* thread = (loader_thread*)array::at(&image_threads, i);
			*thread = create_loader_thread((entrypoint)_load_image_png, (void*)thread, image_files[i].c_str(), array::at(&image_data_array, i));
		}

		//	Initialize shader threads
		shader_threads = array::create(sizeof(loader_thread), shader_files.size());
		//array::zero_all(&shader_threads);
		//	Initialize shader data array
		shader_data_array = array::create(sizeof(shader_data), shader_files.size());
		array::zero_all(&shader_data_array);
		for(u32 i = 0; i < get_item_count(&shader_threads); ++i) {
			loader_thread* thread = (loader_thread*)array::at(&shader_threads, i);
			*thread = create_loader_thread((entrypoint)_load_shader, (void*)thread, shader_files[i].c_str(), array::at(&shader_data_array, i));
		}
	}

	void loading_update() {
		static u32 done_image_threads = 0;
		for(u32 i = 0; i < get_item_count(&image_threads); ++i) {
			loader_thread* thread = (loader_thread*)array::at(&image_threads, i);
			//	Check if the thread is done working
			if(pthread_mutex_trylock(&thread->mutex) == 0) {
				if(thread->working) {
					pthread_mutex_unlock(&thread->mutex);
					pthread_join(thread->id, NULL);
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

		static u32 done_shader_threads = 0;
		for(u32 i = 0; i < get_item_count(&shader_threads); ++i) {
			loader_thread* thread = (loader_thread*)array::at(&shader_threads, i);

			if(pthread_mutex_trylock(&thread->mutex) == 0) {
				if(thread->working) {
					pthread_mutex_unlock(&thread->mutex);
					pthread_join(thread->id, NULL);
					printf("joined shader thread!\n");
					thread->working = false;

					done_shader_threads++;
				}
				else {
					pthread_mutex_unlock(&thread->mutex);
					printf("shader not working\n");
				}
			}
		}

		printf("main thread waiting...\n");
		static bool images_done = false;
		if(done_image_threads >= get_item_count(&image_threads)) {
			printf("Images all done !\n");
			images_done = true;
		}
		static bool shaders_done = false;
		if(done_shader_threads >= get_item_count(&shader_threads)) {
			printf("Shaders all done !\n");
			shaders_done = true;
		}
		if(shaders_done && images_done) {
			printf("All done !\n");
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
		array::destroy(&shader_threads);

		array::destroy(&image_data_array);
		array::destroy(&shader_data_array);
	}

}