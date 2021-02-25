global enablePAE
global enablePaging

enablePAE:
    push rbp
    mov rbp, rsp

    ; Set bit 5 in CR4
    mov rax, cr4
    bts rax, 5
    mov cr4, rax

    mov rsp, rbp
    pop rbp
    ret

enablePaging:
    push rbp
    mov rbp, rsp
    
    ; Save CR0 in RAX
    mov rax, cr0
    
    ; Set paging bit
    push rbx
    mov rbx, 0x80000000
    or rax, rbx
    pop rbx
    
    ; Load CR0 from RAX
    mov cr0, rax
    
    mov rsp, rbp
    pop rbp
    ret