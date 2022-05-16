# Copyright 2022, Technology Innovation Institute
#
# SPDX-License-Identifier: Apache-2.0

# This can be used to turn on/off the functionality
VM_GUEST_IMAGES_SKIP ??= "0"

# List of VM guest images to build and install
VM_GUEST_IMAGES ??= ""
VM_GUEST_IMAGES[type] = "list"

# The default image name format for images is:
# <image>.<VM_GUEST_IMAGE_INSTALL_EXT>
# The default image install dir is defined by VM_GUEST_IMAGE_INSTALL_DIR
# variable. Using the defaults for `foo-image` would install the foo-image
# wic.qcow2 image to:
# /var/lib/virt/images/foo-image.qcow2
#
# The individual image filename and installation path can be customized using
# VM_GUEST_IMAGE_<image> variable. By providing absolute path, one is able to
# control the full install location and the name of the image. If it is not an
# absolute path, it must be a basename with an extension i.e. relative paths
# are not allowed.
#
# Examples:
# VM_GUEST_IMAGE_vm-image-user = "guest-vm.img"
# -> ${VM_GUEST_IMAGE_INSTALL_DIR}/guest-vm.img
# -> /var/lib/virt/images/guest-vm.img (default)

# VM_GUEST_IMAGE_vm-image-user = "/some/path/guest-vm.img"
# -> /some/path/guest-vm.img
#
# The default install path for guest images
VM_GUEST_IMAGES_INSTALL_DIR ??= "${localstatedir}/lib/virt/images"

# The default extension if filename not overridden with VM_GUEST_IMAGE_<image>
VM_GUEST_IMAGES_INSTALL_EXT ??= "qcow2"

# The variable is used to locate the image file(s) from deploy dir
VM_GUEST_IMAGES_FSTYPES ??= "wic.qcow2"

# Helpers
def guest_override_variables_str(d):
    return " ".join(guest_override_variables(d))

def guest_override_variables(d):
    deps = []
    for var in d:
       if var.startswith("VM_GUEST_IMAGE_"):
           deps.append(var)
    return deps

def guest_vm_images(d):
    return set(oe.data.typed_value('VM_GUEST_IMAGES', d))

def default_install_path(image, d):
    install_dir = d.getVar('VM_GUEST_IMAGES_INSTALL_DIR', True)
    install_ext = d.getVar('VM_GUEST_IMAGES_INSTALL_EXT', True)

    return os.path.join(install_dir, f"{image}.{install_ext}")

def image_install_path(image, d):
    guest_override = d.getVar('VM_GUEST_IMAGE_' + image, True)

    if not guest_override:
        return default_install_path(image, d)
    elif os.path.isabs(guest_override):
        return guest_override

    return os.path.join(d.getVar('VM_GUEST_IMAGES_INSTALL_DIR', True), guest_override)

def full_install_path(image, d):
    return oe.path.join(d.getVar("IMAGE_ROOTFS", True),
        image_install_path(image, d))

def image_deploy_path(image, d):
    deploy_dir = d.getVar('DEPLOY_DIR_IMAGE', True)
    machine = d.getVar('MACHINE', True)
    fstypes = d.getVar('VM_GUEST_IMAGES_FSTYPES', True)

    return os.path.join(deploy_dir, f"{image}-{machine}.{fstypes}")

python install_vm_images() {
    import shutil
    for image in guest_vm_images(d):
        dest = full_install_path(image, d)
        src = image_deploy_path(image, d)
        bb.note(f"Install VM image {src} -> {dest}")

        if not os.path.isfile(src):
            bb.fatal(f"VM image {src} does not exist")

        bb.utils.mkdirhier(os.path.dirname(dest))
        shutil.copyfile(src, dest)
}

ROOTFS_POSTINSTALL_COMMAND += "install_vm_images; "

do_rootfs[vardeps] += " \
    VM_GUEST_IMAGES \
    VM_GUEST_IMAGES_INSTALL_DIR \
    VM_GUEST_IMAGES_INSTALL_EXT \
    VM_GUEST_IMAGES_FSTYPES \
    VM_GUEST_IMAGES_SKIP \
    ${@guest_override_variables_str(d)} \
    "

def check_fstypes(d):
    fstypes = d.getVar('VM_GUEST_IMAGES_FSTYPES', True)
    if not fstypes:
        bb.fatal("%s does not define VM_GUEST_IMAGES_FSTYPES" % d.getVar('FILE', False))
    elif len(fstypes.split()) != 1:
        bb.fatal("%s defines multiple VM_GUEST_IMAGES_FSTYPES" % d.getVar('FILE', False))

def validate_guest_override(d):
    # Make sure either absolute, or basename
    for var in guest_override_variables(d):
        guest_override = d.getVar(var, True)
        if not os.path.isabs(guest_override):
            if os.path.basename(guest_override) != guest_override:
                bb.fatal("%s contains relative path" % var)

def set_guest_images_deps(d):
    for image in guest_vm_images(d):
        bb.debug(1, "%s setting depends to %s" % (d.getVar('PN'), image))
        d.appendVarFlag('do_rootfs', 'depends', " " + image + ':do_image_complete')

python __anonymous() {
    if d.getVar('VM_GUEST_IMAGES_SKIP') == "1":
        return

    check_fstypes(d)
    validate_guest_override(d)

    set_guest_images_deps(d)
}
