FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Preserve-PMU-EL0-access.patch \
            file://0002-XHCI-timeout-workaround.patch \
            file://0003-Input-event-workaround.patch \
            file://0004-Add-seL4-tracebuffer-reserved-mem-type.patch \
            file://0005-Add-PMU-as-ftrace-clock-source.patch \
            file://0001-Use-seL4-specific-kludge.patch \
            "

