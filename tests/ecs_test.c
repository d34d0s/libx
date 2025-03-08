#include "../stdlibx/include/stdx_ecs.h"
#include "../stdlibx/include/stdx_memory.h"
#include "../stdlibx/include/stdx_structs.h"

typedef struct my_component {
    str tag;
    i32 ivalue;
    f32 fvalue;
} my_component;

int main() {
    stdx_init_memory();
    stdx_init_structs();
    stdx_init_ecs();
    
    Entity e0 = ecs_api->create_entity();
    printf("my entity: %d\n", e0);
    ecs_api->destroy_entity(e0);
    e0 = ecs_api->create_entity();
    printf("my entity again: %d\n", e0);
    
    if (ecs_api->register_component(0, sizeof(my_component))) {
        printf("registered my component!\n");
    }

    if (ecs_api->add_component(0, e0)) {
        printf("added my_component to e0!\n");
    }

    my_component* comp = ecs_api->get_component(0, e0);
    if (comp == 0) {
        comp->tag = "my tag!";
        comp->ivalue = 420666;
        comp->fvalue = 6969.0;
        printf("my_component memory address retrieved!\n");
        printf("my_component data: %s %d %0.1f\n", comp->tag, comp->ivalue, comp->fvalue);
    }

    stdx_cleanup_ecs();
    stdx_cleanup_structs();
    stdx_cleanup_memory();
    return 0;
}
