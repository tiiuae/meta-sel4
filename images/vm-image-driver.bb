SUMMARY = "Driver VM"

require recipes-core/images/core-image-minimal.bb

IMAGE_INSTALL += " \
    kernel-module-connection \
    qemu \
    screen \
    tii-benchmark \
"

export IMAGE_BASENAME = "vm-image-driver"
