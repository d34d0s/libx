#include <include/SSDK/SSDK.h>

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
    return SSDK_TRUE;
}

byte rem_component(u32 entity) {
    storage.tag[entity] = 0;
    storage.ivalue[entity] = 0;
    storage.fvalue[entity] = 0;
    return SSDK_TRUE;
}

byte get_component(u32 entity, void* component) {
    if (entity < 0 || entity > ENTITY_MAX || !component) return SSDK_FALSE;
    *(my_component*)component = (my_component){
        .tag = storage.tag[entity],
        .ivalue = storage.ivalue[entity],
        .fvalue = storage.fvalue[entity]
    };
    return SSDK_TRUE;
}

byte pre_component_system(u32 entity) {
    saneLog->logFmt(SANE_LOG_INFO, "(PRE) Entity In My Component System: %d", entity);
    return SSDK_TRUE;
}

byte main_component_system(u32 entity) {
    saneLog->logFmt(SANE_LOG_INFO, "(MAIN) Entity In My Component System: %d", entity);
    return SSDK_TRUE;
}

byte post_component_system(u32 entity) {
    saneLog->logFmt(SANE_LOG_INFO, "(POST) Entity In My Component System: %d", entity);
    return SSDK_TRUE;
}

int main() {
    if (ssdkInitLog()) {
        saneLog->log(SANE_LOG_ERROR, "error message!");
        saneLog->logModule(&saneLog->module);
    }
    
    if (ssdkInitMath()) {
        saneLog->logModule(&saneMath->module);
        ssdkExitMath();
    }
    
    if (ssdkInitMemory()) {
        saneLog->logModule(&saneMemory->module);

        ptr mem = saneMemory->alloc(4, 8);
        *(u32*)mem = 42;
        saneMemory->dealloc(mem);

        if (ssdkInitDS()) {
            saneLog->logModule(&saneData->module);
            
            HashArray harr;
            saneData->hashArray.create(16, &harr);
            saneData->hashArray.put("Key1", &(u32){420}, &harr);

            KeyValue pulled;
            saneData->hashArray.pull("Key1", &pulled, &harr);
            saneLog->logFmt(SANE_LOG_SUCCESS, "Pulled: (key=%s, value=%u)", pulled.key, *(u32*)pulled.value);

            cstr* keys = saneData->hashArray.getKeys(&harr);
            u32** values = (u32**)saneData->hashArray.getValues(&harr);

            keys[0] = "";
            values[0] = &(u32){1};

            saneData->hashArray.destroy(&harr);

            if (ssdkInitECS()) {
                saneLog->logModule(&saneECS->module);
                // register a component
                if (saneECS->registerComponent(0, &storage, add_component, rem_component, get_component)) {
                    saneLog->log(SANE_LOG_SUCCESS, "Component Registered!");
                }
                if (saneECS->registerComponent(1, &storage, add_component, rem_component, get_component)) {
                    saneLog->log(SANE_LOG_SUCCESS, "Component Registered!");
                }

                if (
                    saneECS->registerSystem(0, "main", main_component_system) &&
                    saneECS->registerSystem(0, "pre", pre_component_system) &&
                    saneECS->registerSystem(0, "post", post_component_system)
                ) {
                    saneLog->log(SANE_LOG_SUCCESS, "Component System Registered!");
                }

                // create an entity with our components
                u32 entity = saneECS->createEntityWith(2, (byte[]){0, 1});
                saneLog->logFmt(SANE_LOG_SUCCESS, "Created Entity: %u", entity);
                
                // since the component storage exists on the caller side...
                // the caller can manipulate entity-component data
                storage.ivalue[entity] = 123321;

                my_component component;
                if (saneECS->getComponent(0, entity, &component)) {
                    saneLog->logFmt(SANE_LOG_SUCCESS, "Retrieved Component!");
                    saneLog->logFmt(SANE_LOG_INFO, "Tag: %s", component.tag);
                    saneLog->logFmt(SANE_LOG_INFO, "Ivalue: %d", component.ivalue);
                    saneLog->logFmt(SANE_LOG_INFO, "Fvalue: %0.1f", component.fvalue);
                }
                
                // create an entity without our component
                u32 e = saneECS->createEntity();
                saneLog->logFmt(SANE_LOG_SUCCESS, "Created Entity: %u", e);

                // create 8 more entities with our component
                SSDK_FORI(0, 8, 1) {
                    u32 e = saneECS->createEntity();
                    saneECS->addComponent(0, e);
                }

                // we should have 10 entities
                if (saneECS->entities() == 10) saneLog->log(SANE_LOG_INFO, "10 Entities Created!");
                
                // retrieve an array containing all entities with our component
                u32* buffer = saneECS->getEntities(0);
                // SSDK_FORI(0, saneECS->entities(), 1) {
                SSDK_FORI(0, saneData->array.getHeader(buffer).count, 1) {
                    u32 e = buffer[i];
                    if (e == ENTITY_MAX) continue;    // skip sentinnel values
                    saneLog->logFmt(SANE_LOG_INFO, "Entity Has My Component: %d", e);
                }
                saneData->array.destroy(buffer);

                if (saneECS->runSystem(0, "main")) saneLog->log(SANE_LOG_SUCCESS, "System Ran!");
                if (saneECS->runSystems(0)) saneLog->log(SANE_LOG_SUCCESS, "All Systems Ran!");
                    
                if (saneECS->unregisterSystems(0)) saneLog->log(SANE_LOG_SUCCESS, "All Systems Unregistered!");
                if (saneECS->unregisterComponents()) saneLog->log(SANE_LOG_SUCCESS, "All Components Unregistered!");
                ssdkExitECS();
            }

            ssdkExitDS();
        }
        ssdkExitMemory();
    }
    
    saneLog->log(SANE_LOG_SUCCESS, "SSDK Tests Ran!");
    ssdkExitLog();

    return 0;
}
