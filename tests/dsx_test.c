#include <corex/corex.h>

void hash_array_test(void) {
	Hash_Array* h = corex->dsx.array.create_hash_array(32);
	if (!h) {
		printf("failed to create hash array!\n");
		return;
	}
	
	Key_Value kvs[] = {
		(Key_Value){.key = "u_model", .value = COREX_LITERAL_PTR(f32, 420.0)},
		(Key_Value){.key = "u_view", .value = COREX_LITERAL_PTR(f32, 666.0)},
		(Key_Value){.key = "u_proj", .value = COREX_LITERAL_PTR(f32, 123.0)},
		
		(Key_Value){.key = "u_light.ambient", .value = COREX_LITERAL_PTR(f32, 123.0)},
		(Key_Value){.key = "u_light.diffuse", .value = COREX_LITERAL_PTR(f32, 123.0)},
		(Key_Value){.key = "u_light.specular", .value = COREX_LITERAL_PTR(f32, 123.0)},
		(Key_Value){.key = "u_light.location", .value = COREX_LITERAL_PTR(f32, 123.0)},
		
		(Key_Value){.key = "u_material.shine", .value = COREX_LITERAL_PTR(f32, 123.0)},
		(Key_Value){.key = "u_material.ambient", .value = COREX_LITERAL_PTR(f32, 123.0)},
		(Key_Value){.key = "u_material.diffuse", .value = COREX_LITERAL_PTR(f32, 123.0)},
		(Key_Value){.key = "u_material.specular", .value = COREX_LITERAL_PTR(f32, 123.0)},
	};
	int keys = (int)(sizeof(kvs) / sizeof(Key_Value));

	COREX_FORI(0, keys, 1) {
		if (!corex->dsx.array.put_hash_array(h, kvs[i].key, kvs[i].value)) {
			printf("failed to put hash array key: (iter)%d (key)%s\n", i, kvs[i].key);
			corex->dsx.array.destroy_hash_array(h);
			return;
		}
	}
	
	f32 shine = *(f32*)corex->dsx.array.get_hash_array(h, "u_material.shine");
	printf("default shine: %0.1f\n", shine);
	
	corex->dsx.array.put_hash_array(h, "u_material.shine", COREX_LITERAL_PTR(f32, 2222.1111));
	shine = *(f32*)corex->dsx.array.get_hash_array(h, "u_material.shine");
	printf("updated(1) shine: %0.1f\n", shine);

	corex->dsx.array.put_hash_array(h, "u_material.shine", COREX_LITERAL_PTR(f32, 69.69));
	shine = *(f32*)corex->dsx.array.get_hash_array(h, "u_material.shine");
	printf("updated(2) shine: %0.1f\n", shine);

	cstr* all_keys = corex->dsx.array.get_hash_array_keys(h);
	printf("all keys (%d)\n", h->meta.count);
	COREX_FORI(0, h->meta.count, 1) {
		printf("| (%d) %s | ", i, all_keys[i]);
	}
	printf("\n");
	corex->dsx.array.destroy_array(all_keys);
}

void main() {
	corex_init(COREX_MEMX|COREX_MATHX|COREX_DSX);

	f32* verts = corex->dsx.array.create_array(sizeof(f32), 3);

	corex->dsx.array.push_array(verts, COREX_LITERAL_PTR(f32, 23));
	corex->dsx.array.push_array(verts, COREX_LITERAL_PTR(f32, 420));
	corex->dsx.array.push_array(verts, COREX_LITERAL_PTR(f32, 123321));

	Array_Head vhead = corex->dsx.array.get_array_head(verts);
	COREX_FOR(u32, a, 0, vhead.max, 1) { printf("verts[%d] = %0.1f\n", a, verts[a]); }

	// never happens as cap has been reached, index 2 is not overwritten.
	corex->dsx.array.push_array(verts, COREX_LITERAL_PTR(f32, 23));

	f32 out_vert;
	corex->dsx.array.pull_array(verts, 1, &out_vert);
	printf("verts[1] out = %0.1f\n", out_vert);

	vhead = corex->dsx.array.get_array_head(verts);
	printf("size: %d\n", vhead.size);
	printf("stride: %d\n", vhead.stride);
	printf("count: %d\n", vhead.count);
	printf("max: %d\n", vhead.max);

	verts = corex->dsx.array.resize_array(verts, 6);
	if (!verts) printf("failed to resize verts array!\n");
	else printf("resized verts array!\n");
	
	// array headers are automatically updated when using the API.
	vhead = corex->dsx.array.get_array_head(verts);
	printf("size: %d\n", vhead.size);
	printf("stride: %d\n", vhead.stride);
	printf("count: %d\n", vhead.count);
	printf("max: %d\n", vhead.max);
	
	// data persists after resizing
	COREX_FOR(u32, a, 0, vhead.max, 1) { printf("verts[%d] = %0.1f\n", a, verts[a]); }
	
	corex->dsx.array.destroy_array(verts);

	Linked_Array* larr = corex->dsx.array.create_linked_array(NULL, sizeof(f32), 1);
	corex->dsx.array.put_array(larr->array, 0, &(f32){420.666});
	
	Linked_Array* larr2 = corex->dsx.array.create_linked_array(larr, sizeof(f32), 2);
	corex->dsx.array.put_array(larr2->array, 0, &(f32){123.321});
	corex->dsx.array.put_array(larr2->array, 1, &(f32){100.001});
	
	// link 3 new arrays in front of larr2
	Linked_Array* larcorex = corex->dsx.array.create_linked_array(larr, sizeof(f32), 3);
	Linked_Array* larr4 = corex->dsx.array.create_linked_array(larr, sizeof(f32), 4);
	Linked_Array* larr5 = corex->dsx.array.create_linked_array(larr, sizeof(f32), 5);
	corex->dsx.array.put_array(larr5->array, 2, &(f32){69.96});

	printf("larr->array[0]: %0.3f\n", *(f32*)larr->array);
	printf("larr2->array[0]: %0.3f\n", ((f32*)larr2->array)[0]);
	printf("larr2->array[1]: %0.3f\n", ((f32*)larr2->array)[1]);
	
	// remove a link from the middle of the linked structure
	corex->dsx.array.destroy_linked_array(larr4);

	// retrieve larr5 data from larr2 2 links behind as larr4 was destroyed
	printf("larr5->array[2]: %0.3f\n", ((f32*)larr2->last->last->array)[2]);
	
	// collapse the whole array
	corex->dsx.array.collapse_linked_array(larcorex);

	Array_Head larr2head = corex->dsx.array.get_array_head(larr2->array);
	printf("larr2 count %d\n", larr2head.count);

	hash_array_test();

	if (corex_cleanup()) printf("Dsx Test Ran!\n");
}
