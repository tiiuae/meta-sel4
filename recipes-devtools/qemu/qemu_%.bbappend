QEMU_TARGETS = "aarch64"

QEMU_SEL4_DEPS = ""
QEMU_SEL4_DEPS:class-target = "kernel-module-sel4-virt"
DEPENDS += "${QEMU_SEL4_DEPS}"

PACKAGECONFIG[sel4] = "--enable-sel4,--disable-sel4,,"
PACKAGECONFIG:class-target = " \
    fdt \
    pie \
    sel4 \
    virtfs \
    sdl \
    glx \
    virglrenderer \
"

SRC_URI:remove = "https://download.qemu.org/${BPN}-${PV}.tar.xz"

SRC_URI += "gitsm://git@github.com/tiiuae/qemu-sel4-virtio.git;protocol=ssh;destsuffix=${BPN}-${PV};branch=wip/hlyytine-virtio-blk"
SRCREV = "${AUTOREV}"
