#include "../stdlibx/include/stdx_memory.h"
#include "../stdlibx/include/stdx_structs.h"

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

	Stdx_Array_Head vhead = structs_api->get_array_head(verts);
	printf("size: %d\n", vhead.size);
	printf("stride: %d\n", vhead.stride);
	printf("count: %d\n", vhead.count);
	printf("capacity: %d\n", vhead.capacity);

	verts = structs_api->resize_array(verts, 6);
	
	// data persists after resizing
	STDX_FOR(u32, a, 0, vhead.count, 1) { printf("verts[%d] = %0.1f\n", a, verts[a]); }

	// array headers are automatically updated when using the API.
	vhead = structs_api->get_array_head(verts);
	printf("size: %d\n", vhead.size);
	printf("stride: %d\n", vhead.stride);
	printf("count: %d\n", vhead.count);
	printf("capacity: %d\n", vhead.capacity);
	
	structs_api->destroy_array(verts);

	stdx_cleanup_structs();
	printf("OK!\n");
}
