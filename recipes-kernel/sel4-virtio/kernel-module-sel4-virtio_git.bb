SUMMARY = "seL4 virtio kernel module"

LICENSE = "CLOSED"

inherit module

SRC_URI = "git://git@github.com/tiiuae/kmod_sel4_virtio.git;protocol=ssh;branch=tii/development"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git"

MODULE_NAME = "sel4_virtio"

KERNEL_MODULE_AUTOLOAD += "${MODULE_NAME}"
