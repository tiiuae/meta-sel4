# Copyright 2022, Technology Innovation Institute
#
# SPDX-License-Identifier: Apache-2.0

require recipes-core/images/core-image-minimal.bb
inherit image-hostname vm-image-features

SUMMARY = "Driver VM"

IMAGE_FEATURES += " \
    qemu-virtio \
    benchmark \
    "

export IMAGE_BASENAME = "vm-image-driver"

export VM_IMAGE_HOSTNAME = "driver-vm"
