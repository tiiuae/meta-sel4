# Copyright 2022, Technology Innovation Institute
#
# SPDX-License-Identifier: Apache-2.0

SUMMARY = "seL4 QEMU helper utilities"
DESCRIPTION = "Helper utilities for operating seL4 QEMU"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

PR = "r1"

SRC_URI = "\
    file://qemu-sel4 \
    file://qemu-sel4-gui \
    "

S = "${WORKDIR}"

do_configure() {
    :
}

do_compile() {
    :
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 \
        ${WORKDIR}/qemu-sel4 \
        ${WORKDIR}/qemu-sel4-gui \
        ${D}${bindir}
}

PACKAGES = "${PN} ${PN}-gui"

FILES:${PN} = "${bindir}/qemu-sel4"
RDEPENDS:${PN} = "\
    busybox \
    qemu \
    "

FILES:${PN}-gui = "${bindir}/qemu-sel4-gui"
RDEPENDS:${PN}-gui = "\
    ${PN} (= ${EXTENDPKGV}) \
    xdpyinfo \
    "
