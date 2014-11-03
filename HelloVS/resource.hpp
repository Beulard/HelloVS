#include "types.hpp"
#include <string>

namespace resource {

	struct image_data {
		u8* bytes;
		//	Width, length
		int x, y;
		//	Number of channels
		int n;
	};


	enum shader_type {
		ST_VS,
		ST_FS
	};
	struct shader_data {
		shader_type type;
		std::string code;
	};

	void add_image_png(const char* filename);
	void add_shader(const char* filename);
	void loading_start();
	void loading_update();
	bool is_loading();
	bool is_done_loading();
	void destroy();
}