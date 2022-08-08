# Copyright 2022, Technology Innovation Institute
#
# SPDX-License-Identifier: Apache-2.0

require recipes-core/images/core-image-minimal.bb
inherit image-hostname vm-image-features vm-guest-images-install

SUMMARY = "Driver VM"

IMAGE_INSTALL:append = " \
    ${@bb.utils.contains("MACHINE_FEATURES", "pci", "pciutils", "", d)} \
    ${@bb.utils.contains("MACHINE_FEATURES", "usbhost", "usbutils", "", d)} \
    "

IMAGE_FEATURES += " \
    qemu-virtio \
    benchmark \
    "

IMAGE_INSTALL += " \
    trace-cmd \
    sel4-trace-support \
    sel4-collect-traces \
    "

export IMAGE_BASENAME = "vm-image-driver"

export VM_IMAGE_HOSTNAME = "driver-vm"

VM_GUEST_IMAGES ?= " \
    vm-image-user \
    "
VM_GUEST_IMAGE_vm-image-user = "user-vm.qcow2"

remove_eth0_autoconfig() {
    sed -i -e '/eth0/d' ${IMAGE_ROOTFS}${sysconfdir}/network/interfaces
}

ROOTFS_POSTPROCESS_COMMAND:append = " \
    remove_eth0_autoconfig; \
"
