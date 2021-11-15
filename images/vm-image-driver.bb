SUMMARY = "Driver VM"

require recipes-core/images/core-image-minimal.bb

IMAGE_INSTALL += " \
"

export IMAGE_BASENAME = "vm-image-driver"
