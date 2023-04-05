FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SUMMARY = "seL4 cross connector kernel module"

LICENSE = "CLOSED"

inherit module

SRC_URI = "git://github.com/tiiuae/camkes-vm-linux.git;protocol=https;branch=wip/hlyytine-virtio-blk"
SRCREV = "${AUTOREV}"

SRC_URI += " \
    file://Makefile \
"

S = "${WORKDIR}/git/camkes-linux-artifacts/camkes-linux-modules/camkes-connector-modules/connection"

MODULE_NAME = "connection"

KERNEL_MODULE_AUTOLOAD += "${MODULE_NAME}"

do_compile:prepend () {
    cp ${WORKDIR}/Makefile ${S}
}
