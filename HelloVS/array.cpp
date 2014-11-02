#include "array.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace array {

	void set_item_size(array* a, u32 size) {
		*((char*)a->data + ITEM_SIZE_POS) = size;
	}

	void set_item_count(array* a, u32 count) {
		*((char*)a->data + ITEM_COUNT_POS) = count;
	}

	array create(u32 item_size, u32 count) {
		array a;
		a.data = malloc(HEADER_SIZE + item_size * count);
		set_item_size(&a, item_size);
		set_item_count(&a, count);
		zero_all(&a);
		return a;
	}

	void destroy(array* a) {
		free(a->data);
	}

	void resize(array* a, u32 count) {
 		a->data = realloc(a->data, HEADER_SIZE + count * get_item_size(a));
		set_item_count(a, count);
	}

	void* at(array* a, u32 index) {
		if(index < get_item_count(a))
			return ((char*)a->data + HEADER_SIZE + index * get_item_size(a));
		else
			printf("out of bounds !\n");
		return NULL;
	}

	const u32 get_item_size(array* a) {
		return (u32)(*((char*)a->data + ITEM_SIZE_POS));
	}

	const u32 get_item_count(array* a) {
		return (u32)(*((char*)a->data + ITEM_COUNT_POS));
	}

	void set(array* a, int value, u32 index, u32 count) {
		memset((char*)a->data + HEADER_SIZE + index * get_item_size(a), value, count * get_item_size(a));
	}

	void zero(array* a, u32 index, u32 count) {
		set(a, 0, index, count);
	}

	void zero_all(array* a) {
		zero(a, 0, get_item_count(a));
	}

}