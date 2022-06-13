# Copyright 2022, Technology Innovation Institute
#
# SPDX-License-Identifier: Apache-2.0

SUMMARY = "A dummy x-session starting pong-clock"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

INHIBIT_DEFAULT_DEPS = "1"

RDEPENDS:${PN} += " \
    pong-clock \
    "

XSESSIONFILE = "${BPN}"
ALTERNATIVE_PRIORITY = "15"

require default-x-session.inc

inherit allarch

do_configure() {
    :
}

do_compile() {
    :
}
