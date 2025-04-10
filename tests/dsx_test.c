#include <libx/libx.h>

void hash_array_test(void) {
	Hash_Array* h = libx->dsx.array.create_hash_array(32);
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
		if (!libx->dsx.array.put_hash_array(h, kvs[i].key, kvs[i].value)) {
			printf("failed to put hash array key: (iter)%d (key)%s\n", i, kvs[i].key);
			libx->dsx.array.destroy_hash_array(h);
			return;
		}
	}
	
	f32 shine = *(f32*)libx->dsx.array.get_hash_array(h, "u_material.shine");
	printf("default shine: %0.1f\n", shine);
	
	libx->dsx.array.put_hash_array(h, "u_material.shine", LIBX_LITERAL_PTR(f32, 2222.1111));
	shine = *(f32*)libx->dsx.array.get_hash_array(h, "u_material.shine");
	printf("updated(1) shine: %0.1f\n", shine);

	libx->dsx.array.put_hash_array(h, "u_material.shine", LIBX_LITERAL_PTR(f32, 69.69));
	shine = *(f32*)libx->dsx.array.get_hash_array(h, "u_material.shine");
	printf("updated(2) shine: %0.1f\n", shine);

	cstr* all_keys = libx->dsx.array.get_hash_array_keys(h);
	printf("all keys (%d)\n", h->meta.count);
	LIBX_FORI(0, h->meta.count, 1) {
		printf("| (%d) %s | ", i, all_keys[i]);
	}
	printf("\n");
	libx->dsx.array.destroy_array(all_keys);
}

void main() {
	libx_init(LIBX_MEMX|LIBX_MATHX|LIBX_DSX);

	f32* verts = libx->dsx.array.create_array(sizeof(f32), 3);

	libx->dsx.array.push_array(verts, LIBX_LITERAL_PTR(f32, 23));
	libx->dsx.array.push_array(verts, LIBX_LITERAL_PTR(f32, 420));
	libx->dsx.array.push_array(verts, LIBX_LITERAL_PTR(f32, 123321));

	Array_Head vhead = libx->dsx.array.get_array_head(verts);
	LIBX_FOR(u32, a, 0, vhead.max, 1) { printf("verts[%d] = %0.1f\n", a, verts[a]); }

	// never happens as cap has been reached, index 2 is not overwritten.
	libx->dsx.array.push_array(verts, LIBX_LITERAL_PTR(f32, 23));

	f32 out_vert;
	libx->dsx.array.pull_array(verts, 1, &out_vert);
	printf("verts[1] out = %0.1f\n", out_vert);

	vhead = libx->dsx.array.get_array_head(verts);
	printf("size: %d\n", vhead.size);
	printf("stride: %d\n", vhead.stride);
	printf("count: %d\n", vhead.count);
	printf("max: %d\n", vhead.max);

	verts = libx->dsx.array.resize_array(verts, 6);
	if (!verts) printf("failed to resize verts array!\n");
	else printf("resized verts array!\n");
	
	// array headers are automatically updated when using the API.
	vhead = libx->dsx.array.get_array_head(verts);
	printf("size: %d\n", vhead.size);
	printf("stride: %d\n", vhead.stride);
	printf("count: %d\n", vhead.count);
	printf("max: %d\n", vhead.max);
	
	// data persists after resizing
	LIBX_FOR(u32, a, 0, vhead.max, 1) { printf("verts[%d] = %0.1f\n", a, verts[a]); }
	
	libx->dsx.array.destroy_array(verts);

	Linked_Array* larr = libx->dsx.array.create_linked_array(NULL, sizeof(f32), 1);
	libx->dsx.array.put_array(larr->array, 0, &(f32){420.666});
	
	Linked_Array* larr2 = libx->dsx.array.create_linked_array(larr, sizeof(f32), 2);
	libx->dsx.array.put_array(larr2->array, 0, &(f32){123.321});
	libx->dsx.array.put_array(larr2->array, 1, &(f32){100.001});
	
	// link 3 new arrays in front of larr2
	Linked_Array* larlibx = libx->dsx.array.create_linked_array(larr, sizeof(f32), 3);
	Linked_Array* larr4 = libx->dsx.array.create_linked_array(larr, sizeof(f32), 4);
	Linked_Array* larr5 = libx->dsx.array.create_linked_array(larr, sizeof(f32), 5);
	libx->dsx.array.put_array(larr5->array, 2, &(f32){69.96});

	printf("larr->array[0]: %0.3f\n", *(f32*)larr->array);
	printf("larr2->array[0]: %0.3f\n", ((f32*)larr2->array)[0]);
	printf("larr2->array[1]: %0.3f\n", ((f32*)larr2->array)[1]);
	
	// remove a link from the middle of the linked structure
	libx->dsx.array.destroy_linked_array(larr4);

	// retrieve larr5 data from larr2 2 links behind as larr4 was destroyed
	printf("larr5->array[2]: %0.3f\n", ((f32*)larr2->last->last->array)[2]);
	
	// collapse the whole array
	libx->dsx.array.collapse_linked_array(larlibx);

	Array_Head larr2head = libx->dsx.array.get_array_head(larr2->array);
	printf("larr2 count %d\n", larr2head.count);

	hash_array_test();

	if (libx_cleanup()) printf("Dsx Test Ran!\n");
}
