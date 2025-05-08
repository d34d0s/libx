#include <include/SSDK/SSDK.h>

none ssdkModuleDump(SaneModule* module) {
    printf("Module name: %s\n", module->name);
    printf("Module mask: %d\n", module->mask);
    printf("Module size: %llu bytes\n", module->size);
    printf("Module calls: %llu\n", module->calls);
}

int main() {
    if (ssdkInitLog()) {
        saneLog->log(SANE_LOG_ERROR, "error message!");
        ssdkModuleDump(&saneLog->module);
    }
    
    if (ssdkInitMath()) {
        ssdkModuleDump(&saneMath->module);
        ssdkExitMath();
    }
    
    if (ssdkInitMemory()) {
        ssdkModuleDump(&saneMemory->module);

        ptr mem = saneMemory->alloc(4, 8);
        *(u32*)mem = 42;
        saneMemory->dealloc(mem);

        if (ssdkInitDS()) {
            ssdkModuleDump(&saneData->module);
            
            HashArray harr;
            saneData->hashArray.create(16, &harr);
            saneData->hashArray.put("Key1", &(u32){420}, &harr);

            cstr* keys = saneData->hashArray.getKeys(&harr);
            u32** values = (u32**)saneData->hashArray.getValues(&harr);

            keys[0] = "";
            values[0] = &(u32){1};

            ssdkExitDS();
        }
        ssdkExitMemory();
    }
    
    saneLog->log(SANE_LOG_SUCCESS, "SSDK Tests Ran!");
    ssdkExitLog();

    return 0;
}
