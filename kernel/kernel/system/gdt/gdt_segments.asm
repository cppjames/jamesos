global reload_segments

reload_segments:
    ; Reload code segment register by setting up
    ; an interrupt stack frame and returning.
    mov rax, rsp
    push 0x10
    push rax
    pushf
    push 0x8
    push reload_cs
    iretq
reload_cs:
    ; Reload data segment registers
    mov   ax, 0x10
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    mov   ss, ax
    ret