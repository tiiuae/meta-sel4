# Copyright 2022, Technology Innovation Institute
#
# SPDX-License-Identifier: Apache-2.0

inherit core-image

FEATURE_PACKAGES_qemu-virtio = "\
    kernel-module-connection \
    qemu \
    screen \
"

FEATURE_PACKAGES_benchmark = " \
    screen \
    tii-benchmark \
"
