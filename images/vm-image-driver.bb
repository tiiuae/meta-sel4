SUMMARY = "Driver VM"

inherit image

IMAGE_INSTALL += " \
    coreutils \
    kernel-module-sel4-virtio \
    kmod \
    qemu \
    util-linux \
"

export IMAGE_BASENAME = "vm-image-driver"
