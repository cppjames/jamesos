SECTIONS
{
    kernel_phys_offset = 0xffffffff80000000;
    . = kernel_phys_offset + 1M;

    .stivale2hdr ALIGN(4K) :
    {
        KEEP(*(.stivale2hdr))
    }

    .text ALIGN(4K) :
    {
        KEEP(*(.text*))
    }

    .rodata ALIGN(4K) :
    {
        KEEP(*(.rodata*))
    }

    .data ALIGN(4K) :
    {
        KEEP(*(.data*))
        end_data = .;
    }

    .bss ALIGN(4K) :
    {
        sbss = .;
        KEEP(*(COMMON))
        KEEP(*(.bss*))
        ebss = .;
	    kernel_end = .;
    }
}
