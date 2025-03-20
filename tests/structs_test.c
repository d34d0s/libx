#include "../libx/include/libx_memory.h"
#include "../libx/include/libx_structs.h"

void main() {
	libx_init_memory();
	libx_init_structs();

	f32* verts = structx->create_array(sizeof(f32), 3);

	structx->push_array(verts, LIBX_LITERAL_PTR(f32, 23));
	structx->push_array(verts, LIBX_LITERAL_PTR(f32, 420));
	structx->push_array(verts, LIBX_LITERAL_PTR(f32, 123321));

	LIBX_FORI(0, 3, 1) { printf("verts[%d] = %0.1f\n", i, verts[i]); }

	// never happens as cap has been reached, index 2 is not overwritten.
	structx->push_array(verts, LIBX_LITERAL_PTR(f32, 23));
	printf("verts[2] = %0.1f\n", verts[2]);

	f32 out_vert;
	structx->pull_array(verts, 1, &out_vert);
	printf("verts[1] out = %0.1f\n", out_vert);

	Array_Head vhead = structx->get_array_head(verts);
	printf("size: %d\n", vhead.size);
	printf("stride: %d\n", vhead.stride);
	printf("count: %d\n", vhead.count);
	printf("max: %d\n", vhead.max);

	verts = structx->resize_array(verts, 6);
	
	// data persists after resizin g
	LIBX_FOR(u32, a, 0, vhead.count+1, 1) { printf("verts[%d] = %0.1f\n", a, verts[a]); }

	// array headers are automatically updated when using the API.
	vhead = structx->get_array_head(verts);
	printf("size: %d\n", vhead.size);
	printf("stride: %d\n", vhead.stride);
	printf("count: %d\n", vhead.count);
	printf("max: %d\n", vhead.max);
	
	structx->destroy_array(verts);

	Linked_Array* larr = structx->create_linked_array(NULL, sizeof(f32), 1);
	structx->put_array(larr->array, 0, &(f32){420.666});
	
	Linked_Array* larr2 = structx->create_linked_array(larr, sizeof(f32), 2);
	structx->put_array(larr2->array, 0, &(f32){123.321});
	structx->put_array(larr2->array, 1, &(f32){100.001});
	
	// link 3 new arrays in front of larr2
	Linked_Array* larlibx = structx->create_linked_array(larr, sizeof(f32), 3);
	Linked_Array* larr4 = structx->create_linked_array(larr, sizeof(f32), 4);
	Linked_Array* larr5 = structx->create_linked_array(larr, sizeof(f32), 5);
	structx->put_array(larr5->array, 2, &(f32){69.96});

	printf("larr->array[0]: %0.3f\n", *(f32*)larr->array);
	printf("larr2->array[0]: %0.3f\n", ((f32*)larr2->array)[0]);
	printf("larr2->array[1]: %0.3f\n", ((f32*)larr2->array)[1]);
	
	// remove a link from the middle of the linked structure
	structx->destroy_linked_array(larr4);

	// retrieve larr5 data from larr2 2 links behind as larr4 was destroyed
	printf("larr5->array[2]: %0.3f\n", ((f32*)larr2->last->last->array)[2]);
	
	// collapse the whole array
	structx->collapse_linked_array(larlibx);

	Array_Head larr2head = structx->get_array_head(larr2->array);
	printf("larr2 count %d\n", larr2head.count);

	Hash_Array* hash_array = structx->create_hash_array(4);
	structx->put_hash_array(hash_array, "age", 	&(u32){1121});
	structx->put_hash_array(hash_array, "something", 	&(u32){666420999});
	printf("Hash Array Resized %d times!\n", hash_array->meta.max/4);
	structx->put_hash_array(hash_array, "send help", 	"AHHHHHH");
	structx->put_hash_array(hash_array, "something else", 	&(u32){123321});
	printf("Hash Array Resized %d times!\n", hash_array->meta.max/4);
	structx->put_hash_array(hash_array, "year", 	&(u32){999});
	structx->put_hash_array(hash_array, "version", 	&(u32){222});
	

	printf("(get) age: %d\n", *(i32*)structx->get_hash_array(hash_array, "age"));
	printf("(get) something: %d\n", *(i32*)structx->get_hash_array(hash_array, "something"));
	printf("(get) send help: %s\n", (cstr)structx->get_hash_array(hash_array, "send help"));
	printf("(get) something else: %d\n", *(i32*)structx->get_hash_array(hash_array, "something else"));
	printf("(get) year: %d\n", *(i32*)structx->get_hash_array(hash_array, "year"));
	printf("(get) version: %d\n", *(i32*)structx->get_hash_array(hash_array, "version"));

	Key_Value version_kv;
	if (structx->pull_hash_array(hash_array, "version", &version_kv)) 
		printf("(pull) %s: %d\n", version_kv.key, *(u32*)version_kv.value);

	structx->destroy_hash_array(hash_array);

	libx_cleanup_structs();
	printf("OK!\n");
}
