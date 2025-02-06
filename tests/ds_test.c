#include "../stdlibx/include/stdx_ds.h"

void main() {
	stdx_init_data_structs();

	f32* verts = data_structs->create_array(sizeof(f32), 3);

	f32 age = 23.0f;
	f32 age2 = 420.0f;
	f32 age3 = 123321.0f;
	data_structs->push_array(verts, &age);
	printf("verts[0] = %0.1f\n", verts[0]);
	
	data_structs->push_array(verts, &age2);
	printf("verts[1] = %0.1f\n", verts[1]);
	
	data_structs->push_array(verts, &age3);
	printf("verts[2] = %0.1f\n", verts[2]);
	
	// never happens as cap has been reached, index 2 is not overwritten.
	data_structs->push_array(verts, &age);
	printf("verts[2] = %0.1f\n", verts[2]);

	Stdx_Array_Head vhead = data_structs->get_array_head(verts);
	printf("size: %d\n", vhead.size);
	printf("stride: %d\n", vhead.stride);
	printf("count: %d\n", vhead.count);
	printf("capacity: %d\n", vhead.capacity);

	verts = data_structs->resize_array(verts, 6);

	// data persists after resizing
	printf("verts[0] = %0.1f\n", verts[0]);
	printf("verts[1] = %0.1f\n", verts[1]);
	printf("verts[2] = %0.1f\n", verts[2]);

	// array headers are automatically updated when using the API.
	vhead = data_structs->get_array_head(verts);
	printf("size: %d\n", vhead.size);
	printf("stride: %d\n", vhead.stride);
	printf("count: %d\n", vhead.count);
	printf("capacity: %d\n", vhead.capacity);
	
	data_structs->destroy_array(verts);

	stdx_cleanup_data_structs();
	printf("OK!\n");
}
