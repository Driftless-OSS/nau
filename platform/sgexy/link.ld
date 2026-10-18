ENTRY(_start)

SECTIONS {
    . = 0x40010000;
    __kernel_start = .;

    .stub   : { build/crt0.o(.text*) }
    .text   : { *(.text*) }
    .rodata : { *(.rodata*) }
    .data   : { *(.data*) }
    .bss    : {
        . = ALIGN(8);
        __bss_start = .;
        *(.bss*) *(COMMON)
        . = ALIGN(8);
        __bss_end = .;
    }

    . = ALIGN(8);
    STACK_BASE = .;
    . += 0x10000;
    STACK_TOP = .;

    . = ALIGN(4096);
    __kernel_end = .;
}
