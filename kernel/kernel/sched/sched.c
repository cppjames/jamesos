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
static long timeSinceLastIrqNs(void);

// Offsets into TaskControlBlock data structure.
// These are used by assembly procedures to access individual members of TCBs.
const size_t taskStackTopOffset = offsetof(TaskControlBlock, stackTop);
const size_t taskAddressSpaceOffset = offsetof(TaskControlBlock, addressSpace);

// Universal PIT oscillator frequency and period constants.
const long oscillatorFreqHz = 1193182;
const long oscillatorPeriodNs = secondsToNs(1) / oscillatorFreqHz;

// Approximate hardcoded frequency of PIT interrupt.
const long timerFreqHz = 100;

// Hardcoded CPU time quantum for each running task.
const long taskQuantumNs = msToNs(30);

// Counter that keeps track of PIT interrupt count.
// Used in calculation of elapsed time in order to switch context when quantum is exceeded.
static long taskSwitchCounter = 0;

// Last measured time elapsed since last timer IRQ
long lastNsElapsed = 0;


// Placeholder functions that run concurrently.

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
    registerIrqHandler(Irq_PIT, timerIrqHandler);
    setTimerFrequency(timerFreqHz);

    initialTask.addressSpace = getRootTable();

    clearIrqMask(Irq_PIT);

    newKernelTask(doStuffA, "Bruh_Task_A");
    newKernelTask(doStuffB, "Bruh_Task_B");

    kinfoLog(Log_Success, "Multitasking enabled.");
}


void timerIrqHandler(void) {
    eoiPicMaster();
    
    ++taskSwitchCounter;

    updateTimeElapsed(timeSinceLastIrqNs());

    // Task has exceeded its time quantum, switch to next
    if (timeSinceLastIrqNs() >= taskQuantumNs) {
        resetNsElapsed();
        switchToTask(currentTask->nextTask);
    }
}


static long timeSinceLastIrqNs(void) {
    // Convert seconds to nanoseconds because frequency is in Hz
    return secondsToNs(1) * taskSwitchCounter / timerFreqHz;
}


void newKernelTask(TaskFunction function, const char *name) {

    // Disable timer interrupts
    setIrqMask(Irq_PIT);

    // Allocate space for control block and task stack
    // TODO - Don't forget to write an actual allocator
    TaskControlBlock *newTask = kallocFrame();
    char *taskStack = kallocFrame();

    // Push return address to stack
    *(Vaddr*)(taskStack + PAGE_SIZE - 8) = (Vaddr)function;

    // Set RFLAGS to IF=1
    // TODO - Make this look less terrible and get rid of magic numbers like 17 and 9
    // Note: PAGE_SIZE-17*8 is location of RFLAGS on stack
    // Note: (1UL << 9) is InterruptFlag
    // Note: 17 means return address + 16 registers
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

    // Insert new TCB just before the current one in queue
    while (lastTask->nextTask != currentTask)
        lastTask = lastTask->nextTask;
    
    lastTask->nextTask = newTask;
    
    // Enable timer interrupts again
    clearIrqMask(Irq_PIT);
}


void switchToTask(TaskControlBlock *task) {
    updateTimeElapsed(readPitTimeElapsed());
    
    if (task == currentTask)
        return;

    taskSwitchCounter = 0;
    resetNsElapsed();

    contextSwitch(task);
}


void updateTimeElapsed(long elapsed) {
    currentTask->nsElapsed += elapsed - lastNsElapsed;
    lastNsElapsed = elapsed;
    kdebugLog("%s : %ldms\n", currentTask->name, nsToMs(currentTask->nsElapsed));
}


void setTimerFrequency(long freq) {
    long freqDivider = oscillatorFreqHz / freq;

    long lowByte = freqDivider & 0xFF;
    long highByte = (freqDivider >> 8) & 0xFF;

    clearInterrupts();

    outb(0x40, lowByte);
	outb(0x40, highByte);

    setInterrupts();

	return;
}


long readPitTimeElapsed(void) {
    clearInterrupts();

	outb(0x43, 0);
 
    long lowByte = inb(0x40);
    long highByte = inb(0x40) << 8;
 
    setInterrupts();

	long counter = lowByte | highByte;
    long maxCounter = (oscillatorFreqHz / timerFreqHz);

    return (maxCounter - counter) * oscillatorPeriodNs;
}


// Reset elapsed time in the event of a context switch.
void resetNsElapsed(void) {
    lastNsElapsed = 0;
}