#include <corex/corex.h>

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

byte add_component(u32 entity) {
    storage.tag[entity] = "COMPONENT TAG";
    storage.ivalue[entity] = 0;
    storage.fvalue[entity] = 0.0f;
    return COREX_TRUE;
}

byte rem_component(u32 entity) {
    storage.tag[entity] = 0;
    storage.ivalue[entity] = 0;
    storage.fvalue[entity] = 0;
    return COREX_TRUE;
}

byte get_component(u32 entity, void* component) {
    if (entity < 0 || entity > ENTITY_MAX || !component) return COREX_FALSE;
    *(my_component*)component = (my_component){
        .tag = storage.tag[entity],
        .ivalue = storage.ivalue[entity],
        .fvalue = storage.fvalue[entity]
    };
    return COREX_TRUE;
}

byte pre_component_system(u32 entity) {
    printf("(PRE) Entity In My Component System: %d\n", entity);
    return COREX_TRUE;
}

byte main_component_system(u32 entity) {
    printf("(MAIN) Entity In My Component System: %d\n", entity);
    return COREX_TRUE;
}

byte post_component_system(u32 entity) {
    printf("(POST) Entity In My Component System: %d\n", entity);
    return COREX_TRUE;
}

void main() {
    corex_init(COREX_MEMX|COREX_DSX|COREX_ECSX);

    // register a component
    if (corex->ecsx.register_component(0, &storage, add_component, rem_component, get_component)) {
        printf("Component Registered!\n");
    }
    if (corex->ecsx.register_component(1, &storage, add_component, rem_component, get_component)) {
        printf("Component Registered!\n");
    }

    if (
        corex->ecsx.register_system(0, "main", main_component_system) &&
        corex->ecsx.register_system(0, "pre", pre_component_system) &&
        corex->ecsx.register_system(0, "post", post_component_system)
    ) {
        printf("Component System Registered!\n");
    }

    // create an entity with our components
    u32 entity = corex->ecsx.create_entity_with(2, (byte[]){0, 1});
    
    // since the component storage exists on the caller side...
    // the caller can manipulate entity-component data
    storage.ivalue[entity] = 123321;

    my_component component;
    if (corex->ecsx.get_component(0, entity, &component)) {
        printf("Retrieved Component!\n");
        printf("Tag: %s\n", component.tag);
        printf("Ivalue: %d\n", component.ivalue);
        printf("Fvalue: %0.1f\n", component.fvalue);
    }
    
    // create an entity without our component
    u32 e = corex->ecsx.create_entity();

    // create 8 more entities with our component
    COREX_FORI(0, 8, 1) {
        u32 e = corex->ecsx.create_entity();
        corex->ecsx.add_component(0, e);
    }

    // we should have 10 entities
    if (corex->ecsx.entity_manager.count == 10) printf("10 Entities Created!\n");
    
    // retrieve an array containing all entities with our component
    u32* buffer = corex->ecsx.get_entities(0);
    COREX_FORI(0, corex->ecsx.entity_manager.count, 1) {
        u32 e = buffer[i];
        if (e == ENTITY_MAX) continue;    // skip sentinnel values
        printf("Entity Has My Component: %d\n", e);
    }
    corex->dsx.array.destroy_array(buffer);

    if (corex->ecsx.run_system(0, "main")) printf("Component System Ran!\n");
    if (corex->ecsx.run_systems(0)) printf("All Component Systems Ran!\n");
        
    if (corex->ecsx.unregister_systems(0)) printf("All Component Systems Unregistered!\n");
    if (corex->ecsx.unregister_component(0)) printf("Component Unregistered!\n");
    if (corex->ecsx.unregister_component(1)) printf("Component Unregistered!\n");

    if (corex_cleanup()) printf("Ecsx Test Ran!\n");
}
