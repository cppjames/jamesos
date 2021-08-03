extern irqHandlers

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
    global irqIsr%1         ; Declare irqIsr function as global
    
    irqIsr%1:
        pushaq              ; Save registers

        ; Get IRQ handler from lookup table
        mov rcx, %1
        mov rbx, qword [irqHandlers + rcx * 8]

        ; If handler is NULL, skip
        cmp rbx, 0
        je missingHandler%1

        call rbx
    missingHandler%1:
    
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

%macro handle_irq_no_eoi 1
    handle_irq %1, nop
%endmacro

section .text

handle_irq_no_eoi 0
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