#include <libx/libx.h>

u32 _create_entity_impl(void) {
    u32 entity;

    Array_Head qhead = libx->dsx.array.get_array_head(libx->ecsx.entity_manager.queue);
    if (qhead.count > 0) {
        libx->dsx.array.pop_array(libx->ecsx.entity_manager.queue, &entity);
    } else {
        return -1;  // error: array exhausted!
    }
    qhead = libx->dsx.array.get_array_head(libx->ecsx.entity_manager.queue);
    
    libx->ecsx.entity_manager.mask[entity] = 0;
    libx->ecsx.entity_manager.count++;
    return entity;
}

u32 _create_entity_with_impl(u8 count, u8* ids) {
    if (count < 0 || count > COMPONENT_MAX) return LIBX_FALSE;
    u8 result = 0;
    u32 entity = libx->ecsx.create_entity();
    LIBX_FORI(0, count, 1) {
        result = libx->ecsx.add_component(ids[i], entity);
        if (!result) printf("failed to create (entity)%d with component%d:\n", entity, ids[i]);
    }
    return entity;
}

void _destroy_entity_impl(u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return;
    
    libx->dsx.array.push_array(libx->ecsx.entity_manager.queue, &(u32){entity});
    
    libx->ecsx.entity_manager.mask[entity] = 0;
    libx->ecsx.entity_manager.count--;
}


u8 _register_component_impl(
    u8 id, void* storage,
    COMPONENT_ADD_FPTR add_func,
    COMPONENT_REM_FPTR rem_func,
    COMPONENT_GET_FPTR get_func) {
    
    if (id < 0 || id > COMPONENT_MAX || !storage ||
        !add_func || !rem_func || !get_func
    ) return LIBX_FALSE; // error: value error!
    
    ((Hash_Array**)libx->ecsx.component_manager.component_system)[id] = libx->dsx.array.create_hash_array(SYSTEM_MAX);
    if (!((Hash_Array**)libx->ecsx.component_manager.component_system)[id]) return LIBX_FALSE;   // error: out of memory!

    libx->ecsx.component_manager.component_storage[id] = storage;
    libx->ecsx.component_manager.add_component_fptr[id] = add_func;
    libx->ecsx.component_manager.rem_component_fptr[id] = rem_func;
    libx->ecsx.component_manager.get_component_fptr[id] = get_func;
    
    libx->ecsx.component_manager.count++;
    return LIBX_TRUE;
}

u8 _unregister_component_impl(u8 id) {
    if (id < 0 || id > COMPONENT_MAX) return LIBX_FALSE; // error: value error!
    if (!libx->ecsx.component_manager.component_storage[id]) return LIBX_FALSE;  // error: component not registered!

    // TODO: remove this component from all entities with it
    libx->ecsx.component_manager.component_storage[id] = NULL;
    libx->dsx.array.destroy_hash_array(((Hash_Array**)libx->ecsx.component_manager.component_system)[id]);
    libx->ecsx.component_manager.add_component_fptr[id] = NULL;
    libx->ecsx.component_manager.rem_component_fptr[id] = NULL;
    libx->ecsx.component_manager.get_component_fptr[id] = NULL;

    libx->ecsx.component_manager.count--;
    return LIBX_TRUE;
}

u8 _register_system_impl(u8 id, cstr name, void* system) {
    if (id < 0 || id > COMPONENT_MAX || !system) return LIBX_FALSE; // error: null ptr/value error!
    
    Array_Head head = libx->dsx.array.get_array_head(((Hash_Array**)libx->ecsx.component_manager.component_system)[id]);
    if (head.count == SYSTEM_MAX) return LIBX_FALSE;    // error: maximum systems reached for this component!

    libx->dsx.array.put_hash_array(((Hash_Array**)libx->ecsx.component_manager.component_system)[id], name, system);

    return LIBX_TRUE;
}

u8 _unregister_system_impl(u8 id, cstr name) {
    if (id < 0 || id > COMPONENT_MAX || !name) return LIBX_FALSE; // error: null ptr/value error!
    Key_Value temp = {NULL};
    return libx->dsx.array.pull_hash_array(((Hash_Array**)libx->ecsx.component_manager.component_system)[id], name, &temp);
}

u8 _unregister_systems_impl(u8 id) {
    if (id < 0 || id > COMPONENT_MAX) return LIBX_FALSE; // error: null ptr/value error!
    Key_Value temp = {NULL};
    
    cstr* keys = libx->dsx.array.get_hash_array_keys(((Hash_Array**)libx->ecsx.component_manager.component_system)[id]);
    if (!keys) return LIBX_FALSE;   // error: failed to get hash array keys!

    LIBX_FORI(0, ((Hash_Array**)libx->ecsx.component_manager.component_system)[id]->meta.count, 1) {
        if (!libx->dsx.array.pull_hash_array(((Hash_Array**)libx->ecsx.component_manager.component_system)[id], keys[i], &temp)) return LIBX_FALSE;   // error: failed to unregister a system!
    }
    
    if (keys != NULL) libx->dsx.array.destroy_array(keys);
    return LIBX_TRUE;
}


u8 _run_system_impl(u8 id, cstr name) {
    if (id < 0 || id > COMPONENT_MAX || !name) return LIBX_FALSE; // error: null ptr/value error!
    
    COMPONENT_SYSTEM_FPTR system = libx->dsx.array.get_hash_array(((Hash_Array**)libx->ecsx.component_manager.component_system)[id], name);
    if (!system) return LIBX_FALSE; // error: failed to get system from hash array!

    LIBX_FORI(0, libx->ecsx.entity_manager.count, 1) {
        if ((libx->ecsx.entity_manager.mask[i] & (1<<id)) == (1<<id)) {
            if (!system(i)) return LIBX_FALSE;  // error: system failed to run!
        }
    }

}

u8 _run_systems_impl(u8 id) {
    if (id < 0 || id > COMPONENT_MAX) return LIBX_FALSE; // error: null ptr/value error!
    
    cstr* keys = NULL;
    LIBX_FORI(0, libx->ecsx.entity_manager.count, 1) {
        if ((libx->ecsx.entity_manager.mask[i] & (1<<id)) != (1<<id)) continue;  // skip entities with invalid masks
        
        keys = libx->dsx.array.get_hash_array_keys(((Hash_Array**)libx->ecsx.component_manager.component_system)[id]);
        if (!keys) return LIBX_FALSE;   // error: failed to get hash array keys!

        LIBX_FORJ(0, ((Hash_Array**)libx->ecsx.component_manager.component_system)[id]->meta.count, 1) {
            COMPONENT_SYSTEM_FPTR system = libx->dsx.array.get_hash_array(((Hash_Array**)libx->ecsx.component_manager.component_system)[id], keys[j]);
            if (!system) return LIBX_FALSE; // error: failed to get system from hash array!
            if (!system(i)) return LIBX_FALSE;  // error: system failed to run!
        }
    }
    
    if (keys != NULL) libx->dsx.array.destroy_array(keys);
    return LIBX_TRUE;
}

u8 _add_component_impl(u8 id, u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return LIBX_FALSE;  // error: value error!
    if ((libx->ecsx.entity_manager.mask[entity] & (1<<id)) == (1<<id)) return LIBX_FALSE; // error: entity already has this component!
    
    u8 result = ((COMPONENT_ADD_FPTR)libx->ecsx.component_manager.add_component_fptr[id])(entity);
    libx->ecsx.entity_manager.mask[entity] |= (1 << id);
    return result;
}

u8 _rem_component_impl(u8 id, u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return LIBX_FALSE;  // error: value error!
    if ((libx->ecsx.entity_manager.mask[entity] & (1<<id)) != (1<<id)) return LIBX_FALSE; // error: entity does not have this component!

    u8 result = ((COMPONENT_REM_FPTR)libx->ecsx.component_manager.rem_component_fptr[id])(entity);
    libx->ecsx.entity_manager.mask[entity] &= ~(1<<id);
    return result;
}

u8 _has_component_impl(u8 id, u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return LIBX_FALSE;  // error: null ptr/value error!
    return ((libx->ecsx.entity_manager.mask[entity] & (1<<id)) == (1<<id)) ? LIBX_TRUE : LIBX_FALSE;
}

u8 _get_component_impl(u8 id, u32 entity, void* component) {
    if (entity >= ENTITY_MAX || entity < 0 || !component) return LIBX_FALSE;  // error: null ptr/value error!
    if ((libx->ecsx.entity_manager.mask[entity] & (1<<id)) != (1<<id)) return LIBX_FALSE;

    return ((COMPONENT_GET_FPTR)libx->ecsx.component_manager.get_component_fptr[id])(entity, component);
}

u32* _get_entities_impl(u8 id) {
    u32* out = libx->dsx.array.create_array(sizeof(u32), libx->ecsx.entity_manager.count);

    LIBX_FORI(0, libx->ecsx.entity_manager.count, 1) {
        libx->dsx.array.push_array(out, ((libx->ecsx.entity_manager.mask[i] & (1<<id)) == (1<<id)) ? &(u32){i} : &(u32){ENTITY_MAX});
    }

    return out;
}


u8 _libx_init_ecsx(void) {
    if (!libx) return LIBX_FALSE; // error: null ptr!
    if (libx->ecsx.init == LIBX_TRUE) return LIBX_TRUE;    // redundant call: Ecsx API already initialized!

    if (libx->memx.init == LIBX_FALSE) {
        printf("Libx Memx API not initialized!\n");
        return LIBX_FALSE; // error: failed to initialize Memx API!
    }
    
    if (libx->dsx.init == LIBX_FALSE) {
        printf("Libx Dsx API not initialized!\n");
        return LIBX_FALSE; // error: failed to initialize Dsx API!
    }

    libx->ecsx.entity_manager.queue = libx->dsx.array.create_array(sizeof(u32), ENTITY_MAX);
    if (!libx->ecsx.entity_manager.queue) {
        return LIBX_FALSE;  // error: out of memory!
    }
    
    libx->ecsx.entity_manager.mask = libx->dsx.array.create_array(sizeof(u32), ENTITY_MAX);
    if (!libx->ecsx.entity_manager.mask) {
        libx->dsx.array.destroy_array(libx->ecsx.entity_manager.queue);
        return LIBX_FALSE;  // error: out of memory!
    }
    
    LIBX_FORI(ENTITY_MAX-1, -1, -1) {
        libx->dsx.array.push_array(libx->ecsx.entity_manager.queue, &(u32){i});
    }
    libx->ecsx.entity_manager.count = 0;
    
    libx->ecsx.component_manager.component_storage = libx->dsx.array.create_array(sizeof(void*), COMPONENT_MAX);
    if (!libx->ecsx.component_manager.component_storage) {
        libx->dsx.array.destroy_array(libx->ecsx.entity_manager.queue);
        libx->dsx.array.destroy_array(libx->ecsx.entity_manager.mask);
        return LIBX_FALSE;  // error: out of memory!
    }
    
    libx->ecsx.component_manager.component_system = libx->dsx.array.create_array(sizeof(Hash_Array*), COMPONENT_MAX);
    if (!libx->ecsx.component_manager.component_system) {
        libx->dsx.array.destroy_array(libx->ecsx.component_manager.component_storage);
        libx->dsx.array.destroy_array(libx->ecsx.entity_manager.queue);
        libx->dsx.array.destroy_array(libx->ecsx.entity_manager.mask);
        return LIBX_FALSE;  // error: out of memory!
    }

    libx->ecsx.component_manager.add_component_fptr = libx->dsx.array.create_array(sizeof(void*), COMPONENT_MAX);
    if (!libx->ecsx.component_manager.add_component_fptr) {
        libx->dsx.array.destroy_array(libx->ecsx.component_manager.component_storage);
        libx->dsx.array.destroy_array(libx->ecsx.component_manager.component_system);
        libx->dsx.array.destroy_array(libx->ecsx.entity_manager.queue);
        libx->dsx.array.destroy_array(libx->ecsx.entity_manager.mask);
        return LIBX_FALSE;  // error: out of memory!
    }
    
    libx->ecsx.component_manager.rem_component_fptr = libx->dsx.array.create_array(sizeof(void*), COMPONENT_MAX);
    if (!libx->ecsx.component_manager.rem_component_fptr) {
        libx->dsx.array.destroy_array(libx->ecsx.component_manager.component_storage);
        libx->dsx.array.destroy_array(libx->ecsx.component_manager.component_system);
        libx->dsx.array.destroy_array(libx->ecsx.component_manager.add_component_fptr);
        libx->dsx.array.destroy_array(libx->ecsx.entity_manager.queue);
        libx->dsx.array.destroy_array(libx->ecsx.entity_manager.mask);
        return LIBX_FALSE;  // error: out of memory!
    }
    
    libx->ecsx.component_manager.get_component_fptr = libx->dsx.array.create_array(sizeof(void*), COMPONENT_MAX);
    if (!libx->ecsx.component_manager.get_component_fptr) {
        libx->dsx.array.destroy_array(libx->ecsx.component_manager.component_storage);
        libx->dsx.array.destroy_array(libx->ecsx.component_manager.component_system);
        libx->dsx.array.destroy_array(libx->ecsx.component_manager.add_component_fptr);
        libx->dsx.array.destroy_array(libx->ecsx.component_manager.rem_component_fptr);
        libx->dsx.array.destroy_array(libx->ecsx.entity_manager.queue);
        libx->dsx.array.destroy_array(libx->ecsx.entity_manager.mask);
        return LIBX_FALSE;  // error: out of memory!
    }
    
    libx->ecsx.component_manager.count = 0;

    libx->ecsx.create_entity = _create_entity_impl;
    libx->ecsx.create_entity_with = _create_entity_with_impl;
    libx->ecsx.destroy_entity = _destroy_entity_impl;

    libx->ecsx.register_component = _register_component_impl;
    libx->ecsx.unregister_component = _unregister_component_impl;
    
    libx->ecsx.register_system = _register_system_impl;
    libx->ecsx.unregister_system = _unregister_system_impl;
    libx->ecsx.unregister_systems = _unregister_systems_impl;
    
    libx->ecsx.run_system = _run_system_impl;
    libx->ecsx.run_systems = _run_systems_impl;
    libx->ecsx.add_component = _add_component_impl;
    libx->ecsx.get_component = _get_component_impl;
    libx->ecsx.rem_component = _rem_component_impl;
    libx->ecsx.has_component = _has_component_impl;
    libx->ecsx.get_entities = _get_entities_impl;
    
	libx->ecsx.init = LIBX_TRUE;
    return LIBX_TRUE;
}

void _libx_cleanup_ecsx(void) {
    if (libx->ecsx.init == LIBX_FALSE) return;    // error: ecsx API not initialized!

    libx->dsx.array.destroy_array(libx->ecsx.entity_manager.mask);
    libx->dsx.array.destroy_array(libx->ecsx.entity_manager.queue);

    Array_Head head = libx->dsx.array.get_array_head(libx->ecsx.component_manager.component_system);
    LIBX_FORI(0, head.count, 1) {
        if (((Hash_Array**)libx->ecsx.component_manager.component_system)[i]) {
            libx->dsx.array.destroy_hash_array(((Hash_Array**)libx->ecsx.component_manager.component_system)[i]);
        }
    }

    libx->dsx.array.destroy_array(libx->ecsx.component_manager.component_storage);
    libx->dsx.array.destroy_array(libx->ecsx.component_manager.component_system);
    libx->dsx.array.destroy_array(libx->ecsx.component_manager.add_component_fptr);
    libx->dsx.array.destroy_array(libx->ecsx.component_manager.rem_component_fptr);
    libx->dsx.array.destroy_array(libx->ecsx.component_manager.get_component_fptr);

	libx->ecsx.init = LIBX_FALSE;
	libx->ecsx	= (Ecsx){0};
}
