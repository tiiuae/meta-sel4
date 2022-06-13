# Copyright 2022, Technology Innovation Institute
#
# SPDX-License-Identifier: Apache-2.0

SECTION = "x11"

SRC_URI = "file://${XSESSIONFILE}"
S = "${WORKDIR}"

RDEPENDS:${PN} += " \
    mini-x-session \
    busybox \
    "

inherit update-alternatives

ALTERNATIVE:${PN} = "default-x-session"
ALTERNATIVE_LINK_NAME[default-x-session] = "${sysconfdir}/mini_x/session"
ALTERNATIVE_TARGET[default-x-session] = "${datadir}/default-x-session/${XSESSIONFILE_INSTALL}"

FILES:${PN} += "${datadir}/default-x-session"

# Target filename
XSESSIONFILE_INSTALL ??= "${XSESSIONFILE}"

do_install() {
    install -d ${D}/${datadir}/default-x-session
    install -m 0755 \
        ${S}/${XSESSIONFILE} \
        ${D}/${datadir}/default-x-session/${XSESSIONFILE_INSTALL}
}

python __anonymous() {
    if not  d.getVar('XSESSIONFILE', True):
        bb.fatal("%s does not define XSESSIONFILE" % d.getVar('FILE', False))
}
