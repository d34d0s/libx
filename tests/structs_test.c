#include "../libx/include/libx_memory.h"
#include "../libx/include/libx_structs.h"

void hash_array_test(void) {
	Hash_Array* h = structx->create_hash_array(32);
	if (!h) {
		printf("failed to create hash array!\n");
		return;
	}
	
	Key_Value kvs[] = {
		(Key_Value){.key = "u_model", .value = LIBX_LITERAL_PTR(f32, 420.0)},
		(Key_Value){.key = "u_view", .value = LIBX_LITERAL_PTR(f32, 666.0)},
		(Key_Value){.key = "u_proj", .value = LIBX_LITERAL_PTR(f32, 123.0)},
		
		(Key_Value){.key = "u_light.ambient", .value = LIBX_LITERAL_PTR(f32, 123.0)},
		(Key_Value){.key = "u_light.diffuse", .value = LIBX_LITERAL_PTR(f32, 123.0)},
		(Key_Value){.key = "u_light.specular", .value = LIBX_LITERAL_PTR(f32, 123.0)},
		(Key_Value){.key = "u_light.location", .value = LIBX_LITERAL_PTR(f32, 123.0)},
		
		(Key_Value){.key = "u_material.shine", .value = LIBX_LITERAL_PTR(f32, 123.0)},
		(Key_Value){.key = "u_material.ambient", .value = LIBX_LITERAL_PTR(f32, 123.0)},
		(Key_Value){.key = "u_material.diffuse", .value = LIBX_LITERAL_PTR(f32, 123.0)},
		(Key_Value){.key = "u_material.specular", .value = LIBX_LITERAL_PTR(f32, 123.0)},
	};
	int keys = (int)(sizeof(kvs) / sizeof(Key_Value));

	LIBX_FORI(0, keys, 1) {
		if (!structx->put_hash_array(h, kvs[i].key, kvs[i].value)) {
			printf("failed to put hash array key: (iter)%d (key)%s\n", i, kvs[i].key);
			structx->destroy_hash_array(h);
			return;
		}
	}
	
	f32 shine = *(f32*)structx->get_hash_array(h, "u_material.shine");
	printf("default shine: %0.1f\n", shine);
	
	structx->put_hash_array(h, "u_material.shine", LIBX_LITERAL_PTR(f32, 2222.1111));
	shine = *(f32*)structx->get_hash_array(h, "u_material.shine");
	printf("updated(1) shine: %0.1f\n", shine);

	structx->put_hash_array(h, "u_material.shine", LIBX_LITERAL_PTR(f32, 69.69));
	shine = *(f32*)structx->get_hash_array(h, "u_material.shine");
	printf("updated(2) shine: %0.1f\n", shine);

	cstr* all_keys = structx->get_hash_array_keys(h);
	printf("all keys (%d)\n", h->meta.count);
	LIBX_FORI(0, h->meta.count, 1) {
		printf("| (%d) %s | ", i, all_keys[i]);
	}
	printf("\n");
	structx->destroy_array(all_keys);
}

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

	hash_array_test();

	libx_cleanup_structs();
	printf("OK!\n");
}
