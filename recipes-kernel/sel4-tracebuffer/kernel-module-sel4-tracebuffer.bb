FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SUMMARY = "seL4 trace buffer kernel module"

LICENSE = "CLOSED"

inherit module

SRC_URI += " \
    file://Makefile \
    file://sel4-tracebuffer.c \
    file://sel4-support.h \
"

MODULE_NAME = "sel4-tracebuffer"

KERNEL_MODULE_AUTOLOAD += "${MODULE_NAME}"

do_compile:prepend () {
    cp ${WORKDIR}/Makefile ${S}
    cp ${WORKDIR}/sel4-tracebuffer.c ${S}
    cp ${WORKDIR}/sel4-support.h ${S}
}
