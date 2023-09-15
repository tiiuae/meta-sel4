SUMMARY = "Kernel module to manage seL4 guest VMs"
SECTION = "kernel"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://LICENSE;md5=b234ee4d69f5fce4486a80fdaf4a4263"

inherit module

SRC_URI = "git://github.com/tiiuae/kmod-sel4-virt.git;protocol=https;branch=main"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git"

EXTRA_OEMAKE += "INSTALL_HDR_PATH=${D}"
MODULES_INSTALL_TARGET = "modules_install headers_install"
MODULE_NAME = "sel4_virt"

KERNEL_MODULE_AUTOLOAD += "${MODULE_NAME}"

