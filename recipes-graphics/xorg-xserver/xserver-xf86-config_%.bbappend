# Copyright 2022, Technology Innovation Institute
#
# SPDX-License-Identifier: Apache-2.0

SUMMARY = "Enable SW mouse cursor"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append = " \
    file://xorg.conf.d/99-enable_sw_cursor.conf \
"
do_install:append () {
        install -d ${D}/${sysconfdir}/X11/xorg.conf.d/
        install -m 0644 ${WORKDIR}/xorg.conf.d/99-enable_sw_cursor.conf ${D}/${sysconfdir}/X11/xorg.conf.d/
}

FILES:${PN}:append = " ${sysconfdir}/X11/xorg.conf.d/*"
