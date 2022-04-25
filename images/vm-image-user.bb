SUMMARY = "User VM"

require recipes-core/images/core-image-minimal.bb

IMAGE_ROOTFS_EXTRA_SPACE = "327680"

IMAGE_FSTYPES += " \
    wic.qcow2 \
"

IMAGE_INSTALL += " \
    screen \
    tii-benchmark \
"

export IMAGE_BASENAME = "vm-image-user"
