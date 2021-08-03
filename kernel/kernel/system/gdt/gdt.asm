global reloadSegments
global loadGdt

; *********************************************************
; Function to load the GDT pointer into GDTR.
;
; Parameters:
;     (qword) rdi - the address of a GDT pointer structure
;
; C prototype:
;     void loadGdt(GdtPtr *gdt_ptr);

loadGdt:
    lgdt [rdi]
    ret


; *********************************************************
; Function to reload all segment registers after the GDT has been loaded.
;
; C prototype:
;     void reloadSegments(void);

reloadSegments:
    ; Set up interrupts stack frame
    mov rax, rsp
    push 0x10
    push rax
    pushf
    push 0x8
    push reload_cs

    ; Reload code segment by far-returning to reload_cs
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