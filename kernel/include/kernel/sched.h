#ifndef TASKS_H
#define TASKS_H

#include <kernel/system/memory.h>

typedef enum TaskState {
    TaskState_Ready,
    TaskState_Running,
    TaskState_Blocked
} TaskState;

typedef struct TaskControlBlock {
    void *stackTop;
    Paddr addressSpace;
    TaskState state;
    const char *name;
    unsigned long nsElapsed;
    struct TaskControlBlock *nextTask;
} TaskControlBlock;

typedef void (*TaskFunction)(void);

extern TaskControlBlock *currentTask;

extern const long counterFreqHz;
extern const long timerFreqHz;
extern const long taskQuantumNs;

void initMultitasking(void);

void newKernelTask(TaskFunction function, const char *name);
void switchToTask(TaskControlBlock *task);
void contextSwitch(TaskControlBlock *task);

void updateTimeElapsed(long elapsed);
void resetNsElapsed(void);

void setTimerFrequency(long freq);
long readPitTimeElapsed(void);

#define secondsToMs(s) ((s)  * 1000L)
#define msToUs(ms)     ((ms) * 1000L)
#define usToNs(us)     ((us) * 1000L)

#define secondsToUs(s) msToUs(secondsToMs(s))
#define secondsToNs(s) usToNs(secondsToUs(s))
#define msToNs(ms)     usToNs(msToUs(ms))

#define nsToUs(ns)      ((ns) / 1000L)
#define usToMs(us)      ((us) / 1000L)
#define msToSeconds(ms) ((ms) / 1000L)

#define nsToMs(ns)      usToMs(nsToUs(ns))
#define nsToSeconds(ns) msToSeconds(nsToMs(ns))
#define usToSeconds(us) msToSeconds(usToMs(us))

#endif // TASKS_H