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
