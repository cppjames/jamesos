global reload_segments

reload_segments:
    ; Reload CS register containing code selector:
    push 0x08
    push reload_cs
    retfq
reload_cs:
    ; Reload data segment registers:
    mov   ax, 0x10
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    mov   ss, ax
    ret