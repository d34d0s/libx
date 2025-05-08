#include <include/SSDK/SSDK.h>

struct _EntityInternal {
    u32* queue;
    u32* mask;
    u32 count;
} _EntityInternal = {0};

struct _ComponentInternal {
    HashArray* componentSystem;
    void** componentStorage;
    void** addComponentFptr;
    void** remComponentFptr;
    void** getComponentFptr;
    u32 count;
} _ComponentInternal = {NULL};

SaneECS* saneECS = NULL;

u32 _entitiesImpl(none) { return _EntityInternal.count; }

u32 _createEntityImpl(void) {
    u32 entity;

    ArrayHeader qhead = saneData->array.getHeader(_EntityInternal.queue);
    if (qhead.count > 0) {
        saneData->array.pop(_EntityInternal.queue, &entity);
    } else {
        return -1;  // error: array exhausted!
    }
    qhead = saneData->array.getHeader(_EntityInternal.queue);
    
    _EntityInternal.mask[entity] = 0;
    _EntityInternal.count++;
    return entity;
}

u32 _createEntityWithImpl(byte count, byte* ids) {
    if (count < 0 || count > COMPONENT_MAX) return SSDK_FALSE;
    byte result = 0;
    u32 entity = saneECS->createEntity();
    SSDK_FORI(0, count, 1) {
        result = saneECS->addComponent(ids[i], entity);
        if (!result) printf("failed to create (entity)%d with component%d:\n", entity, ids[i]);
    }
    return entity;
}

void _destroyEntityImpl(u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return;
    
    saneData->array.push(_EntityInternal.queue, &(u32){entity});
    
    _EntityInternal.mask[entity] = 0;
    _EntityInternal.count--;
}


byte _registerComponentImpl(
    u8 id, void* storage,
    COMPONENT_ADD_FPTR add_func,
    COMPONENT_REM_FPTR rem_func,
    COMPONENT_GET_FPTR get_func) {
    
    if (id < 0 || id > COMPONENT_MAX || !storage ||
        !add_func || !rem_func || !get_func
    ) return SSDK_FALSE; // error: value error!
    
    saneData->hashArray.create(SYSTEM_MAX, &_ComponentInternal.componentSystem[id]);
    if (!_ComponentInternal.componentSystem[id].map) return SSDK_FALSE;   // error: out of memory!

    _ComponentInternal.componentStorage[id] = storage;
    _ComponentInternal.addComponentFptr[id] = add_func;
    _ComponentInternal.remComponentFptr[id] = rem_func;
    _ComponentInternal.getComponentFptr[id] = get_func;
    
    _ComponentInternal.count++;
    return SSDK_TRUE;
}

byte _unregisterComponentImpl(u8 id) {
    if (id < 0 || id > COMPONENT_MAX) return SSDK_FALSE; // error: value error!
    if (!_ComponentInternal.componentStorage[id]) return SSDK_FALSE;  // error: component not registered!
    
    u32* entities = saneECS->getEntities(id);
    ArrayHeader header = saneData->array.getHeader(entities);
    SSDK_FORI(0, header.count, 1) {
        saneECS->remComponent(id, entities[i]);
    }

    _ComponentInternal.componentStorage[id] = NULL;
    _ComponentInternal.addComponentFptr[id] = NULL;
    _ComponentInternal.remComponentFptr[id] = NULL;
    _ComponentInternal.getComponentFptr[id] = NULL;
    saneData->hashArray.destroy(&_ComponentInternal.componentSystem[id]);

    _ComponentInternal.count--;
    return SSDK_TRUE;
}

byte _unregisterComponentsImpl(none) {
    SSDK_FORI(0, _ComponentInternal.count, 1) {
        if (!saneECS->unregisterComponent(i)) return SSDK_FALSE;
    }
    return SSDK_TRUE;
}


byte _registerSystemImpl(u8 id, cstr name, void* system) {
    if (id < 0 || id > COMPONENT_MAX || !system) return SSDK_FALSE; // error: null ptr/value error!
    
    ArrayHeader head = saneData->array.getHeader(_ComponentInternal.componentSystem[id].map);
    if (head.count == SYSTEM_MAX) return SSDK_FALSE;    // error: maximum systems reached for this component!

    saneData->hashArray.put(name, system, &_ComponentInternal.componentSystem[id]);

    return SSDK_TRUE;
}

byte _unregisterSystemImpl(u8 id, cstr name) {
    if (id < 0 || id > COMPONENT_MAX || !name) return SSDK_FALSE; // error: null ptr/value error!
    KeyValue temp = {NULL};
    return saneData->hashArray.pull(name, &temp, &_ComponentInternal.componentSystem[id]);
}

byte _unregisterSystemsImpl(u8 id) {
    if (id < 0 || id > COMPONENT_MAX) return SSDK_FALSE; // error: null ptr/value error!
    
    cstr* keys = saneData->hashArray.getKeys(&_ComponentInternal.componentSystem[id]);
    if (!keys) return SSDK_FALSE;   // error: failed to get hash array keys!

    SSDK_FORI(0, _ComponentInternal.componentSystem[id].meta.count, 1) {
        saneECS->unregisterSystem(id, keys[i]);
    }
    
    if (keys != NULL) saneData->array.destroy(keys);
    return SSDK_TRUE;
}


byte _runSystemImpl(u8 id, cstr name) {
    if (id < 0 || id > COMPONENT_MAX || !name) return SSDK_FALSE; // error: null ptr/value error!
    
    COMPONENT_SYSTEM_FPTR system = saneData->hashArray.get(name, &_ComponentInternal.componentSystem[id]);
    if (!system) return SSDK_FALSE; // error: failed to get system from hash array!

    SSDK_FORI(0, _EntityInternal.count, 1) {
        if ((_EntityInternal.mask[i] & (1<<id)) == (1<<id)) {
            if (!system(i)) return SSDK_FALSE;  // error: system failed to run!
        }
    }

	return SSDK_TRUE;
}

byte _runSystemsImpl(u8 id) {
    if (id < 0 || id > COMPONENT_MAX) return SSDK_FALSE; // error: null ptr/value error!
    
    cstr* keys = NULL;
    SSDK_FORI(0, _EntityInternal.count, 1) {
        if ((_EntityInternal.mask[i] & (1<<id)) != (1<<id)) continue;  // skip entities with invalid masks
        
        keys = saneData->hashArray.getKeys(&_ComponentInternal.componentSystem[id]);
        if (!keys) return SSDK_FALSE;   // error: failed to get hash array keys!

        SSDK_FORJ(0, _ComponentInternal.componentSystem[id].meta.count, 1) {
            COMPONENT_SYSTEM_FPTR system = saneData->hashArray.get(keys[j], &_ComponentInternal.componentSystem[id]);
            if (!system) return SSDK_FALSE; // error: failed to get system from hash array!
            if (!system(i)) return SSDK_FALSE;  // error: system failed to run!
        }
    }
    
    if (keys != NULL) saneData->array.destroy(keys);
    return SSDK_TRUE;
}

byte _addComponentImpl(u8 id, u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return SSDK_FALSE;  // error: value error!
    if ((_EntityInternal.mask[entity] & (1<<id)) == (1<<id)) return SSDK_FALSE; // error: entity already has this component!
    
    byte result = ((COMPONENT_ADD_FPTR)_ComponentInternal.addComponentFptr[id])(entity);
    _EntityInternal.mask[entity] |= (1 << id);
    return result;
}

byte _remComponentImpl(u8 id, u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return SSDK_FALSE;  // error: value error!
    if ((_EntityInternal.mask[entity] & (1<<id)) != (1<<id)) return SSDK_FALSE; // error: entity does not have this component!

    byte result = ((COMPONENT_REM_FPTR)_ComponentInternal.remComponentFptr[id])(entity);
    _EntityInternal.mask[entity] &= ~(1<<id);
    return result;
}

byte _hasComponentImpl(u8 id, u32 entity) {
    if (entity >= ENTITY_MAX || entity < 0) return SSDK_FALSE;  // error: null ptr/value error!
    return ((_EntityInternal.mask[entity] & (1<<id)) == (1<<id)) ? SSDK_TRUE : SSDK_FALSE;
}

byte _getComponentImpl(u8 id, u32 entity, void* component) {
    if (entity >= ENTITY_MAX || entity < 0 || !component) return SSDK_FALSE;  // error: null ptr/value error!
    if ((_EntityInternal.mask[entity] & (1<<id)) != (1<<id)) return SSDK_FALSE;

    return ((COMPONENT_GET_FPTR)_ComponentInternal.getComponentFptr[id])(entity, component);
}

u32* _getEntitiesImpl(u8 id) {
    u32* out = saneData->array.create(sizeof(u32), _EntityInternal.count);

    SSDK_FORI(0, _EntityInternal.count, 1) {
        saneData->array.push(out, ((_EntityInternal.mask[i] & (1<<id)) == (1<<id)) ? &(u32){i} : &(u32){ENTITY_MAX});
    }

    return out;
}



byte ssdkInitECS(none) {
    if (saneECS != NULL) return SSDK_TRUE;

    if (saneMemory == NULL) {
        if (saneLog != NULL) {
            saneLog->log(SANE_LOG_ERROR, "SaneMemory Not Initialized!");
        } else { printf("SaneMemory Not Initialized!\n"); }
        return SSDK_FALSE;
    }
    
    if (saneData == NULL) {
        if (saneLog != NULL) {
            saneLog->log(SANE_LOG_ERROR, "SaneData Not Initialized!");
        } else { printf("SaneData Not Initialized!\n"); }
        return SSDK_FALSE;
    }

    saneECS = malloc(sizeof(SaneECS));
    if (saneECS == NULL) return SSDK_FALSE;  // error: out of memory!

    _EntityInternal.queue = saneData->array.create(sizeof(u32), ENTITY_MAX);
    if (!_EntityInternal.queue) {
        return SSDK_FALSE;  // error: out of memory!
    }
    
    _EntityInternal.mask = saneData->array.create(sizeof(u32), ENTITY_MAX);
    if (!_EntityInternal.mask) {
        saneData->array.destroy(_EntityInternal.queue);
        return SSDK_FALSE;  // error: out of memory!
    }
    
    SSDK_FORI(ENTITY_MAX-1, -1, -1) {
        saneData->array.push(_EntityInternal.queue, &(u32){i});
    }
    _EntityInternal.count = 0;
    
    _ComponentInternal.componentStorage = saneData->array.create(sizeof(void*), COMPONENT_MAX);
    if (!_ComponentInternal.componentStorage) {
        saneData->array.destroy(_EntityInternal.queue);
        saneData->array.destroy(_EntityInternal.mask);
        return SSDK_FALSE;  // error: out of memory!
    }
    
    _ComponentInternal.componentSystem = saneData->array.create(sizeof(HashArray*), COMPONENT_MAX);
    if (!_ComponentInternal.componentSystem) {
        saneData->array.destroy(_ComponentInternal.componentStorage);
        saneData->array.destroy(_EntityInternal.queue);
        saneData->array.destroy(_EntityInternal.mask);
        return SSDK_FALSE;  // error: out of memory!
    }

    _ComponentInternal.addComponentFptr = saneData->array.create(sizeof(void*), COMPONENT_MAX);
    if (!_ComponentInternal.addComponentFptr) {
        saneData->array.destroy(_ComponentInternal.componentStorage);
        saneData->array.destroy(_ComponentInternal.componentSystem);
        saneData->array.destroy(_EntityInternal.queue);
        saneData->array.destroy(_EntityInternal.mask);
        return SSDK_FALSE;  // error: out of memory!
    }
    
    _ComponentInternal.remComponentFptr = saneData->array.create(sizeof(void*), COMPONENT_MAX);
    if (!_ComponentInternal.remComponentFptr) {
        saneData->array.destroy(_ComponentInternal.componentStorage);
        saneData->array.destroy(_ComponentInternal.componentSystem);
        saneData->array.destroy(_ComponentInternal.addComponentFptr);
        saneData->array.destroy(_EntityInternal.queue);
        saneData->array.destroy(_EntityInternal.mask);
        return SSDK_FALSE;  // error: out of memory!
    }
    
    _ComponentInternal.getComponentFptr = saneData->array.create(sizeof(void*), COMPONENT_MAX);
    if (!_ComponentInternal.getComponentFptr) {
        saneData->array.destroy(_ComponentInternal.componentStorage);
        saneData->array.destroy(_ComponentInternal.componentSystem);
        saneData->array.destroy(_ComponentInternal.addComponentFptr);
        saneData->array.destroy(_ComponentInternal.remComponentFptr);
        saneData->array.destroy(_EntityInternal.queue);
        saneData->array.destroy(_EntityInternal.mask);
        return SSDK_FALSE;  // error: out of memory!
    }
    
    _ComponentInternal.count = 0;

    saneECS->entities = _entitiesImpl;
    saneECS->createEntity = _createEntityImpl;
    saneECS->createEntityWith = _createEntityWithImpl;
    saneECS->destroyEntity = _destroyEntityImpl;

    saneECS->registerComponent = _registerComponentImpl;
    saneECS->unregisterComponent = _unregisterComponentImpl;
    saneECS->unregisterComponents = _unregisterComponentsImpl;
    
    saneECS->registerSystem = _registerSystemImpl;
    saneECS->unregisterSystem = _unregisterSystemImpl;
    saneECS->unregisterSystems = _unregisterSystemsImpl;
    
    saneECS->runSystem = _runSystemImpl;
    saneECS->runSystems = _runSystemsImpl;
    saneECS->addComponent = _addComponentImpl;
    saneECS->getComponent = _getComponentImpl;
    saneECS->remComponent = _remComponentImpl;
    saneECS->hasComponent = _hasComponentImpl;
    saneECS->getEntities = _getEntitiesImpl;

    saneECS->module.mask = 0;
    saneECS->module.calls = 0;
    saneECS->module.name = "SaneECS";
    saneECS->module.size = sizeof(SaneECS);
    return SSDK_TRUE;
}

none ssdkExitECS(none) {
    if (saneECS == NULL) return;
	free(saneECS);

    saneData->array.destroy(_EntityInternal.mask);
    saneData->array.destroy(_EntityInternal.queue);

    ArrayHeader head = saneData->array.getHeader(_ComponentInternal.componentSystem);
    SSDK_FORI(0, head.count, 1) {
        if (_ComponentInternal.componentSystem[i].map) {
            saneData->hashArray.destroy(&_ComponentInternal.componentSystem[i]);
        }
    }

    saneData->array.destroy(_ComponentInternal.componentStorage);
    saneData->array.destroy(_ComponentInternal.componentSystem);
    saneData->array.destroy(_ComponentInternal.addComponentFptr);
    saneData->array.destroy(_ComponentInternal.remComponentFptr);
    saneData->array.destroy(_ComponentInternal.getComponentFptr);

    saneECS = NULL;
}
