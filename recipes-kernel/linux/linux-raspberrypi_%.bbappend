FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://virtio.cfg \
    file://virtio-blk.cfg \
    file://virtio-net.cfg \
    file://virtio-9p.cfg \
    file://v3d.cfg \
"
