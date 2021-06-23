#ifndef KINFO_H
#define KINFO_H

#include <kernel/stivale2.h>
#include <stdarg.h>

typedef enum LogStatus {
    Log_Success,
    Log_Info,
    Log_Warn,
    Log_Fail,
    Log_Panic
} LogStatus;

typedef struct stivale2_struct StivaleStruct;
typedef struct stivale2_header StivaleHeader;
typedef struct stivale2_tag StivaleTag;

typedef struct stivale2_header_tag_smp StivaleHTagSmp;
typedef struct stivale2_header_tag_framebuffer StivaleHTagFramebuffer;

typedef struct stivale2_struct_tag_memmap StivaleTagMemmap;
typedef struct stivale2_struct_tag_cmdline StivaleTagCmdline;
typedef struct stivale2_struct_tag_firmware StivaleTagFirmware;
typedef struct stivale2_struct_tag_framebuffer StivaleTagFramebuffer;
typedef struct stivale2_struct_tag_modules StivaleTagModules;
typedef struct stivale2_struct_tag_epoch StivaleTagEpoch;

typedef struct stivale2_mmap_entry StivaleMMapEntry;

void kinfoPrintSplash(StivaleStruct *info);
void kinfoLog(LogStatus status, char *format, ...);

void kinfoParseStivaleStruct(StivaleStruct *info);
StivaleTagMemmap *kinfoGetMemmapTag();
StivaleTagCmdline *kinfoGetCmdlineTag();

#endif // KINFO_H