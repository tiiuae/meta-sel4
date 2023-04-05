FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SUMMARY = "Kernel module to manage seL4 traces: collect and access"
SECTION = "kernel"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://LICENSE;md5=b234ee4d69f5fce4486a80fdaf4a4263"

DEPENDS += " kernel-sel4-support "

inherit module

SRC_URI = "git://github.com/tiiuae/kmod-sel4-tracebuffer.git;protocol=https;branch=main"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git"

EXTRA_OEMAKE += "EXTRA_CFLAGS=-I${STAGING_DIR_TARGET}${includedir}"

MODULES_INSTALL_TARGET = "modules_install"
MODULE_NAME = "sel4-tracebuffer"
KERNEL_MODULE_AUTOLOAD += "${MODULE_NAME}"
