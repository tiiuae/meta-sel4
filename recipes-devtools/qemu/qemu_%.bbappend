QEMU_TARGETS = "aarch64"

QEMU_SEL4_DEPS = ""
QEMU_SEL4_DEPS:class-target = "kernel-module-sel4-virt"
DEPENDS += " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'sel4', "${QEMU_SEL4_DEPS}", '', d)} \
"

PACKAGECONFIG:class-target = " \
    fdt \
    pie \
    kvm \
    virtfs \
    sdl \
    vhost \
    ${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'virglrenderer epoxy', '', d)} \
"
