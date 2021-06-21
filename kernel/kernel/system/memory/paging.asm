; Helper functions for enabling paging (mostly for changing the contents of various control registers).

section .data:

IA32_EFER_ID        equ     0xC0000080      ; MSR number of IA32_EFER
IA32_EFER_NXE       equ     11              ; IA32_EFER.NXE (no execute) bit

section .text:

global loadTable
global getTable
global enableNXE

; *********************************************************
; Function to load the physical address of the PML4 into CR3.
;
; Parameters:
;     (qword) rdi - PML4 paddr
;
; C prototype:
;     void loadTable(uint64_t);

loadTable:
    mov cr3, rdi
    ret


; *********************************************************
; Function to get the physical address of the PML4 from CR3.
;
; Returns:
;     (qword) rax - PML4 paddr
;
; C prototype:
;     uint64_t getTable();

getTable:
    mov rax, cr3
    ret


; *********************************************************
; Function to enable IA32_EFER.NXE (no-execute) bit. Only
; needs to be called once.
;
; C prototype:
;     void enableNXE();

enableNXE:
    push rax
    push rcx
    push rdx

    ;  Read EFER in EDX:EAX
    mov rcx, IA32_EFER_ID
    rdmsr

    ; Set IA32_EFER.NXE
    bts eax, IA32_EFER_NXE

    ; Write back into EFER from EDX:EAX
    wrmsr

    pop rdx
    pop rcx
    pop rax
    ret