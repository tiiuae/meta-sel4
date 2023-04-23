# based on meta-raspberrypi/recipes-bsp/rpi-u-boot-scr/rpi-u-boot-scr.bb

SUMMARY = "Network boot scripts for Raspberry Pi"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
COMPATIBLE_MACHINE = "^rpi$"

DEPENDS = "u-boot-mkimage-native"

INHIBIT_DEFAULT_DEPS = "1"

SRC_URI = " \
    file://tftpboot-bootefi.cmd.in \
    file://tftpboot-go.cmd.in \
    file://tftpboot-linux.cmd.in \
    "

do_compile() {
    for f in ${WORKDIR}/*.cmd.in; do
        base=`basename $f | sed -e 's/\.cmd\.in$//g'`
        sed -e 's/@@KERNEL_IMAGETYPE@@/${KERNEL_IMAGETYPE}/' \
            -e 's/@@KERNEL_BOOTCMD@@/${KERNEL_BOOTCMD}/' \
            "${f}" > "${WORKDIR}/${base}.cmd"
        mkimage -A ${UBOOT_ARCH} -T script -C none -n "Boot script" -d "${WORKDIR}/${base}.cmd" ${base}.scr
    done
}

inherit kernel-arch deploy nopackages

do_deploy() {
    install -d ${DEPLOYDIR}/bootscripts
    for f in *.scr; do
        install -m 0644 ${f} ${DEPLOYDIR}/bootscripts
    done
}

addtask do_deploy after do_compile before do_build

PROVIDES += "bootscripts"
