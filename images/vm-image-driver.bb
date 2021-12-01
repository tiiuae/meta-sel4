SUMMARY = "Driver VM"

require recipes-core/images/core-image-minimal.bb

IMAGE_INSTALL += " \
    qemu \
"

export IMAGE_BASENAME = "vm-image-driver"
