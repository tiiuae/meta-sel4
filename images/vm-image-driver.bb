SUMMARY = "Driver VM"

inherit image

IMAGE_INSTALL += " \
    coreutils \
    qemu \
    util-linux \
"

export IMAGE_BASENAME = "vm-image-driver"
