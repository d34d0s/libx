#include "../include/stdx_ecs.h"

_stdlibx_ecs_api* ecs_api = NULL;
typedef u8 (*_add_component_fptr)(u32 entity);
typedef u8 (*_rem_component_fptr)(u32 entity);
typedef u8 (*_get_component_fptr)(u32 entity, void* component);

u32 _create_entity_impl(void) {
    u32 entity;

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

void _destroy_entity_impl(u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return;
    
    structs_api->push_array(ecs_api->entity_manager.queue, &(u32){entity});
    
    ecs_api->entity_manager.mask[entity] = 0;
    ecs_api->entity_manager.count--;
}


u8 _register_component_impl(
    u8 id, void* storage, void* add_func,
    void* rem_func, void* get_func) {
    if (id < 0 || id > COMPONENT_MAX || !storage ||
        !add_func || !rem_func || !get_func
    ) return STDX_FALSE; // error: value error!
    
    ecs_api->component_manager.component_storage[id] = storage;
    ecs_api->component_manager.add_component_fptr[id] = add_func;
    ecs_api->component_manager.rem_component_fptr[id] = rem_func;
    ecs_api->component_manager.get_component_fptr[id] = get_func;
    
    ecs_api->component_manager.count++;
    return STDX_TRUE;
}

u8 _unregister_component_impl(u8 id) {
    if (id < 0 || id > COMPONENT_MAX) return STDX_FALSE; // error: value error!
    if (!ecs_api->component_manager.component_storage[id]) return STDX_FALSE;  // error: component not registered!

    // TODO: remove this component from all entities with it
    ecs_api->component_manager.component_storage[id] = NULL;
    ecs_api->component_manager.add_component_fptr[id] = NULL;
    ecs_api->component_manager.rem_component_fptr[id] = NULL;
    ecs_api->component_manager.get_component_fptr[id] = NULL;

    ecs_api->component_manager.count--;
    return STDX_TRUE;
}


u8 _add_component_impl(u8 id, u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return STDX_FALSE;  // error: value error!
    if ((ecs_api->entity_manager.mask[entity] & (1<<id)) == (1<<id)) return STDX_FALSE; // error: entity already has this component!
    
    u8 result = ((_add_component_fptr)ecs_api->component_manager.add_component_fptr[id])(entity);
    ecs_api->entity_manager.mask[entity] |= (1 << id);
    return result;
}

u8 _rem_component_impl(u8 id, u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return STDX_FALSE;  // error: value error!
    if ((ecs_api->entity_manager.mask[entity] & (1<<id)) != (1<<id)) return STDX_FALSE; // error: entity does not have this component!

    u8 result = ((_rem_component_fptr)ecs_api->component_manager.rem_component_fptr[id])(entity);
    ecs_api->entity_manager.mask[entity] &= ~(1<<id);
    return result;
}

u8 _get_component_impl(u8 id, u32 entity, void* component) {
    if (entity >= ENTITY_MAX || entity < 0) return STDX_FALSE;  // error: value error!
    if ((ecs_api->entity_manager.mask[entity] & (1<<id)) != (1<<id)) return STDX_FALSE;

    return ((_get_component_fptr)ecs_api->component_manager.get_component_fptr[id])(entity, component);
}

u32* _get_entities_impl(u8 id) {
    u32* out = structs_api->create_array(sizeof(u32), ecs_api->entity_manager.count);

    STDX_FORI(0, ecs_api->entity_manager.count, 1) {
        structs_api->push_array(out, ((ecs_api->entity_manager.mask[i] & (1<<id)) == (1<<id)) ? &(u32){i} : &(u32){ENTITY_MAX});
    }

    return out;
}


u8 stdx_init_ecs(void) {
    if (!stdx_init_memory()) return STDX_FALSE; // error: failed to initialize memory api!
    if (!stdx_init_structs()) return STDX_FALSE;    // error: failed to initialize structs api!

    ecs_api = memory_api->alloc(sizeof(_stdlibx_ecs_api), 16);
    if (!ecs_api) return STDX_FALSE;  // error: out of memory!
    
    ecs_api->entity_manager.queue = structs_api->create_array(sizeof(u32), ENTITY_MAX);
    if (!ecs_api->entity_manager.queue) {
        memory_api->dealloc(ecs_api);
        return STDX_FALSE;  // error: out of memory!
    }
    
    ecs_api->entity_manager.mask = structs_api->create_array(sizeof(u32), ENTITY_MAX);
    if (!ecs_api->entity_manager.mask) {
        structs_api->destroy_array(ecs_api->entity_manager.queue);
        memory_api->dealloc(ecs_api);
        return STDX_FALSE;  // error: out of memory!
    }
    
    STDX_FORI(ENTITY_MAX-1, -1, -1) {
        structs_api->push_array(ecs_api->entity_manager.queue, &(u32){i});
    }
    ecs_api->entity_manager.count = 0;
    
    ecs_api->component_manager.component_storage = structs_api->create_array(sizeof(void*), COMPONENT_MAX);
    if (!ecs_api->component_manager.component_storage) {
        structs_api->destroy_array(ecs_api->entity_manager.queue);
        structs_api->destroy_array(ecs_api->entity_manager.mask);
        memory_api->dealloc(ecs_api);
        return STDX_FALSE;  // error: out of memory!
    }
    ecs_api->component_manager.add_component_fptr = structs_api->create_array(sizeof(void*), COMPONENT_MAX);
    if (!ecs_api->component_manager.add_component_fptr) {
        structs_api->destroy_array(ecs_api->component_manager.component_storage);
        structs_api->destroy_array(ecs_api->entity_manager.queue);
        structs_api->destroy_array(ecs_api->entity_manager.mask);
        memory_api->dealloc(ecs_api);
        return STDX_FALSE;  // error: out of memory!
    }
    
    ecs_api->component_manager.rem_component_fptr = structs_api->create_array(sizeof(void*), COMPONENT_MAX);
    if (!ecs_api->component_manager.rem_component_fptr) {
        structs_api->destroy_array(ecs_api->component_manager.component_storage);
        structs_api->destroy_array(ecs_api->component_manager.add_component_fptr);
        structs_api->destroy_array(ecs_api->entity_manager.queue);
        structs_api->destroy_array(ecs_api->entity_manager.mask);
        memory_api->dealloc(ecs_api);
        return STDX_FALSE;  // error: out of memory!
    }
    
    ecs_api->component_manager.get_component_fptr = structs_api->create_array(sizeof(void*), COMPONENT_MAX);
    if (!ecs_api->component_manager.get_component_fptr) {
        structs_api->destroy_array(ecs_api->component_manager.component_storage);
        structs_api->destroy_array(ecs_api->component_manager.add_component_fptr);
        structs_api->destroy_array(ecs_api->component_manager.rem_component_fptr);
        structs_api->destroy_array(ecs_api->entity_manager.queue);
        structs_api->destroy_array(ecs_api->entity_manager.mask);
        memory_api->dealloc(ecs_api);
        return STDX_FALSE;  // error: out of memory!
    }
    
    ecs_api->component_manager.count = 0;

    ecs_api->create_entity = _create_entity_impl;
    ecs_api->destroy_entity = _destroy_entity_impl;

    ecs_api->register_component = _register_component_impl;
    ecs_api->unregister_component = _unregister_component_impl;
    
    ecs_api->add_component = _add_component_impl;
    ecs_api->get_component = _get_component_impl;
    ecs_api->rem_component = _rem_component_impl;
    ecs_api->get_entities = _get_entities_impl;
    
    return STDX_TRUE;
}

void stdx_cleanup_ecs(void) {
    structs_api->destroy_array(ecs_api->entity_manager.mask);
    structs_api->destroy_array(ecs_api->entity_manager.queue);

    structs_api->destroy_array(ecs_api->component_manager.component_storage);
    structs_api->destroy_array(ecs_api->component_manager.add_component_fptr);
    structs_api->destroy_array(ecs_api->component_manager.rem_component_fptr);
    structs_api->destroy_array(ecs_api->component_manager.get_component_fptr);

    memory_api->dealloc(ecs_api);
    stdx_cleanup_structs();
    stdx_cleanup_memory();
}
