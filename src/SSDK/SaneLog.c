#include <include/SSDK/SSDK.h>

SaneLog* saneLog = NULL;

str tags[5] = {
    "[LOG]",
    "[LOG-INFO]",
    "[LOG-WARN]",
    "[LOG-SUCCESS]",
    "[LOG-ERROR]"
};
str colors[5] = {
    "\033[0m",
    "\033[94m",
    "\033[93m",
    "\033[92m",
    "\033[91m"
};


void _log_impl(u8 level, cstr msg) {
    saneLog->module.calls++;
    printf("%s%s %s%s\n", colors[level], tags[level], colors[0], msg);
}

void _logFmt_impl(u8 level, cstr msg, ...) {
    saneLog->module.calls++;

    char buffer[1024];

    va_list args;
    va_start(args, msg);
    vsnprintf(buffer, 1024, msg, args);
    va_end(args);

    saneLog->log(level, buffer);
}

/* ---------------- API ---------------- */
byte ssdkInitLog(none) {
    if (saneLog != NULL) return SSDK_TRUE;
    
    saneLog = malloc(sizeof(SaneLog));
    if (!saneLog) return SSDK_FALSE;  // error: out of memory!

    saneLog->log = _log_impl;
    saneLog->logFmt = _logFmt_impl;
    
    saneLog->module.mask = 0;
    saneLog->module.calls = 0;
    saneLog->module.name = "SaneLog";
    saneLog->module.size = sizeof(SaneLog);
    return SSDK_TRUE;
}

void ssdkExitLog(none) {
    if (saneLog == NULL) return;
    free(saneLog);
    saneLog = NULL;
}
/* ---------------- API ---------------- */
