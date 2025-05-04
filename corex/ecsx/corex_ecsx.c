#include <corex/corex.h>

u32 _create_entity_impl(void) {
    u32 entity;

    Array_Head qhead = corex->dsx.array.get_array_head(corex->ecsx.entity_manager.queue);
    if (qhead.count > 0) {
        corex->dsx.array.pop_array(corex->ecsx.entity_manager.queue, &entity);
    } else {
        return -1;  // error: array exhausted!
    }
    qhead = corex->dsx.array.get_array_head(corex->ecsx.entity_manager.queue);
    
    corex->ecsx.entity_manager.mask[entity] = 0;
    corex->ecsx.entity_manager.count++;
    return entity;
}

u32 _create_entity_with_impl(byte count, byte* ids) {
    if (count < 0 || count > COMPONENT_MAX) return COREX_FALSE;
    byte result = 0;
    u32 entity = corex->ecsx.create_entity();
    COREX_FORI(0, count, 1) {
        result = corex->ecsx.add_component(ids[i], entity);
        if (!result) printf("failed to create (entity)%d with component%d:\n", entity, ids[i]);
    }
    return entity;
}

void _destroy_entity_impl(u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return;
    
    corex->dsx.array.push_array(corex->ecsx.entity_manager.queue, &(u32){entity});
    
    corex->ecsx.entity_manager.mask[entity] = 0;
    corex->ecsx.entity_manager.count--;
}


byte _register_component_impl(
    u8 id, void* storage,
    COMPONENT_ADD_FPTR add_func,
    COMPONENT_REM_FPTR rem_func,
    COMPONENT_GET_FPTR get_func) {
    
    if (id < 0 || id > COMPONENT_MAX || !storage ||
        !add_func || !rem_func || !get_func
    ) return COREX_FALSE; // error: value error!
    
    ((Hash_Array**)corex->ecsx.component_manager.component_system)[id] = corex->dsx.array.create_hash_array(SYSTEM_MAX);
    if (!((Hash_Array**)corex->ecsx.component_manager.component_system)[id]) return COREX_FALSE;   // error: out of memory!

    corex->ecsx.component_manager.component_storage[id] = storage;
    corex->ecsx.component_manager.add_component_fptr[id] = add_func;
    corex->ecsx.component_manager.rem_component_fptr[id] = rem_func;
    corex->ecsx.component_manager.get_component_fptr[id] = get_func;
    
    corex->ecsx.component_manager.count++;
    return COREX_TRUE;
}

byte _unregister_component_impl(u8 id) {
    if (id < 0 || id > COMPONENT_MAX) return COREX_FALSE; // error: value error!
    if (!corex->ecsx.component_manager.component_storage[id]) return COREX_FALSE;  // error: component not registered!

    // TODO: remove this component from all entities with it
    corex->ecsx.component_manager.component_storage[id] = NULL;
    corex->dsx.array.destroy_hash_array(((Hash_Array**)corex->ecsx.component_manager.component_system)[id]);
    corex->ecsx.component_manager.add_component_fptr[id] = NULL;
    corex->ecsx.component_manager.rem_component_fptr[id] = NULL;
    corex->ecsx.component_manager.get_component_fptr[id] = NULL;

    corex->ecsx.component_manager.count--;
    return COREX_TRUE;
}

byte _register_system_impl(u8 id, cstr name, void* system) {
    if (id < 0 || id > COMPONENT_MAX || !system) return COREX_FALSE; // error: null ptr/value error!
    
    Array_Head head = corex->dsx.array.get_array_head(((Hash_Array**)corex->ecsx.component_manager.component_system)[id]);
    if (head.count == SYSTEM_MAX) return COREX_FALSE;    // error: maximum systems reached for this component!

    corex->dsx.array.put_hash_array(((Hash_Array**)corex->ecsx.component_manager.component_system)[id], name, system);

    return COREX_TRUE;
}

byte _unregister_system_impl(u8 id, cstr name) {
    if (id < 0 || id > COMPONENT_MAX || !name) return COREX_FALSE; // error: null ptr/value error!
    Key_Value temp = {NULL};
    return corex->dsx.array.pull_hash_array(((Hash_Array**)corex->ecsx.component_manager.component_system)[id], name, &temp);
}

byte _unregister_systems_impl(u8 id) {
    if (id < 0 || id > COMPONENT_MAX) return COREX_FALSE; // error: null ptr/value error!
    Key_Value temp = {NULL};
    
    cstr* keys = corex->dsx.array.get_hash_array_keys(((Hash_Array**)corex->ecsx.component_manager.component_system)[id]);
    if (!keys) return COREX_FALSE;   // error: failed to get hash array keys!

    COREX_FORI(0, ((Hash_Array**)corex->ecsx.component_manager.component_system)[id]->meta.count, 1) {
        if (!corex->dsx.array.pull_hash_array(((Hash_Array**)corex->ecsx.component_manager.component_system)[id], keys[i], &temp)) return COREX_FALSE;   // error: failed to unregister a system!
    }
    
    if (keys != NULL) corex->dsx.array.destroy_array(keys);
    return COREX_TRUE;
}


byte _run_system_impl(u8 id, cstr name) {
    if (id < 0 || id > COMPONENT_MAX || !name) return COREX_FALSE; // error: null ptr/value error!
    
    COMPONENT_SYSTEM_FPTR system = corex->dsx.array.get_hash_array(((Hash_Array**)corex->ecsx.component_manager.component_system)[id], name);
    if (!system) return COREX_FALSE; // error: failed to get system from hash array!

    COREX_FORI(0, corex->ecsx.entity_manager.count, 1) {
        if ((corex->ecsx.entity_manager.mask[i] & (1<<id)) == (1<<id)) {
            if (!system(i)) return COREX_FALSE;  // error: system failed to run!
        }
    }

	return COREX_TRUE;
}

byte _run_systems_impl(u8 id) {
    if (id < 0 || id > COMPONENT_MAX) return COREX_FALSE; // error: null ptr/value error!
    
    cstr* keys = NULL;
    COREX_FORI(0, corex->ecsx.entity_manager.count, 1) {
        if ((corex->ecsx.entity_manager.mask[i] & (1<<id)) != (1<<id)) continue;  // skip entities with invalid masks
        
        keys = corex->dsx.array.get_hash_array_keys(((Hash_Array**)corex->ecsx.component_manager.component_system)[id]);
        if (!keys) return COREX_FALSE;   // error: failed to get hash array keys!

        COREX_FORJ(0, ((Hash_Array**)corex->ecsx.component_manager.component_system)[id]->meta.count, 1) {
            COMPONENT_SYSTEM_FPTR system = corex->dsx.array.get_hash_array(((Hash_Array**)corex->ecsx.component_manager.component_system)[id], keys[j]);
            if (!system) return COREX_FALSE; // error: failed to get system from hash array!
            if (!system(i)) return COREX_FALSE;  // error: system failed to run!
        }
    }
    
    if (keys != NULL) corex->dsx.array.destroy_array(keys);
    return COREX_TRUE;
}

byte _add_component_impl(u8 id, u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return COREX_FALSE;  // error: value error!
    if ((corex->ecsx.entity_manager.mask[entity] & (1<<id)) == (1<<id)) return COREX_FALSE; // error: entity already has this component!
    
    byte result = ((COMPONENT_ADD_FPTR)corex->ecsx.component_manager.add_component_fptr[id])(entity);
    corex->ecsx.entity_manager.mask[entity] |= (1 << id);
    return result;
}

byte _rem_component_impl(u8 id, u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return COREX_FALSE;  // error: value error!
    if ((corex->ecsx.entity_manager.mask[entity] & (1<<id)) != (1<<id)) return COREX_FALSE; // error: entity does not have this component!

    byte result = ((COMPONENT_REM_FPTR)corex->ecsx.component_manager.rem_component_fptr[id])(entity);
    corex->ecsx.entity_manager.mask[entity] &= ~(1<<id);
    return result;
}

byte _has_component_impl(u8 id, u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return COREX_FALSE;  // error: null ptr/value error!
    return ((corex->ecsx.entity_manager.mask[entity] & (1<<id)) == (1<<id)) ? COREX_TRUE : COREX_FALSE;
}

byte _get_component_impl(u8 id, u32 entity, void* component) {
    if (entity >= ENTITY_MAX || entity < 0 || !component) return COREX_FALSE;  // error: null ptr/value error!
    if ((corex->ecsx.entity_manager.mask[entity] & (1<<id)) != (1<<id)) return COREX_FALSE;

    return ((COMPONENT_GET_FPTR)corex->ecsx.component_manager.get_component_fptr[id])(entity, component);
}

u32* _get_entities_impl(u8 id) {
    u32* out = corex->dsx.array.create_array(sizeof(u32), corex->ecsx.entity_manager.count);

    COREX_FORI(0, corex->ecsx.entity_manager.count, 1) {
        corex->dsx.array.push_array(out, ((corex->ecsx.entity_manager.mask[i] & (1<<id)) == (1<<id)) ? &(u32){i} : &(u32){ENTITY_MAX});
    }

    return out;
}


byte _corex_init_ecsx(void) {
    if (!corex) return COREX_FALSE; // error: null ptr!
    if (corex->ecsx.init == COREX_TRUE) return COREX_TRUE;    // redundant call: Ecsx API already initialized!

    if (corex->memx.init == COREX_FALSE) {
        printf("Corex Memx API not initialized!\n");
        return COREX_FALSE; // error: failed to initialize Memx API!
    }
    
    if (corex->dsx.init == COREX_FALSE) {
        printf("Corex Dsx API not initialized!\n");
        return COREX_FALSE; // error: failed to initialize Dsx API!
    }

    corex->ecsx.entity_manager.queue = corex->dsx.array.create_array(sizeof(u32), ENTITY_MAX);
    if (!corex->ecsx.entity_manager.queue) {
        return COREX_FALSE;  // error: out of memory!
    }
    
    corex->ecsx.entity_manager.mask = corex->dsx.array.create_array(sizeof(u32), ENTITY_MAX);
    if (!corex->ecsx.entity_manager.mask) {
        corex->dsx.array.destroy_array(corex->ecsx.entity_manager.queue);
        return COREX_FALSE;  // error: out of memory!
    }
    
    COREX_FORI(ENTITY_MAX-1, -1, -1) {
        corex->dsx.array.push_array(corex->ecsx.entity_manager.queue, &(u32){i});
    }
    corex->ecsx.entity_manager.count = 0;
    
    corex->ecsx.component_manager.component_storage = corex->dsx.array.create_array(sizeof(void*), COMPONENT_MAX);
    if (!corex->ecsx.component_manager.component_storage) {
        corex->dsx.array.destroy_array(corex->ecsx.entity_manager.queue);
        corex->dsx.array.destroy_array(corex->ecsx.entity_manager.mask);
        return COREX_FALSE;  // error: out of memory!
    }
    
    corex->ecsx.component_manager.component_system = corex->dsx.array.create_array(sizeof(Hash_Array*), COMPONENT_MAX);
    if (!corex->ecsx.component_manager.component_system) {
        corex->dsx.array.destroy_array(corex->ecsx.component_manager.component_storage);
        corex->dsx.array.destroy_array(corex->ecsx.entity_manager.queue);
        corex->dsx.array.destroy_array(corex->ecsx.entity_manager.mask);
        return COREX_FALSE;  // error: out of memory!
    }

    corex->ecsx.component_manager.add_component_fptr = corex->dsx.array.create_array(sizeof(void*), COMPONENT_MAX);
    if (!corex->ecsx.component_manager.add_component_fptr) {
        corex->dsx.array.destroy_array(corex->ecsx.component_manager.component_storage);
        corex->dsx.array.destroy_array(corex->ecsx.component_manager.component_system);
        corex->dsx.array.destroy_array(corex->ecsx.entity_manager.queue);
        corex->dsx.array.destroy_array(corex->ecsx.entity_manager.mask);
        return COREX_FALSE;  // error: out of memory!
    }
    
    corex->ecsx.component_manager.rem_component_fptr = corex->dsx.array.create_array(sizeof(void*), COMPONENT_MAX);
    if (!corex->ecsx.component_manager.rem_component_fptr) {
        corex->dsx.array.destroy_array(corex->ecsx.component_manager.component_storage);
        corex->dsx.array.destroy_array(corex->ecsx.component_manager.component_system);
        corex->dsx.array.destroy_array(corex->ecsx.component_manager.add_component_fptr);
        corex->dsx.array.destroy_array(corex->ecsx.entity_manager.queue);
        corex->dsx.array.destroy_array(corex->ecsx.entity_manager.mask);
        return COREX_FALSE;  // error: out of memory!
    }
    
    corex->ecsx.component_manager.get_component_fptr = corex->dsx.array.create_array(sizeof(void*), COMPONENT_MAX);
    if (!corex->ecsx.component_manager.get_component_fptr) {
        corex->dsx.array.destroy_array(corex->ecsx.component_manager.component_storage);
        corex->dsx.array.destroy_array(corex->ecsx.component_manager.component_system);
        corex->dsx.array.destroy_array(corex->ecsx.component_manager.add_component_fptr);
        corex->dsx.array.destroy_array(corex->ecsx.component_manager.rem_component_fptr);
        corex->dsx.array.destroy_array(corex->ecsx.entity_manager.queue);
        corex->dsx.array.destroy_array(corex->ecsx.entity_manager.mask);
        return COREX_FALSE;  // error: out of memory!
    }
    
    corex->ecsx.component_manager.count = 0;

    corex->ecsx.create_entity = _create_entity_impl;
    corex->ecsx.create_entity_with = _create_entity_with_impl;
    corex->ecsx.destroy_entity = _destroy_entity_impl;

    corex->ecsx.register_component = _register_component_impl;
    corex->ecsx.unregister_component = _unregister_component_impl;
    
    corex->ecsx.register_system = _register_system_impl;
    corex->ecsx.unregister_system = _unregister_system_impl;
    corex->ecsx.unregister_systems = _unregister_systems_impl;
    
    corex->ecsx.run_system = _run_system_impl;
    corex->ecsx.run_systems = _run_systems_impl;
    corex->ecsx.add_component = _add_component_impl;
    corex->ecsx.get_component = _get_component_impl;
    corex->ecsx.rem_component = _rem_component_impl;
    corex->ecsx.has_component = _has_component_impl;
    corex->ecsx.get_entities = _get_entities_impl;
    
    corex->meta.usage.ecsx= sizeof(Ecsx);
	corex->ecsx.init = COREX_TRUE;
    return COREX_TRUE;
}

void _corex_cleanup_ecsx(void) {
    if (corex->ecsx.init == COREX_FALSE) return;    // error: ecsx API not initialized!

    corex->dsx.array.destroy_array(corex->ecsx.entity_manager.mask);
    corex->dsx.array.destroy_array(corex->ecsx.entity_manager.queue);

    Array_Head head = corex->dsx.array.get_array_head(corex->ecsx.component_manager.component_system);
    COREX_FORI(0, head.count, 1) {
        if (((Hash_Array**)corex->ecsx.component_manager.component_system)[i]) {
            corex->dsx.array.destroy_hash_array(((Hash_Array**)corex->ecsx.component_manager.component_system)[i]);
        }
    }

    corex->dsx.array.destroy_array(corex->ecsx.component_manager.component_storage);
    corex->dsx.array.destroy_array(corex->ecsx.component_manager.component_system);
    corex->dsx.array.destroy_array(corex->ecsx.component_manager.add_component_fptr);
    corex->dsx.array.destroy_array(corex->ecsx.component_manager.rem_component_fptr);
    corex->dsx.array.destroy_array(corex->ecsx.component_manager.get_component_fptr);

	corex->meta.usage.apis &= ~COREX_ECSX;
	corex->ecsx.init = COREX_FALSE;
    corex->meta.usage.ecsx = 0;
	corex->ecsx	= (Ecsx){0};
}
