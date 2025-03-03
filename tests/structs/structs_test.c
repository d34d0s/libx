#include "../../stdlibx/include/stdx_memory.h"
#include "../../stdlibx/include/stdx_structs.h"

void main() {
	stdx_init_memory();
	stdx_init_structs();

	f32* verts = structs_api->create_array(sizeof(f32), 3);

	structs_api->push_array(verts, STDX_LITERAL_PTR(f32, 23));
	printf("verts[0] = %0.1f\n", verts[0]);
	
	structs_api->push_array(verts, STDX_LITERAL_PTR(f32, 420));
	printf("verts[1] = %0.1f\n", verts[1]);
	
	structs_api->push_array(verts, STDX_LITERAL_PTR(f32, 123321));
	printf("verts[2] = %0.1f\n", verts[2]);
	
	// never happens as cap has been reached, index 2 is not overwritten.
	structs_api->push_array(verts, STDX_LITERAL_PTR(f32, 23));
	printf("verts[2] = %0.1f\n", verts[2]);

	f32 out_vert;
	structs_api->pull_array(verts, 1, &out_vert);
	printf("verts[1] out = %0.1f\n", out_vert);

	Array_Head vhead = structs_api->get_array_head(verts);
	printf("size: %d\n", vhead.size);
	printf("stride: %d\n", vhead.stride);
	printf("count: %d\n", vhead.count);
	printf("max: %d\n", vhead.max);

	verts = structs_api->resize_array(verts, 6);
	
	// data persists after resizing
	STDX_FOR(u32, a, 0, vhead.count, 1) { printf("verts[%d] = %0.1f\n", a, verts[a]); }

	// array headers are automatically updated when using the API.
	vhead = structs_api->get_array_head(verts);
	printf("size: %d\n", vhead.size);
	printf("stride: %d\n", vhead.stride);
	printf("count: %d\n", vhead.count);
	printf("max: %d\n", vhead.max);
	
	structs_api->destroy_array(verts);


	Linked_Array* larr = structs_api->create_linked_array(NULL, sizeof(f32), 1);
	structs_api->put_array(larr->array, 0, &(f32){420.666});
	
	Linked_Array* larr2 = structs_api->create_linked_array(larr, sizeof(f32), 2);
	structs_api->put_array(larr2->array, 0, &(f32){123.321});
	structs_api->put_array(larr2->array, 1, &(f32){100.001});
	
	// link 3 new arrays in front of larr2
	Linked_Array* larr3 = structs_api->create_linked_array(larr, sizeof(f32), 3);
	Linked_Array* larr4 = structs_api->create_linked_array(larr, sizeof(f32), 4);
	Linked_Array* larr5 = structs_api->create_linked_array(larr, sizeof(f32), 5);
	structs_api->put_array(larr5->array, 2, &(f32){69.96});

	printf("larr->array[0]: %0.3f\n", *(f32*)larr->array);
	printf("larr2->array[0]: %0.3f\n", ((f32*)larr2->array)[0]);
	printf("larr2->array[1]: %0.3f\n", ((f32*)larr2->array)[1]);
	
	// remove a link from the middle of the linked structure
	structs_api->destroy_linked_array(larr4);

	// retrieve larr5 data from larr2 2 links behind as larr4 was destroyed
	printf("larr5->array[2]: %0.3f\n", ((f32*)larr2->last->last->array)[2]);
	
	// collapse the whole array
	structs_api->collapse_linked_array(larr3);

	Hashmap* hm = structs_api->create_hashmap(4);
	structs_api->set_hashmap(hm, "age", 	&(u32){23});
	structs_api->set_hashmap(hm, "year", 	&(u32){25});
	structs_api->set_hashmap(hm, "version", 	&(u32){202501});

	printf("age: %d\n", *(u32*)structs_api->get_hashmap(hm, "age"));
	printf("year: %d\n", *(u32*)structs_api->get_hashmap(hm, "year"));
	printf("version: %d\n", *(u32*)structs_api->get_hashmap(hm, "version"));

	if (structs_api->rem_hashmap(hm, "version"))  printf("version key removed\n");

	structs_api->destroy_hashmap(hm);

	stdx_cleanup_structs();
	printf("OK!\n");
}
