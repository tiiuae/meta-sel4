# Copyright 2022, Technology Innovation Institute
#
# SPDX-License-Identifier: Apache-2.0

SUMMARY = "Driver VM"

require recipes-core/images/core-image-minimal.bb
inherit image-hostname

IMAGE_INSTALL += " \
    kernel-module-connection \
    qemu \
    screen \
    tii-benchmark \
"

export IMAGE_BASENAME = "vm-image-driver"

export VM_IMAGE_HOSTNAME = "driver-vm"
