# Copyright 2022, Technology Innovation Institute
#
# SPDX-License-Identifier: Apache-2.0

inherit image

fix_hostname() {
    echo "${VM_IMAGE_HOSTNAME}" > ${IMAGE_ROOTFS}/etc/hostname
}

ROOTFS_POSTPROCESS_COMMAND += "fix_hostname; "

def vm_image_check_mandatory_vars(d):
    if d.getVar('VM_IMAGE_HOSTNAME', False) == None:
        bb.fatal("%s does not define VM_IMAGE_HOSTNAME" % d.getVar('FILE', False))

python __anonymous() {
    vm_image_check_mandatory_vars(d)
}
