SUMMARY = "seL4 guest kernel module"

LICENSE = "CLOSED"

inherit module

SRC_URI = "git://git@github.com/tiiuae/kmod_sel4.git;protocol=ssh;branch=tii/development"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git"

MODULE_NAME = "sel4"

KERNEL_MODULE_AUTOLOAD += "${MODULE_NAME}"
