FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SUMMARY = "sel4 extract trace"

LICENSE = "CLOSED"

inherit cmake

SRC_URI += " \
     file://CMakeLists.txt \
     file://sel4-extract.c \
"

S = "${WORKDIR}"

EXTRA_OECMAKE = ""
