extern irqHandlers

; End of interrupt signal for master PIC IRQs.
%macro eoiPicMaster 0
    mov al, 0x20
    out 0x20, al
%endmacro

; End of interrupt signal for slave PIC IRQs.
%macro eoiPicSlave 0
    mov al, 0x20
    out 0xA0, al
    out 0x20, al
%endmacro

%macro handleIrq 2
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

%macro handleIrqMaster 1
    handleIrq %1, eoiPicMaster
%endmacro

%macro handleIrqSlave 1
    handleIrq %1, eoiPicSlave
%endmacro

%macro handleIrqNoEoi 1
    handleIrq %1, nop
%endmacro

section .text

handleIrqNoEoi 0
handleIrqMaster 1
handleIrqMaster 2
handleIrqMaster 3
handleIrqMaster 4
handleIrqMaster 5
handleIrqMaster 6
handleIrqMaster 7
handleIrqSlave  8
handleIrqSlave  9
handleIrqSlave  10
handleIrqSlave  11
handleIrqSlave  12
handleIrqSlave  13
handleIrqSlave  14
handleIrqSlave  15