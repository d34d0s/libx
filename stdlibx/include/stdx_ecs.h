#pragma once

#include "stdx_def.h"

#define COMPONENT_MAX   (1 << 5)
#define ENTITY_MAX      ((1 << 16) - 1)

typedef u32 Entity;
typedef void* Component;
typedef u8 Component_Id;

typedef struct _stdlibx_ecs_api {
    struct entity_manager {
        Entity* queue;
        Entity* mask;
        u32 count;
    } entity_manager;

    struct component_manager {
        Component** components;
        u32 count;
    } component_manager;

    Entity (*create_entity)(void);
    void (*destroy_entity)(Entity entity);

    u8 (*register_component)(Component_Id id, u64 stride);
    u8 (*add_component)(Component_Id id, Entity entity);
    u8 (*rem_component)(Component_Id id, Entity entity);
    Component (*get_component)(Component_Id id, Entity entity);
    
} _stdlibx_ecs_api;
extern _stdlibx_ecs_api* ecs_api;

STDX_API u8 stdx_init_ecs(void);
STDX_API void stdx_cleanup_ecs(void);
