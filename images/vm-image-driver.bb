SUMMARY = "Driver VM"

inherit image

IMAGE_INSTALL += " \
    coreutils \
    kmod \
    qemu \
    util-linux \
"

export IMAGE_BASENAME = "vm-image-driver"
