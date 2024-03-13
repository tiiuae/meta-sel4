PACKAGECONFIG[sel4] = "--enable-sel4,--disable-sel4,,"

SRC_URI:remove = "https://download.qemu.org/${BPN}-${PV}.tar.xz"
SRC_URI += "gitsm://github.com/tiiuae/qemu-sel4-virtio.git;protocol=https;destsuffix=${BPN}-${PV};branch=tii/main"
SRCREV = "${AUTOREV}"

PACKAGECONFIG:class-target += " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'sel4', 'sel4', '', d)} \
"
