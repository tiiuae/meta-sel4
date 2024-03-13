FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:${THISDIR}/files:"

KVM_FRAGMENTS = " \
    file://kvm.cfg \
"

SRC_URI += " \
    file://virtio.cfg \
    file://virtio-blk.cfg \
    file://virtio-net.cfg \
    file://virtio-9p.cfg \
    file://virtio-gpu.cfg \
    file://virtio-input.cfg \
    file://v3d.cfg \
    file://pstore.cfg \
    file://swiotlb.cfg \
    file://tap.cfg \
    ${@bb.utils.contains('DISTRO_FEATURES', 'kvm', "${KVM_FRAGMENTS}", "", d)} \
"
