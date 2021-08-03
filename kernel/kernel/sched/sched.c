#define MODULE sched

#include <kernel/sched.h>

#include <kernel/kinfo.h>
#include <kernel/system/idt.h>
#include <kernel/system/pmm.h>
#include <kernel/system/vmm.h>
#include <kernel/kdebug.h>

#include <sys/io.h>

TaskControlBlock initialTask = {
    .name = "Kernel_Task",
    .state = TaskState_Running,
    .nextTask = &initialTask,
    .nsElapsed = 0
};

TaskControlBlock *currentTask = &initialTask;

void timerIrqHandler(void);

const size_t taskStackTopOffset = (char*)&initialTask.stackTop - (char*)&initialTask;
const size_t taskAddressSpaceOffset = (char*)&initialTask.addressSpace - (char*)&initialTask;

const unsigned counterFreqHertz = 1193182;
const unsigned timerFreqHerts = 100;

const unsigned long taskQuantumNs = msToNs(30);
unsigned taskSwitchCounter = 0;

// Last measured time elapsed since last timer IRQ
unsigned long lastNsElapsed = 0;

static void doStuffA(void) {
    int counter = 0;

    while (true) {
        if (++counter == 10000000) {
            putchar('A');
            counter = 0;
        }
    }
}

static void doStuffB(void) {
    int counter = 0;

    while (true) {
        if (++counter == 10000000) {
            putchar('B');
            counter = 0;
        }
    }
}

void initMultitasking(void) {
    initialTask.addressSpace = getRootTable();
    setTimerFrequency(timerFreqHerts);
    registerIrqHandler(Irq_PIT, timerIrqHandler);
    clearIrqMask(Irq_PIT);

    newKernelTask(doStuffA, "Bruh_Task_A");
    newKernelTask(doStuffB, "Bruh_Task_B");

    kinfoLog(Log_Success, "Multitasking enabled.");
}


void timerIrqHandler(void) {
    eoiPicMaster();
    
    updateTimeElapsed();
    resetNsElapsed();

    ++taskSwitchCounter;

    // Task has exceeded its time quantum, switch to next
    if (secondsToNs(taskSwitchCounter) / timerFreqHerts >= taskQuantumNs) {
        taskSwitchCounter = 0;
        switchToTask(currentTask->nextTask);
    }
}


void newKernelTask(TaskFunction function, const char *name) {
    setIrqMask(Irq_PIT);

    // Allocate space for control block and task stack
    // TODO - Don't forget to write an actual allocator
    TaskControlBlock *newTask = kallocFrame();
    char *taskStack = kallocFrame();

    // Push return address to stack
    *(Vaddr*)(taskStack + PAGE_SIZE - 8) = (Vaddr)function;

    // Set RFLAGS to IF=1
    // TODO - Make this look less terrible and get rid of magic numbers like 17 and 9
    *(uint64_t*)(taskStack + PAGE_SIZE - 17 * 8) = (1UL << 9);

    *newTask = (TaskControlBlock) {
        // We're popping 17 qwords off the stack
        .stackTop = taskStack + PAGE_SIZE - 17 * 8,
        .name = name,
        .addressSpace = getRootTable(),
        .nextTask = currentTask,
        .state = TaskState_Ready,
        .nsElapsed = 0
    };

    TaskControlBlock *lastTask = currentTask;
    while (lastTask->nextTask != currentTask)
        lastTask = lastTask->nextTask;
    
    lastTask->nextTask = newTask;
    
    clearIrqMask(Irq_PIT);
}

void switchToTask(TaskControlBlock *task) {
    updateTimeElapsed();
    if (task == currentTask)
        return;
    kdebugLog("%s : %lums\n", currentTask->name, nsToMs(currentTask->nsElapsed));
    contextSwitch(task);
}

void updateTimeElapsed(void) {
    unsigned newNsElapsed = secondsToNs(1) * (unsigned long)readTimerCount() / counterFreqHertz;
    currentTask->nsElapsed += newNsElapsed - lastNsElapsed;
    lastNsElapsed = newNsElapsed;
}

void setTimerFrequency(unsigned freq) {
    unsigned value = counterFreqHertz / freq;

    unsigned lowByte = value & 0xFF;
    unsigned highByte = (value >> 8) & 0xFF;

    clearInterrupts();

    outb(0x40, lowByte);
	outb(0x40, highByte);

    setInterrupts();

	return;
}

unsigned readTimerCount(void) {
    clearInterrupts();

	outb(0x43, 0);
 
    unsigned lowByte = inb(0x40);
    unsigned highByte = inb(0x40) << 8;
 
    setInterrupts();

	return lowByte | highByte;
}

void resetNsElapsed(void) {
    lastNsElapsed = 0;
}