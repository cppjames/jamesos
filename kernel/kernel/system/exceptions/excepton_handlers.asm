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

%macro pop_err 0
    mov rdi, qword [rsp]
    add rsp, 8
%endmacro

%macro nopop_err 0
    ; Nothing here
%endmacro

%macro handle_exc 2
    extern exc%1_handler
    global exc%1
    exc%1:
        pushaq
        %2
        call exc%1_handler
        popaq
        iretq
%endmacro

%macro handle_exc_noerr 1
    handle_exc %1, nopop_err
%endmacro

%macro handle_exc_err 1
    handle_exc %1, pop_err
%endmacro

handle_exc_noerr 0
handle_exc_noerr 1
handle_exc_noerr 2
handle_exc_noerr 3
handle_exc_noerr 4
handle_exc_noerr 5
handle_exc_noerr 6
handle_exc_noerr 7
handle_exc_err   8
handle_exc_noerr 9
handle_exc_err   10
handle_exc_err   11
handle_exc_err   12
handle_exc_err   13
handle_exc_err   14
handle_exc_noerr 15
handle_exc_noerr 16
handle_exc_err   17
handle_exc_noerr 18
handle_exc_noerr 19
handle_exc_noerr 20
handle_exc_err   30