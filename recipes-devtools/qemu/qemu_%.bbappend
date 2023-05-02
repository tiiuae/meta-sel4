QEMU_TARGETS = "aarch64"

QEMU_SEL4_DEPS = ""
QEMU_SEL4_DEPS:class-target = "kernel-module-sel4-virt"
DEPENDS += "${QEMU_SEL4_DEPS}"

PACKAGECONFIG[sel4] = "--enable-sel4,--disable-sel4,,"
PACKAGECONFIG:class-target = " \
    fdt \
    pie \
    kvm \
    sel4 \
    virtfs \
    sdl \
    glx \
    virglrenderer \
"

SRC_URI:remove = "https://download.qemu.org/${BPN}-${PV}.tar.xz"
SRC_URI += "gitsm://github.com/tiiuae/qemu-sel4-virtio.git;protocol=https;destsuffix=${BPN}-${PV};branch=kvm_comparison"
SRCREV = "${AUTOREV}"
