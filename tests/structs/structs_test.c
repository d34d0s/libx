#include "../../stdlibx/include/stdx_memory.h"
#include "../../stdlibx/include/stdx_structs.h"

void main() {
	stdx_init_memory();
	stdx_init_structs();

	f32* verts = structs_api->create_array(sizeof(f32), 3);

	structs_api->push_array(verts, STDX_LITERAL_PTR(f32, 23));
	structs_api->push_array(verts, STDX_LITERAL_PTR(f32, 420));
	structs_api->push_array(verts, STDX_LITERAL_PTR(f32, 123321));

	STDX_FORI(0, 3, 1) { printf("verts[%d] = %0.1f\n", i, verts[i]); }

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
	STDX_FOR(u32, a, 0, vhead.count+1, 1) { printf("verts[%d] = %0.1f\n", a, verts[a]); }

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

	Array_Head larr2head = structs_api->get_array_head(larr2->array);
	printf("larr2 count %d\n", larr2head.count);

	Hash_Array* hash_array = structs_api->create_hash_array(4);
	structs_api->put_hash_array(hash_array, "age", 	&(u32){1121});
	structs_api->put_hash_array(hash_array, "something", 	&(u32){666420999});
	printf("Hash Array Resized %d times!\n", hash_array->meta.max/4);
	structs_api->put_hash_array(hash_array, "send help", 	"AHHHHHH");
	structs_api->put_hash_array(hash_array, "something else", 	&(u32){123321});
	printf("Hash Array Resized %d times!\n", hash_array->meta.max/4);
	structs_api->put_hash_array(hash_array, "year", 	&(u32){999});
	structs_api->put_hash_array(hash_array, "version", 	&(u32){222});
	

	printf("(get) age: %d\n", *(i32*)structs_api->get_hash_array(hash_array, "age"));
	printf("(get) something: %d\n", *(i32*)structs_api->get_hash_array(hash_array, "something"));
	printf("(get) send help: %s\n", (cstr)structs_api->get_hash_array(hash_array, "send help"));
	printf("(get) something else: %d\n", *(i32*)structs_api->get_hash_array(hash_array, "something else"));
	printf("(get) year: %d\n", *(i32*)structs_api->get_hash_array(hash_array, "year"));
	printf("(get) version: %d\n", *(i32*)structs_api->get_hash_array(hash_array, "version"));

	Key_Value version_kv;
	if (structs_api->pull_hash_array(hash_array, "version", &version_kv)) 
		printf("(pull) %s: %d\n", version_kv.key, *(u32*)version_kv.value);

	structs_api->destroy_hash_array(hash_array);

	stdx_cleanup_structs();
	printf("OK!\n");
}
