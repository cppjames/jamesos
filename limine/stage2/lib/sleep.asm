section .realmode

int_08_ticks_counter: dd 0

int_08_isr:
    bits 16
    inc dword [cs:int_08_ticks_counter]
    int 0x80   ; call callback
    iret
    bits 32

extern getchar_internal

global pit_sleep_and_quit_on_keypress
pit_sleep_and_quit_on_keypress:
    ; Hook int 0x08
    mov edx, dword [0x08*4]
    mov dword [0x80*4], edx
    mov edx, int_08_isr
    mov dword [0x08*4], int_08_isr

    ; pit_ticks in edx
    mov edx, dword [esp+4]

    mov dword [int_08_ticks_counter], 0

    ; Save non-scratch GPRs
    push ebx
    push esi
    push edi
    push ebp

    ; Jump to real mode
    jmp 0x08:.bits16
  .bits16:
    bits 16
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov eax, cr0
    and al, 0xfe
    mov cr0, eax
    jmp 0x00:.cszero
  .cszero:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    sti

  .loop:
    cmp dword [int_08_ticks_counter], edx
    je .timeout

    push ecx
    push edx
    mov ah, 0x01
    xor al, al
    int 0x16
    pop edx
    pop ecx

    jz .loop

    ; on keypress
    xor ax, ax
    int 0x16
    jmp .done

  .timeout:
    xor eax, eax

  .done:
    cli

    ; Jump back to pmode
    mov ebx, cr0
    or bl, 1
    mov cr0, ebx
    jmp 0x18:.bits32
  .bits32:
    bits 32
    mov bx, 0x20
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    mov ss, bx

    ; Restore non-scratch GPRs
    pop ebp
    pop edi
    pop esi
    pop ebx

    ; Dehook int 0x08
    mov edx, dword [0x80*4]
    mov dword [0x08*4], edx

    push eax
    call getchar_internal
    pop edx

    ret
