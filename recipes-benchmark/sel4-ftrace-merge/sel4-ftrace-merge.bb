FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SUMMARY = "sel4 extract trace"

LICENSE = "CLOSED"

inherit cmake

SRC_URI += " \
     file://CMakeLists.txt \
     file://ftrace-merge.c \
     file://include/ftrace.h       \
"

S = "${WORKDIR}"

EXTRA_OECMAKE = ""
