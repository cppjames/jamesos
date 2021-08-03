; Instructions for popping error informations off the stack
%macro pop_err 0
    mov rdi, qword [rsp]
    add rsp, 8
%endmacro

; Instructions for not popping anything
%macro nopop_err 0
    ; Nothing here
%endmacro

; Generic macro to handle an exception
%macro handle_exc 2
    extern excHandler%1
    global excIsr%1
    
    excIsr%1:
        pushaq              ; Save all registers
        %2                  ; To pop or not to pop
        call excHandler%1   ; Call C handler function
        popaq               ; Restore all registers
        iretq               ; Return from ISR
%endmacro

; Macro to handle an exception that doesn't push error information to the stack
%macro handle_exc_noerr 1
    handle_exc %1, nopop_err
%endmacro

; Macro to handle an exception that pushes error information to the stack
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