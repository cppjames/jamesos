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

%macro handle 1
    pushaq
    cld
    call %1
    popaq
    iretq
%endmacro

global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

extern irq0_handler
extern irq1_handler
extern irq2_handler
extern irq3_handler
extern irq4_handler
extern irq5_handler
extern irq6_handler
extern irq7_handler
extern irq8_handler
extern irq9_handler
extern irq10_handler
extern irq11_handler
extern irq12_handler
extern irq13_handler
extern irq14_handler
extern irq15_handler

irq0:
    handle irq0_handler
irq1:
    handle irq1_handler
irq2:
    handle irq2_handler
irq3:
    handle irq3_handler
irq4:
    handle irq4_handler
irq5:
    handle irq5_handler
irq6:
    handle irq6_handler
irq7:
    handle irq7_handler
irq8:
    handle irq8_handler
irq9:
    handle irq9_handler
irq10:
    handle irq10_handler
irq11:
    handle irq11_handler
irq12:
    handle irq12_handler
irq13:
    handle irq13_handler
irq14:
    handle irq14_handler
irq15:
    handle irq15_handler