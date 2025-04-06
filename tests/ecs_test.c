#include "../libx/include/libx_ecs.h"

typedef struct my_component {
    str tag;
    i32 ivalue;
    f32 fvalue;
} my_component;

struct _storage {
    str tag[ENTITY_MAX];
    i32 ivalue[ENTITY_MAX];
    f32 fvalue[ENTITY_MAX];
} storage = {0};

u8 add_component(u32 entity) {
    storage.tag[entity] = "COMPONENT TAG";
    storage.ivalue[entity] = 0;
    storage.fvalue[entity] = 0.0f;
    return LIBX_TRUE;
}

u8 rem_component(u32 entity) {
    storage.tag[entity] = 0;
    storage.ivalue[entity] = 0;
    storage.fvalue[entity] = 0;
    return LIBX_TRUE;
}

u8 get_component(u32 entity, void* component) {
    if (entity < 0 || entity > ENTITY_MAX || !component) return LIBX_FALSE;
    *(my_component*)component = (my_component){
        .tag = storage.tag[entity],
        .ivalue = storage.ivalue[entity],
        .fvalue = storage.fvalue[entity]
    };
    return LIBX_TRUE;
}

u8 pre_component_system(u32 entity) {
    printf("(PRE) Entity In My Component System: %d\n", entity);
    return LIBX_TRUE;
}

u8 main_component_system(u32 entity) {
    printf("(MAIN) Entity In My Component System: %d\n", entity);
    return LIBX_TRUE;
}

u8 post_component_system(u32 entity) {
    printf("(POST) Entity In My Component System: %d\n", entity);
    return LIBX_TRUE;
}

int main() {
    libx_init_memory();
    libx_init_structs();
    libx_init_ecs();

    // register a component
    if (ecsx->register_component(0, &storage, add_component, rem_component, get_component)) {
        printf("Component Registered!\n");
    }

    if (
        ecsx->register_system(0, "main", main_component_system) &&
        ecsx->register_system(0, "pre", pre_component_system) &&
        ecsx->register_system(0, "post", post_component_system)
    ) {
        printf("Component System Registered!\n");
    }

    // create an entity
    u32 entity = ecsx->create_entity();
    
    // add our component
    if (ecsx->add_component(0, entity)) {
        printf("Added Component: %d To Entity: %d\n", 0, entity);
    }

    // since the component storage exists on the caller side...
    // the caller can manipulate entity-component data
    storage.ivalue[entity] = 123321;

    my_component component;
    if (ecsx->get_component(0, entity, &component)) {
        printf("Retrieved Component!\n");
        printf("Tag: %s\n", component.tag);
        printf("Ivalue: %d\n", component.ivalue);
        printf("Fvalue: %0.1f\n", component.fvalue);
    }
    
    // create an entity without our component
    u32 e = ecsx->create_entity();

    // create 8 more entities with our component
    LIBX_FORI(0, 8, 1) {
        u32 e = ecsx->create_entity();
        ecsx->add_component(0, e);
    }

    // we should have 10 entities
    if (ecsx->entity_manager.count == 10) printf("10 Entities Created!\n");
    
    // retrieve an array containing all entities with our component
    u32* buffer = ecsx->get_entities(0);
    LIBX_FORI(0, ecsx->entity_manager.count, 1) {
        u32 e = buffer[i];
        if (e == ENTITY_MAX) continue;    // skip sentinnel values
        printf("Entity Has My Component: %d\n", e);
    }
    structx->array.destroy_array(buffer);

    if (ecsx->run_system(0, "main")) printf("Component System Ran!\n");
    if (ecsx->run_systems(0)) printf("All Component Systems Ran!\n");
        
    if (ecsx->unregister_systems(0)) printf("All Component Systems Unregistered!\n");
    if (ecsx->unregister_component(0)) printf("Component Unregistered!\n");

    libx_cleanup_ecs();
    libx_cleanup_structs();
    libx_cleanup_memory();
    return 0;
}
