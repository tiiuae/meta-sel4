FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

LICENSE = "CLOSED"

SRC_URI += " \
    file://iperf3-benchmark \
    file://bonnie++-benchmark \
    file://dd-benchmark \
    file://console-stress-test \
    file://screenrc-uservm \
    file://screenrc-drivervm \
"

RDEPENDS:${PN} += " \
    bonnie++ \
    iperf3 \
"

do_install:append() {
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/iperf3-benchmark ${D}${bindir}
    install -m 0755 ${WORKDIR}/bonnie++-benchmark ${D}${bindir}
    install -m 0755 ${WORKDIR}/dd-benchmark ${D}${bindir}
    install -m 0755 ${WORKDIR}/console-stress-test ${D}${bindir}

    install -d ${D}/home/root
    install -m 0755 ${WORKDIR}/screenrc-uservm ${D}/home/root
    install -m 0755 ${WORKDIR}/screenrc-drivervm ${D}/home/root
}

FILES:${PN} = "\
    ${bindir} \
    /home/root \
"
