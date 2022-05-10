# Copyright 2022, Technology Innovation Institute
#
# SPDX-License-Identifier: Apache-2.0

require vm-image-driver.bb

SUMMARY = "Driver VM with graphics support"

export IMAGE_BASENAME = "vm-image-driver-gui"

IMAGE_FEATURES += " \
    x11 \
    gui-benchmark \
"
