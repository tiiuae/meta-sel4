SUMMARY = "Driver VM"

require recipes-core/images/core-image-minimal.bb

IMAGE_INSTALL += " \
    kernel-module-connection \
    qemu \
    screen \
    tii-benchmark \
"

export IMAGE_BASENAME = "vm-image-driver"

fix_hostname() {
    echo "driver-vm" > ${IMAGE_ROOTFS}/etc/hostname
}

ROOTFS_POSTPROCESS_COMMAND += "fix_hostname; "
