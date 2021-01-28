%macro pushaq 0
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro popaq 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

%macro handle_irq 1
    extern irq%1_handler
    global irq%1
    irq%1:
        pushaq
        cld
        call irq%1_handler
        popaq
        iretq
%endmacro

handle_irq 0
handle_irq 1
handle_irq 2
handle_irq 3
handle_irq 4
handle_irq 5
handle_irq 6
handle_irq 7
handle_irq 8
handle_irq 9
handle_irq 10
handle_irq 11
handle_irq 12
handle_irq 13
handle_irq 14
handle_irq 15