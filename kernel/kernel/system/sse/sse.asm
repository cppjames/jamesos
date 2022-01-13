; Functions for enabling SSE and wrappers for various SIMD instructions.

section .data:

CR0_MP              equ     1       ; CR0.MP (coprocessor monitoring) bit
CR0_EM              equ     2       ; CR0.EM (coprocessor emulation) bit
CR0_TS              equ     3       ; CR0.TS (task switched) bit

CR4_OSFXSR          equ     9       ; CR4.OSFXSR bit
CR4_OSXMMEXCPT      equ     10      ; CR4.OSXMMEXCPT bit

section .text:

global setSseControlBits
global saveSseState

; *********************************************************
; Function to add support for SSE instructions by altering
; control registers CR0 and CR4.
;
; C prototype:
;     void setSseControlBits(void);

setSseControlBits:
    push rax
    push rdx

    ;-----
    ; Alter control register CR0

    mov rax, cr0
    
    ; Clear CR0.EM
    mov dx, 1
    shl dx, CR0_EM
    not dx
    and ax, dx

    ; Clear CR0.TS
    mov dx, 1
    shl dx, CR0_TS
    not dx
    and ax, dx
    
    ; Set CR0.MP
    mov dx, 1
    shl dx, CR0_MP
    or ax, dx

    mov cr0, rax

    ;-----
    ; Alter control register CR4

    mov rax, cr4

    ; Set CR4.OSFXSR
    mov dx, 1
    shl dx, CR4_OSFXSR
    or ax, dx

    ; Set CR4.OSXMMEXCPT
    mov dx, 1
    shl dx, CR4_OSXMMEXCPT
    or ax, dx

    mov cr4, rax

    pop rdx
    pop rax
    ret

; *********************************************************
;
; Save complete SSE / x87 / FPU / MMX state into buffer.
;
; C prototype:
;     void saveSseState(SseStateBuffer* buffer);

saveSseState:
    fxsave [rdi]