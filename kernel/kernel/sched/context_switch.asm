global contextSwitch

extern currentTask
extern taskStackTopOffset
extern taskAddressSpaceOffset

extern updateTimeElapsed

contextSwitch:
    pushaq
    cli

    mov rsi, [currentTask]          ; rdi contains the address of the current task
    mov rdx, [taskStackTopOffset]   ; Offset of stack top into task control block
    mov [rsi + rdx], rsp            ; Save rsp in the task's control block's stack top

    ;-----
    ; Load next task's state

    mov [currentTask], rdi          ; rdi contains the address of the next task
    mov rsp, [rdi + rdx]            ; Load top of the stack from next task's control block
    
    mov rdx, [taskAddressSpaceOffset]
    mov rax, [rdi + rdx]            ; Load next task's address space into rax

    mov rcx, cr3
    cmp rax, rcx                    ; If the address space is the same, skip
    je sameAddressSpace
    
    mov cr3, rax                    ; Otherwise, load new address space into cr3
sameAddressSpace:

    popaq
    ret