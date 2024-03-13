# Copyright 2022, 2024, Technology Innovation Institute
#
# SPDX-License-Identifier: Apache-2.0

SUMMARY = "TII hypervisor R&D QEMU helper utilities"
DESCRIPTION = "Helper utilities for QEMU and TII hypervisor R&D"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

PR = "r1"

SRC_URI = "\
    file://qemu-tii \
    file://qemu-tii-gui \
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
        ${WORKDIR}/qemu-tii \
        ${WORKDIR}/qemu-tii-gui \
        ${D}${bindir}
}

PACKAGES = "${PN} ${PN}-gui"

FILES:${PN} = "${bindir}/qemu-tii"
RDEPENDS:${PN} = "\
    busybox \
    qemu \
    "

FILES:${PN}-gui = "${bindir}/qemu-tii-gui"
RDEPENDS:${PN}-gui = "\
    ${PN} (= ${EXTENDPKGV}) \
    xdpyinfo \
    "
