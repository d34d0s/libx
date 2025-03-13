#include "../stdlibx/include/stdx_ecs.h"

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
    return STDX_TRUE;
}

u8 rem_component(u32 entity) {
    storage.tag[entity] = 0;
    storage.ivalue[entity] = 0;
    storage.fvalue[entity] = 0;
    return STDX_TRUE;
}

u8 get_component(u32 entity, void* component) {
    if (entity < 0 || entity > ENTITY_MAX || !component) return STDX_FALSE;
    *(my_component*)component = (my_component){
        .tag = storage.tag[entity],
        .ivalue = storage.ivalue[entity],
        .fvalue = storage.fvalue[entity]
    };
    return STDX_TRUE;
}

int main() {
    stdx_init_ecs();

    // register a component
    if (ecs_api->register_component(0, &storage, add_component, rem_component, get_component)) {
        printf("Component Registered!\n");
    }

    // create an entity
    u32 entity = ecs_api->create_entity();
    
    // add our component
    if (ecs_api->add_component(0, entity)) {
        printf("Added Component: %d To Entity: %d\n", 0, entity);
    }

    // since the component storage exists on the caller side...
    // the caller can manipulate entity-component data
    storage.ivalue[entity] = 123321;

    my_component component;
    if (ecs_api->get_component(0, entity, &component)) {
        printf("Retrieved Component!\n");
        printf("Tag: %s\n", component.tag);
        printf("Ivalue: %d\n", component.ivalue);
        printf("Fvalue: %0.1f\n", component.fvalue);
    }
    
    // create an entity without our component
    u32 e = ecs_api->create_entity();

    // create 8 more entities with our component
    STDX_FORI(0, 8, 1) {
        u32 e = ecs_api->create_entity();
        ecs_api->add_component(0, e);
    }

    // we should have 10 entities
    _STATIC_ASSERT(ecs_api->entity_manager.count == 10);
    
    // retrieve an array containing all entities with our component
    u32* buffer = ecs_api->get_entities(0);
    STDX_FORI(0, ecs_api->entity_manager.count, 1) {
        u32 e = buffer[i];
        if (e == ENTITY_MAX) continue;    // skip sentinnel values
        printf("Entity Has My Component: %d\n", e);
    }
    structs_api->destroy_array(buffer);
        
    ecs_api->unregister_component(0);
    stdx_cleanup_ecs();
    return 0;
}
