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

; End of interrupt signal for master PIC IRQs.
%macro eoi_pic_master 0
    mov al, 0x20
    out 0x20, al
%endmacro

; End of interrupt signal for slave PIC IRQs.
%macro eoi_pic_slave 0
    mov al, 0x20
    out 0xA0, al
    out 0x20, al
%endmacro

%macro handle_irq 2
    extern irqHandler%1     ; Use external irqHandler C function
    global irqIsr%1         ; Declare irqIsr function as global
    irqIsr%1:
        pushaq              ; Save registers
        call irqHandler%1   ; Call irqHandler function
        %2                  ; Send end-of-interrupt signal
        popaq               ; Restore registers
        iretq               ; Return from interrupt
%endmacro

%macro handle_irq_master 1
    handle_irq %1, eoi_pic_master
%endmacro

%macro handle_irq_slave 1
    handle_irq %1, eoi_pic_slave
%endmacro

handle_irq_master 0
handle_irq_master 1
handle_irq_master 2
handle_irq_master 3
handle_irq_master 4
handle_irq_master 5
handle_irq_master 6
handle_irq_master 7
handle_irq_slave  8
handle_irq_slave  9
handle_irq_slave  10
handle_irq_slave  11
handle_irq_slave  12
handle_irq_slave  13
handle_irq_slave  14
handle_irq_slave  15