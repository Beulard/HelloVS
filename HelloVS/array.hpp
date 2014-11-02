#include "types.hpp"

namespace array {

	//	The array struct contains a continuous memory chunk in which lie
	//	the size of items, the item count and the actual data
	//	The structure of the memory chunk goes like this :
	//	Bytes 0-3 : item size (unsigned int)
	//	Bytes 4-7 : item count (unsigned int)
	//	Bytes after 8 : data
	struct array {
		void* data;
	};

	enum {
		ITEM_SIZE_POS = 0,
		ITEM_COUNT_POS = 4,
		HEADER_SIZE = 8,
		DATA_POS = 8
	};

	array create(u32 itemSize, u32 count);
	void destroy(array* a);
	void resize(array* a, u32 count);
	void* at(array* a, u32 index);
	const u32 get_item_count(array* a);
	const u32 get_item_size(array* a);
	void set(array* a, int value, u32 index, u32 count = 1);
	void zero(array* a, u32 index, u32 count = 1);
	void zero_all(array* a);

}