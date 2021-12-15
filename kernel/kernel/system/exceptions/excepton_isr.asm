; Instructions for popping error informations off the stack
%macro popErr 0
    mov rdi, qword [rsp]
    add rsp, 8
%endmacro

; Instructions for not popping anything
%macro noPopErr 0
    ; Nothing here
%endmacro

; Generic macro to handle an exception
%macro handleExc 2
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
%macro handleExc_noErr 1
    handleExc %1, noPopErr
%endmacro

; Macro to handle an exception that pushes error information to the stack
%macro handleExc_err 1
    handleExc %1, popErr
%endmacro

handleExc_noErr 0
handleExc_noErr 1
handleExc_noErr 2
handleExc_noErr 3
handleExc_noErr 4
handleExc_noErr 5
handleExc_noErr 6
handleExc_noErr 7
handleExc_err   8
handleExc_noErr 9
handleExc_err   10
handleExc_err   11
handleExc_err   12
handleExc_err   13
handleExc_err   14
handleExc_noErr 15
handleExc_noErr 16
handleExc_err   17
handleExc_noErr 18
handleExc_noErr 19
handleExc_noErr 20
handleExc_err   30