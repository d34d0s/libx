#include <include/SSDK/SSDK.h>

SSDK* ssdk = NULL;

byte ssdkInit() {
   printf("SSDK GLOBAL INIT!\n");
   return SSDK_TRUE;
}

void ssdkExit() {
   printf("SSDK GLOBAL EXIT!\n");
}

