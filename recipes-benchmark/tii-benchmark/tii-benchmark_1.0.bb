FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

LICENSE = "CLOSED"

SRC_URI += " \
    file://fio-benchmark \
    file://iperf3-benchmark \
    file://console-stress-test \
    file://screenrc-vm1 \
    file://screenrc-vm0 \
"

RDEPENDS:${PN} += " \
    e2fsprogs-mke2fs \
    fio \
    iperf3 \
"

do_install:append() {
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/iperf3-benchmark ${D}${bindir}
    install -m 0755 ${WORKDIR}/fio-benchmark ${D}${bindir}
    install -m 0755 ${WORKDIR}/console-stress-test ${D}${bindir}

    install -d ${D}/home/root
    install -m 0755 ${WORKDIR}/screenrc-vm1 ${D}/home/root
    install -m 0755 ${WORKDIR}/screenrc-vm0 ${D}/home/root
}

FILES:${PN} = "\
    ${bindir} \
    /home/root \
"
