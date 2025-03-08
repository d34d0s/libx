#include "../include/stdx_ecs.h"
#include "../include/stdx_memory.h"
#include "../include/stdx_structs.h"

_stdlibx_ecs_api* ecs_api = NULL;

Entity _create_entity_impl(void) {
    Entity entity;

    Array_Head qhead = structs_api->get_array_head(ecs_api->entity_manager.queue);
    if (qhead.count > 0) {
        structs_api->pop_array(ecs_api->entity_manager.queue, &entity);
    } else {
        return -1;  // error: array exhausted!
    }
    qhead = structs_api->get_array_head(ecs_api->entity_manager.queue);
    
    ecs_api->entity_manager.mask[entity] = 0;
    ecs_api->entity_manager.count++;
    return entity;
}

void _destroy_entity_impl(Entity entity) {
    if (entity >= ENTITY_MAX || entity < 0) return;
    
    structs_api->push_array(ecs_api->entity_manager.queue, &(Entity){entity});
    
    ecs_api->entity_manager.mask[entity] = 0;
    ecs_api->entity_manager.count--;
}


u8 _register_component_impl(Component_Id id, u64 stride) {
    if (id < 0 || id > COMPONENT_MAX || !stride) return STDX_FALSE; // error: value error!

    ecs_api->component_manager.components[id] = structs_api->create_array(stride, ENTITY_MAX);
    if (!ecs_api->component_manager.components[id]) return STDX_FALSE;    // error: out of memory!

    ecs_api->component_manager.count++;
    return STDX_TRUE;
}

u8 _add_component_impl(Component_Id id, Entity entity) {
    if (entity >= ENTITY_MAX || entity < 0) return STDX_FALSE;  // error: value error!
    if (!ecs_api->component_manager.components[id]) return STDX_FALSE;  // error: component not registered!
    if ((ecs_api->entity_manager.mask[entity] & (1<<id)) == (1<<id)) return STDX_FALSE; // error: entity already has this component!

    Array_Head head = structs_api->get_array_head(ecs_api->component_manager.components[id]);
    structs_api->put_array(ecs_api->component_manager.components[id], entity, memory_api->alloc(head.stride, 16));

    ecs_api->entity_manager.mask[entity] |= (1 << id);
    return STDX_TRUE;
}

u8 _rem_component_impl(Component_Id id, Entity entity) {
    if (entity >= ENTITY_MAX || entity < 0) return STDX_FALSE;  // error: value error!
    if (!ecs_api->component_manager.components[id]) return STDX_FALSE;  // error: component not registered!
    if ((ecs_api->entity_manager.mask[entity] & (1<<id)) != (1<<id)) return STDX_FALSE; // error: entity does not have this component!

    ecs_api->entity_manager.mask[entity] &= ~(1<<id);

    return STDX_TRUE;
}

Component _get_component_impl(Component_Id id, Entity entity) {
    if (entity >= ENTITY_MAX || entity < 0) return STDX_FALSE;  // error: value error!
    if (!ecs_api->component_manager.components[id]) return STDX_FALSE;  // error: component not registered!
    if ((ecs_api->entity_manager.mask[entity] & (1<<id)) == (1<<id)) return STDX_FALSE; // error: entity already has this component!

    return ecs_api->component_manager.components[id][entity];
}

Entity* _get_entities_impl(Component_Id id) {

}


u8 stdx_init_ecs(void) {
    ecs_api = memory_api->alloc(sizeof(_stdlibx_ecs_api), 16);
    if (!ecs_api) return STDX_FALSE;  // error: out of memory!
    
    ecs_api->entity_manager.queue = structs_api->create_array(sizeof(Entity), ENTITY_MAX);
    if (!ecs_api->entity_manager.queue) {
        memory_api->dealloc(ecs_api);
        return STDX_FALSE;  // error: out of memory!
    }
    
    ecs_api->entity_manager.mask = structs_api->create_array(sizeof(Entity), ENTITY_MAX);
    if (!ecs_api->entity_manager.mask) {
        structs_api->destroy_array(ecs_api->entity_manager.queue);
        memory_api->dealloc(ecs_api);
        return STDX_FALSE;  // error: out of memory!
    }
    
    STDX_FORI(0, ENTITY_MAX, 1) {
        structs_api->push_array(ecs_api->entity_manager.queue, &(Entity){i});
    }
    ecs_api->entity_manager.count = 0;
    
    ecs_api->component_manager.components = structs_api->create_array(sizeof(void*), COMPONENT_MAX);
    if (!ecs_api->component_manager.components) {
        structs_api->destroy_array(ecs_api->entity_manager.queue);
        structs_api->destroy_array(ecs_api->entity_manager.mask);
        memory_api->dealloc(ecs_api);
        return STDX_FALSE;
    }
    ecs_api->component_manager.count = 0;

    ecs_api->create_entity = _create_entity_impl;
    ecs_api->destroy_entity = _destroy_entity_impl;

    ecs_api->register_component = _register_component_impl;
    ecs_api->add_component = _add_component_impl;
    ecs_api->get_component = _get_component_impl;
    ecs_api->rem_component = _rem_component_impl;
    
    return STDX_TRUE;
}

void stdx_cleanup_ecs(void) {
    structs_api->destroy_array(ecs_api->entity_manager.mask);
    structs_api->destroy_array(ecs_api->entity_manager.queue);
    
    STDX_FORI(0, ecs_api->component_manager.count, 1)
        structs_api->destroy_array(ecs_api->component_manager.components[i]);
    structs_api->destroy_array(ecs_api->component_manager.components);

    memory_api->dealloc(ecs_api);
}
