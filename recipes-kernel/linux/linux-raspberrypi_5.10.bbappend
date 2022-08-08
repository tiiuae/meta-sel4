FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Preserve-PMU-EL0-access.patch"
SRC_URI += "file://0002-xhci_single_core_fix.patch"
SRC_URI += "file://0003-events_loopback_war.patch"
SRC_URI += "file://0004-add-sel4-tracebuffer-reserved.patch"
SRC_URI += "file://0005-ftrace-add-pmu-as-clock-source.patch"
