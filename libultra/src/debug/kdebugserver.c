#include "PR/os_internal.h"
#include "PR/rcp.h"
#include "PR/rdb.h"

// not included in final rom, but __osThreadSave is here for some reason
OSThread __osThreadSave;

extern OSThread *__osRunningThread;
extern u32 __osRdb_IP6_Empty;

