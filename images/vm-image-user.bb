# Copyright 2022, Technology Innovation Institute
#
# SPDX-License-Identifier: Apache-2.0

require recipes-core/images/core-image-minimal.bb
inherit image-hostname vm-image-features vm-guest-image

SUMMARY = "User VM"

IMAGE_ROOTFS_EXTRA_SPACE = "327680"

IMAGE_FEATURES += " \
    benchmark \
    "

export IMAGE_BASENAME = "vm-image-user"
export VM_IMAGE_HOSTNAME = "user-vm"

add_qemu_share() {
    mkdir -p ${IMAGE_ROOTFS}/mnt/shared
    cat >> ${IMAGE_ROOTFS}${sysconfdir}/fstab <<EOF

# Mount qemu shared folder
shared /mnt/shared    9p      trans=virtio,version=9p2000.L   0 0

EOF
}

ROOTFS_POSTPROCESS_COMMAND:append = " \
	add_qemu_share; \
	"

