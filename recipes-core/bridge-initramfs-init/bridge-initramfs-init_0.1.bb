FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

LICENSE = "CLOSED"

SUMMARY = "Bridged Ethernet setup initramfs for NFS root"

SRC_URI += " \
    file://init \
"

RDEPENDS:${PN} += " \
    busybox \
    bridge-utils \
    tunctl \
    kernel-module-tun \
"

do_install:append() {
    install -m 0755 ${WORKDIR}/init ${D}
}

FILES:${PN} = "\
    /init \
"
