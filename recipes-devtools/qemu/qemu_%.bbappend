QEMU_TARGETS = "aarch64"

PACKAGECONFIG[sel4] = "--enable-sel4,--disable-sel4,,"

PACKAGECONFIG = " \
    fdt \
    pie \
    sel4 \
"
