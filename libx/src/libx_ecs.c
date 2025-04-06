#include "../include/libx_ecs.h"

_libx_ecs_api* ecsx = NULL;

u32 _create_entity_impl(void) {
    u32 entity;

    Array_Head qhead = structx->array.get_array_head(ecsx->entity_manager.queue);
    if (qhead.count > 0) {
        structx->array.pop_array(ecsx->entity_manager.queue, &entity);
    } else {
        return -1;  // error: array exhausted!
    }
    qhead = structx->array.get_array_head(ecsx->entity_manager.queue);
    
    ecsx->entity_manager.mask[entity] = 0;
    ecsx->entity_manager.count++;
    return entity;
}

void _destroy_entity_impl(u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return;
    
    structx->array.push_array(ecsx->entity_manager.queue, &(u32){entity});
    
    ecsx->entity_manager.mask[entity] = 0;
    ecsx->entity_manager.count--;
}


u8 _register_component_impl(
    u8 id, void* storage,
    COMPONENT_ADD_FPTR add_func,
    COMPONENT_REM_FPTR rem_func,
    COMPONENT_GET_FPTR get_func) {
    
    if (id < 0 || id > COMPONENT_MAX || !storage ||
        !add_func || !rem_func || !get_func
    ) return LIBX_FALSE; // error: value error!
    
    ecsx->component_manager.component_system[id] = structx->array.create_hash_array(SYSTEM_MAX);
    if (!ecsx->component_manager.component_system[id]) return LIBX_FALSE;   // error: out of memory!

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
    structx->array.destroy_hash_array(ecsx->component_manager.component_system[id]);
    ecsx->component_manager.add_component_fptr[id] = NULL;
    ecsx->component_manager.rem_component_fptr[id] = NULL;
    ecsx->component_manager.get_component_fptr[id] = NULL;

    ecsx->component_manager.count--;
    return LIBX_TRUE;
}

u8 _register_system_impl(u8 id, cstr name, void* system) {
    if (id < 0 || id > COMPONENT_MAX || !system) return LIBX_FALSE; // error: null ptr/value error!
    
    Array_Head head = structx->array.get_array_head(ecsx->component_manager.component_system[id]);
    if (head.count == SYSTEM_MAX) return LIBX_FALSE;    // error: maximum systems reached for this component!

    structx->array.put_hash_array(ecsx->component_manager.component_system[id], name, system);

    return LIBX_TRUE;
}

u8 _unregister_system_impl(u8 id, cstr name) {
    if (id < 0 || id > COMPONENT_MAX || !name) return LIBX_FALSE; // error: null ptr/value error!
    Key_Value temp = {NULL};
    return structx->array.pull_hash_array(ecsx->component_manager.component_system[id], name, &temp);
}

u8 _unregister_systems_impl(u8 id) {
    if (id < 0 || id > COMPONENT_MAX) return LIBX_FALSE; // error: null ptr/value error!
    Key_Value temp = {NULL};
    
    cstr* keys = structx->array.get_hash_array_keys(ecsx->component_manager.component_system[id]);
    if (!keys) return LIBX_FALSE;   // error: failed to get hash array keys!

    LIBX_FORI(0, ecsx->component_manager.component_system[id]->meta.count, 1) {
        if (!structx->array.pull_hash_array(ecsx->component_manager.component_system[id], keys[i], &temp)) return LIBX_FALSE;   // error: failed to unregister a system!
    }
    
    if (keys != NULL) structx->array.destroy_array(keys);
    return LIBX_TRUE;
}


u8 _run_system_impl(u8 id, cstr name) {
    if (id < 0 || id > COMPONENT_MAX || !name) return LIBX_FALSE; // error: null ptr/value error!
    
    COMPONENT_SYSTEM_FPTR system = structx->array.get_hash_array(ecsx->component_manager.component_system[id], name);
    if (!system) return LIBX_FALSE; // error: failed to get system from hash array!

    LIBX_FORI(0, ecsx->entity_manager.count, 1) {
        if ((ecsx->entity_manager.mask[i] & (1<<id)) == (1<<id)) {
            if (!system(i)) return LIBX_FALSE;  // error: system failed to run!
        }
    }

}

u8 _run_systems_impl(u8 id) {
    if (id < 0 || id > COMPONENT_MAX) return LIBX_FALSE; // error: null ptr/value error!
    
    cstr* keys = NULL;
    LIBX_FORI(0, ecsx->entity_manager.count, 1) {
        if ((ecsx->entity_manager.mask[i] & (1<<id)) != (1<<id)) continue;  // skip entities with invalid masks
        
        keys = structx->array.get_hash_array_keys(ecsx->component_manager.component_system[id]);
        if (!keys) return LIBX_FALSE;   // error: failed to get hash array keys!

        LIBX_FORJ(0, ecsx->component_manager.component_system[id]->meta.count, 1) {
            COMPONENT_SYSTEM_FPTR system = structx->array.get_hash_array(ecsx->component_manager.component_system[id], keys[j]);
            if (!system) return LIBX_FALSE; // error: failed to get system from hash array!
            if (!system(i)) return LIBX_FALSE;  // error: system failed to run!
        }
    }
    
    if (keys != NULL) structx->array.destroy_array(keys);
    return LIBX_TRUE;
}

u8 _add_component_impl(u8 id, u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return LIBX_FALSE;  // error: value error!
    if ((ecsx->entity_manager.mask[entity] & (1<<id)) == (1<<id)) return LIBX_FALSE; // error: entity already has this component!
    
    u8 result = ((COMPONENT_ADD_FPTR)ecsx->component_manager.add_component_fptr[id])(entity);
    ecsx->entity_manager.mask[entity] |= (1 << id);
    return result;
}

u8 _rem_component_impl(u8 id, u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return LIBX_FALSE;  // error: value error!
    if ((ecsx->entity_manager.mask[entity] & (1<<id)) != (1<<id)) return LIBX_FALSE; // error: entity does not have this component!

    u8 result = ((COMPONENT_REM_FPTR)ecsx->component_manager.rem_component_fptr[id])(entity);
    ecsx->entity_manager.mask[entity] &= ~(1<<id);
    return result;
}

u8 _has_component_impl(u8 id, u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return LIBX_FALSE;  // error: null ptr/value error!
    return ((ecsx->entity_manager.mask[entity] & (1<<id)) == (1<<id)) ? LIBX_TRUE : LIBX_FALSE;
}

u8 _get_component_impl(u8 id, u32 entity, void* component) {
    if (entity >= ENTITY_MAX || entity < 0 || !component) return LIBX_FALSE;  // error: null ptr/value error!
    if ((ecsx->entity_manager.mask[entity] & (1<<id)) != (1<<id)) return LIBX_FALSE;

    return ((COMPONENT_GET_FPTR)ecsx->component_manager.get_component_fptr[id])(entity, component);
}

u32* _get_entities_impl(u8 id) {
    u32* out = structx->array.create_array(sizeof(u32), ecsx->entity_manager.count);

    LIBX_FORI(0, ecsx->entity_manager.count, 1) {
        structx->array.push_array(out, ((ecsx->entity_manager.mask[i] & (1<<id)) == (1<<id)) ? &(u32){i} : &(u32){ENTITY_MAX});
    }

    return out;
}


u8 libx_init_ecs(void) {
    if (ecsx != NULL) return LIBX_TRUE;    // redundant call: ecs API already initialized!

    if (!memx) {
        printf("libx memory api not initialized!\n");
        return LIBX_FALSE; // error: failed to initialize memory api!
    }
    
    if (!structx) {
        printf("libx structs api not initialized!\n");
        return LIBX_FALSE; // error: failed to initialize memory api!
    }

    ecsx = memx->alloc(sizeof(_libx_ecs_api), 8);
    if (!ecsx) return LIBX_FALSE;  // error: out of memory!
    
    ecsx->entity_manager.queue = structx->array.create_array(sizeof(u32), ENTITY_MAX);
    if (!ecsx->entity_manager.queue) {
        memx->dealloc(ecsx);
        return LIBX_FALSE;  // error: out of memory!
    }
    
    ecsx->entity_manager.mask = structx->array.create_array(sizeof(u32), ENTITY_MAX);
    if (!ecsx->entity_manager.mask) {
        structx->array.destroy_array(ecsx->entity_manager.queue);
        memx->dealloc(ecsx);
        return LIBX_FALSE;  // error: out of memory!
    }
    
    LIBX_FORI(ENTITY_MAX-1, -1, -1) {
        structx->array.push_array(ecsx->entity_manager.queue, &(u32){i});
    }
    ecsx->entity_manager.count = 0;
    
    ecsx->component_manager.component_storage = structx->array.create_array(sizeof(void*), COMPONENT_MAX);
    if (!ecsx->component_manager.component_storage) {
        structx->array.destroy_array(ecsx->entity_manager.queue);
        structx->array.destroy_array(ecsx->entity_manager.mask);
        memx->dealloc(ecsx);
        return LIBX_FALSE;  // error: out of memory!
    }
    
    ecsx->component_manager.component_system = structx->array.create_array(sizeof(Hash_Array*), COMPONENT_MAX);
    if (!ecsx->component_manager.component_system) {
        structx->array.destroy_array(ecsx->component_manager.component_storage);
        structx->array.destroy_array(ecsx->entity_manager.queue);
        structx->array.destroy_array(ecsx->entity_manager.mask);
        memx->dealloc(ecsx);
        return LIBX_FALSE;  // error: out of memory!
    }

    ecsx->component_manager.add_component_fptr = structx->array.create_array(sizeof(void*), COMPONENT_MAX);
    if (!ecsx->component_manager.add_component_fptr) {
        structx->array.destroy_array(ecsx->component_manager.component_storage);
        structx->array.destroy_array(ecsx->component_manager.component_system);
        structx->array.destroy_array(ecsx->entity_manager.queue);
        structx->array.destroy_array(ecsx->entity_manager.mask);
        memx->dealloc(ecsx);
        return LIBX_FALSE;  // error: out of memory!
    }
    
    ecsx->component_manager.rem_component_fptr = structx->array.create_array(sizeof(void*), COMPONENT_MAX);
    if (!ecsx->component_manager.rem_component_fptr) {
        structx->array.destroy_array(ecsx->component_manager.component_storage);
        structx->array.destroy_array(ecsx->component_manager.component_system);
        structx->array.destroy_array(ecsx->component_manager.add_component_fptr);
        structx->array.destroy_array(ecsx->entity_manager.queue);
        structx->array.destroy_array(ecsx->entity_manager.mask);
        memx->dealloc(ecsx);
        return LIBX_FALSE;  // error: out of memory!
    }
    
    ecsx->component_manager.get_component_fptr = structx->array.create_array(sizeof(void*), COMPONENT_MAX);
    if (!ecsx->component_manager.get_component_fptr) {
        structx->array.destroy_array(ecsx->component_manager.component_storage);
        structx->array.destroy_array(ecsx->component_manager.component_system);
        structx->array.destroy_array(ecsx->component_manager.add_component_fptr);
        structx->array.destroy_array(ecsx->component_manager.rem_component_fptr);
        structx->array.destroy_array(ecsx->entity_manager.queue);
        structx->array.destroy_array(ecsx->entity_manager.mask);
        memx->dealloc(ecsx);
        return LIBX_FALSE;  // error: out of memory!
    }
    
    ecsx->component_manager.count = 0;

    ecsx->create_entity = _create_entity_impl;
    ecsx->destroy_entity = _destroy_entity_impl;

    ecsx->register_component = _register_component_impl;
    ecsx->unregister_component = _unregister_component_impl;
    
    ecsx->register_system = _register_system_impl;
    ecsx->unregister_system = _unregister_system_impl;
    ecsx->unregister_systems = _unregister_systems_impl;
    
    ecsx->run_system = _run_system_impl;
    ecsx->run_systems = _run_systems_impl;
    ecsx->add_component = _add_component_impl;
    ecsx->get_component = _get_component_impl;
    ecsx->rem_component = _rem_component_impl;
    ecsx->has_component = _has_component_impl;
    ecsx->get_entities = _get_entities_impl;
    
    return LIBX_TRUE;
}

void libx_cleanup_ecs(void) {
    if (ecsx == NULL) return;    // error: ecs API not initialized!

    structx->array.destroy_array(ecsx->entity_manager.mask);
    structx->array.destroy_array(ecsx->entity_manager.queue);

    
    Array_Head head = structx->array.get_array_head(ecsx->component_manager.component_system);
    LIBX_FORI(0, head.count, 1) {
        if (ecsx->component_manager.component_system[i]) {
            structx->array.destroy_hash_array(ecsx->component_manager.component_system[i]);
        }
    }

    structx->array.destroy_array(ecsx->component_manager.component_storage);
    structx->array.destroy_array(ecsx->component_manager.component_system);
    structx->array.destroy_array(ecsx->component_manager.add_component_fptr);
    structx->array.destroy_array(ecsx->component_manager.rem_component_fptr);
    structx->array.destroy_array(ecsx->component_manager.get_component_fptr);

    memx->dealloc(ecsx);
    ecsx = NULL;
}
