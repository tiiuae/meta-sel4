FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

QEMU_TARGETS = "aarch64"

PACKAGECONFIG[sel4] = "--enable-sel4,--disable-sel4,,"

PACKAGECONFIG = " \
    fdt \
    pie \
    sel4 \
    virtfs \
    sdl \
    glx \
    virglrenderer \
"

SRC_URI:remove = "https://download.qemu.org/${BPN}-${PV}.tar.xz"

SRC_URI += "gitsm://git@github.com/tiiuae/qemu-sel4-virtio.git;protocol=ssh;destsuffix=${BPN}-${PV};branch=wip/hlyytine-virtio-blk"
SRCREV = "${AUTOREV}"

SRC_URI += " \
    file://qemu-run.sh \
"

do_install:append() {
    install -m 0755 ${WORKDIR}/qemu-run.sh ${D}/${bindir}/qemu-run.sh
}

FILES:${PN} += " \
    ${bindir}/qemu.sh \
"
