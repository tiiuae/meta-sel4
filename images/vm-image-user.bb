# Copyright 2022, Technology Innovation Institute
#
# SPDX-License-Identifier: Apache-2.0

SUMMARY = "User VM"

require recipes-core/images/core-image-minimal.bb
inherit image-hostname

IMAGE_ROOTFS_EXTRA_SPACE = "327680"

IMAGE_FSTYPES += " \
    wic.qcow2 \
"

IMAGE_INSTALL += " \
    screen \
    tii-benchmark \
"

export IMAGE_BASENAME = "vm-image-user"
export VM_IMAGE_HOSTNAME = "user-vm"
