# Copyright 2022, Technology Innovation Institute
#
# SPDX-License-Identifier: Apache-2.0

require vm-image-user.bb

SUMMARY = "User VM with graphics support"

export IMAGE_BASENAME = "vm-image-user-gui"

IMAGE_FEATURES += " \
    x11 \
    gui-benchmark \
    "

IMAGE_INSTALL += " \
    x-session-pong \
    "
