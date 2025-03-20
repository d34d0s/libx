#include "../include/libx_ecs.h"

_libx_ecs_api* ecsx = NULL;
typedef u8 (*_add_component_fptr)(u32 entity);
typedef u8 (*_rem_component_fptr)(u32 entity);
typedef u8 (*_get_component_fptr)(u32 entity, void* component);

u32 _create_entity_impl(void) {
    u32 entity;

    Array_Head qhead = structx->get_array_head(ecsx->entity_manager.queue);
    if (qhead.count > 0) {
        structx->pop_array(ecsx->entity_manager.queue, &entity);
    } else {
        return -1;  // error: array exhausted!
    }
    qhead = structx->get_array_head(ecsx->entity_manager.queue);
    
    ecsx->entity_manager.mask[entity] = 0;
    ecsx->entity_manager.count++;
    return entity;
}

void _destroy_entity_impl(u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return;
    
    structx->push_array(ecsx->entity_manager.queue, &(u32){entity});
    
    ecsx->entity_manager.mask[entity] = 0;
    ecsx->entity_manager.count--;
}


u8 _register_component_impl(
    u8 id, void* storage, void* add_func,
    void* rem_func, void* get_func) {
    if (id < 0 || id > COMPONENT_MAX || !storage ||
        !add_func || !rem_func || !get_func
    ) return LIBX_FALSE; // error: value error!
    
    ecsx->component_manager.component_storage[id] = storage;
    ecsx->component_manager.add_component_fptr[id] = add_func;
    ecsx->component_manager.rem_component_fptr[id] = rem_func;
    ecsx->component_manager.get_component_fptr[id] = get_func;
    
    ecsx->component_manager.count++;
    return LIBX_TRUE;
}

u8 _unregister_component_impl(u8 id) {
    if (id < 0 || id > COMPONENT_MAX) return LIBX_FALSE; // error: value error!
    if (!ecsx->component_manager.component_storage[id]) return LIBX_FALSE;  // error: component not registered!

    // TODO: remove this component from all entities with it
    ecsx->component_manager.component_storage[id] = NULL;
    ecsx->component_manager.add_component_fptr[id] = NULL;
    ecsx->component_manager.rem_component_fptr[id] = NULL;
    ecsx->component_manager.get_component_fptr[id] = NULL;

    ecsx->component_manager.count--;
    return LIBX_TRUE;
}


u8 _add_component_impl(u8 id, u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return LIBX_FALSE;  // error: value error!
    if ((ecsx->entity_manager.mask[entity] & (1<<id)) == (1<<id)) return LIBX_FALSE; // error: entity already has this component!
    
    u8 result = ((_add_component_fptr)ecsx->component_manager.add_component_fptr[id])(entity);
    ecsx->entity_manager.mask[entity] |= (1 << id);
    return result;
}

u8 _rem_component_impl(u8 id, u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return LIBX_FALSE;  // error: value error!
    if ((ecsx->entity_manager.mask[entity] & (1<<id)) != (1<<id)) return LIBX_FALSE; // error: entity does not have this component!

    u8 result = ((_rem_component_fptr)ecsx->component_manager.rem_component_fptr[id])(entity);
    ecsx->entity_manager.mask[entity] &= ~(1<<id);
    return result;
}

u8 _get_component_impl(u8 id, u32 entity, void* component) {
    if (entity >= ENTITY_MAX || entity < 0) return LIBX_FALSE;  // error: value error!
    if ((ecsx->entity_manager.mask[entity] & (1<<id)) != (1<<id)) return LIBX_FALSE;

    return ((_get_component_fptr)ecsx->component_manager.get_component_fptr[id])(entity, component);
}

u32* _get_entities_impl(u8 id) {
    u32* out = structx->create_array(sizeof(u32), ecsx->entity_manager.count);

    LIBX_FORI(0, ecsx->entity_manager.count, 1) {
        structx->push_array(out, ((ecsx->entity_manager.mask[i] & (1<<id)) == (1<<id)) ? &(u32){i} : &(u32){ENTITY_MAX});
    }

    return out;
}


u8 libx_init_ecs(void) {
    if (!memx) {
        printf("libx memory api not initialized!\n");
        return LIBX_FALSE; // error: failed to initialize memory api!
    }
    
    if (!structx) {
        printf("libx structs api not initialized!\n");
        return LIBX_FALSE; // error: failed to initialize memory api!
    }

    ecsx = memx->alloc(sizeof(_libx_ecs_api), 16);
    if (!ecsx) return LIBX_FALSE;  // error: out of memory!
    
    ecsx->entity_manager.queue = structx->create_array(sizeof(u32), ENTITY_MAX);
    if (!ecsx->entity_manager.queue) {
        memx->dealloc(ecsx);
        return LIBX_FALSE;  // error: out of memory!
    }
    
    ecsx->entity_manager.mask = structx->create_array(sizeof(u32), ENTITY_MAX);
    if (!ecsx->entity_manager.mask) {
        structx->destroy_array(ecsx->entity_manager.queue);
        memx->dealloc(ecsx);
        return LIBX_FALSE;  // error: out of memory!
    }
    
    LIBX_FORI(ENTITY_MAX-1, -1, -1) {
        structx->push_array(ecsx->entity_manager.queue, &(u32){i});
    }
    ecsx->entity_manager.count = 0;
    
    ecsx->component_manager.component_storage = structx->create_array(sizeof(void*), COMPONENT_MAX);
    if (!ecsx->component_manager.component_storage) {
        structx->destroy_array(ecsx->entity_manager.queue);
        structx->destroy_array(ecsx->entity_manager.mask);
        memx->dealloc(ecsx);
        return LIBX_FALSE;  // error: out of memory!
    }
    ecsx->component_manager.add_component_fptr = structx->create_array(sizeof(void*), COMPONENT_MAX);
    if (!ecsx->component_manager.add_component_fptr) {
        structx->destroy_array(ecsx->component_manager.component_storage);
        structx->destroy_array(ecsx->entity_manager.queue);
        structx->destroy_array(ecsx->entity_manager.mask);
        memx->dealloc(ecsx);
        return LIBX_FALSE;  // error: out of memory!
    }
    
    ecsx->component_manager.rem_component_fptr = structx->create_array(sizeof(void*), COMPONENT_MAX);
    if (!ecsx->component_manager.rem_component_fptr) {
        structx->destroy_array(ecsx->component_manager.component_storage);
        structx->destroy_array(ecsx->component_manager.add_component_fptr);
        structx->destroy_array(ecsx->entity_manager.queue);
        structx->destroy_array(ecsx->entity_manager.mask);
        memx->dealloc(ecsx);
        return LIBX_FALSE;  // error: out of memory!
    }
    
    ecsx->component_manager.get_component_fptr = structx->create_array(sizeof(void*), COMPONENT_MAX);
    if (!ecsx->component_manager.get_component_fptr) {
        structx->destroy_array(ecsx->component_manager.component_storage);
        structx->destroy_array(ecsx->component_manager.add_component_fptr);
        structx->destroy_array(ecsx->component_manager.rem_component_fptr);
        structx->destroy_array(ecsx->entity_manager.queue);
        structx->destroy_array(ecsx->entity_manager.mask);
        memx->dealloc(ecsx);
        return LIBX_FALSE;  // error: out of memory!
    }
    
    ecsx->component_manager.count = 0;

    ecsx->create_entity = _create_entity_impl;
    ecsx->destroy_entity = _destroy_entity_impl;

    ecsx->register_component = _register_component_impl;
    ecsx->unregister_component = _unregister_component_impl;
    
    ecsx->add_component = _add_component_impl;
    ecsx->get_component = _get_component_impl;
    ecsx->rem_component = _rem_component_impl;
    ecsx->get_entities = _get_entities_impl;
    
    return LIBX_TRUE;
}

void libx_cleanup_ecs(void) {
    structx->destroy_array(ecsx->entity_manager.mask);
    structx->destroy_array(ecsx->entity_manager.queue);

    structx->destroy_array(ecsx->component_manager.component_storage);
    structx->destroy_array(ecsx->component_manager.add_component_fptr);
    structx->destroy_array(ecsx->component_manager.rem_component_fptr);
    structx->destroy_array(ecsx->component_manager.get_component_fptr);

    memx->dealloc(ecsx);
}
