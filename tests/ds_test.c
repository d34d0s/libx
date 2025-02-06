#include "../stdlibx/include/stdx_ds.h"

int main() {
	u32* array = (u32*)stdx_create_array(sizeof(u32), 10);

	array[0] = 123;
	array[1] = 456;
	array[2] = 789;

	for (int i = 0; i < 3; i++) {
		printf("ARR[%d] = %d\n", i, array[i]);
	}

	Stdx_Array_Head arrhead = stdx_arrhead(array);
	printf("ARR SIZE: %d\n", arrhead.size);
	printf("ARR STRIDE: %d\n", arrhead.stride);
	printf("ARR COUNT: %d\n", arrhead.count);
	printf("ARR CAPACITY: %d\n", arrhead.capacity);

	stdx_realloc(array, (sizeof(u32) * 20), 16);

	printf("ARR SIZE: %d\n", arrhead.size);
	printf("ARR STRIDE: %d\n", arrhead.stride);
	printf("ARR COUNT: %d\n", arrhead.count);
	printf("ARR CAPACITY: %d\n", arrhead.capacity);

	stdx_destroy_array(array);

	printf("OK!\n");
	return 0;
}

