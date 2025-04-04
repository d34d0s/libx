#include "../libx/include/libx_memory.h"
#include "../libx/include/libx_structs.h"

void main() {
	libx_init_memory();
	libx_init_structs();

	f32* verts = structx->create_array(sizeof(f32), 3);

	structx->push_array(verts, LIBX_LITERAL_PTR(f32, 23));
	structx->push_array(verts, LIBX_LITERAL_PTR(f32, 420));
	structx->push_array(verts, LIBX_LITERAL_PTR(f32, 123321));

	Array_Head vhead = structx->get_array_head(verts);
	LIBX_FOR(u32, a, 0, vhead.max, 1) { printf("verts[%d] = %0.1f\n", a, verts[a]); }

	// never happens as cap has been reached, index 2 is not overwritten.
	structx->push_array(verts, LIBX_LITERAL_PTR(f32, 23));

	f32 out_vert;
	structx->pull_array(verts, 1, &out_vert);
	printf("verts[1] out = %0.1f\n", out_vert);

	vhead = structx->get_array_head(verts);
	printf("size: %d\n", vhead.size);
	printf("stride: %d\n", vhead.stride);
	printf("count: %d\n", vhead.count);
	printf("max: %d\n", vhead.max);

	verts = structx->resize_array(verts, 6);
	if (!verts) printf("failed to resize verts array!\n");
	else printf("resized verts array!\n");
	
	// array headers are automatically updated when using the API.
	vhead = structx->get_array_head(verts);
	printf("size: %d\n", vhead.size);
	printf("stride: %d\n", vhead.stride);
	printf("count: %d\n", vhead.count);
	printf("max: %d\n", vhead.max);
	
	// data persists after resizing
	LIBX_FOR(u32, a, 0, vhead.max, 1) { printf("verts[%d] = %0.1f\n", a, verts[a]); }
	
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
	structx->put_hash_array(hash_array, "age", 	&(i32){1121});
	structx->put_hash_array(hash_array, "something", 	&(i32){666420999});
	structx->put_hash_array(hash_array, "send help", 	"AHHHHHH");
	structx->put_hash_array(hash_array, "something else", 	&(i32){123321});

	printf("hash meta: (size)%d (stride)%d (count)%d (max)%d\n",
		hash_array->meta.size,
		hash_array->meta.stride,
		hash_array->meta.count,
		hash_array->meta.max
	);

	structx->put_hash_array(hash_array, "year", 	&(i32){999});
	structx->put_hash_array(hash_array, "version", 	&(i32){2200200});

	printf("hash meta 2: (size)%d (stride)%d (count)%d (max)%d\n",
		hash_array->meta.size,
		hash_array->meta.stride,
		hash_array->meta.count,
		hash_array->meta.max
	);
	
	i32 get_age = *(i32*)structx->get_hash_array(hash_array, "age");
	if (get_age) printf("(get) age: %d\n", get_age);
	
	i32 get_something = *(i32*)structx->get_hash_array(hash_array, "something");
	if (get_something) printf("(get) something: %d\n", get_something);
	
	str get_help = (str)structx->get_hash_array(hash_array, "send help");
	if (get_help) printf("(get) send help: %s\n",get_help);
	
	i32 get_else = *(i32*)structx->get_hash_array(hash_array, "something else");
	if (get_else) printf("(get) something else: %d\n", get_else);
	
	i32 get_year = *(i32*)structx->get_hash_array(hash_array, "year");
	if (get_year) printf("(get) year: %d\n", get_year);
	
	i32 get_version = *(i32*)structx->get_hash_array(hash_array, "version");
	if (get_version) printf("(get) version: %d\n", get_version);
	
	printf("all hash array keys (%d)\n", hash_array->meta.count);
	cstr* keys = structx->get_hash_array_keys(hash_array);
	LIBX_FORI(0, hash_array->meta.count, 1) {
		printf("%s |", keys[i]);
	}
	printf("\n");
	structx->destroy_array(keys);
	
	printf("all hash array values (%d)\n", hash_array->meta.count);
	void** values = structx->get_hash_array_values(hash_array);
	LIBX_FORI(0, hash_array->meta.count, 1) {
		if (i == 1) printf("%s |", values[i]);
		else printf("%d |", *(i32*)values[i]);
	}
	printf("\n");
	structx->destroy_array(values);
	
	Key_Value pulled;
	if (structx->pull_hash_array(hash_array, "age", &pulled)) {
		printf("pulled (key)%s | (value)%d\n", pulled.key, *(i32*)pulled.value);
	}
	printf("hash meta 3: (size)%d (stride)%d (count)%d (max)%d\n",
		hash_array->meta.size,
		hash_array->meta.stride,
		hash_array->meta.count,
		hash_array->meta.max
	);

	keys = structx->get_hash_array_keys(hash_array);
	LIBX_FORI(0, hash_array->meta.count, 1) {
		printf("%s |", keys[i]);
	}
	printf("\n");
	structx->destroy_array(keys);
	
	printf("all hash array values (%d)\n", hash_array->meta.count);
	values = structx->get_hash_array_values(hash_array);
	LIBX_FORI(0, hash_array->meta.count, 1) {
		if (i == 0) printf("%s |", values[i]);
		else printf("%d |", *(i32*)values[i]);
	}
	printf("\n");
	structx->destroy_array(values);
	
	structx->destroy_hash_array(hash_array);

	libx_cleanup_structs();
	printf("OK!\n");
}
