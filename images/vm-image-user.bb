SUMMARY = "User VM"

require recipes-core/images/core-image-minimal.bb

IMAGE_ROOTFS_EXTRA_SPACE = "327680"

IMAGE_FSTYPES += " \
    wic.qcow2 \
"

IMAGE_INSTALL += " \
    screen \
    tii-benchmark \
    kernel-module-sel4 \
"

export IMAGE_BASENAME = "vm-image-user"

fix_hostname() {
    echo "user-vm" > ${IMAGE_ROOTFS}/etc/hostname
}

ROOTFS_POSTPROCESS_COMMAND += "fix_hostname; "
