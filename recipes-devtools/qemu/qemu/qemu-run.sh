#!/bin/sh

# guest memory size in megabytes
# TODO: autoprobe from uio
MEMSZ=128

# rootfs for guest
GUEST_IMG="${GUEST_IMG:-/var/lib/virt/images/user-vm.qcow2}"
# directory shared to guest
GUEST_SHARED="${GUEST_SHARED:-$HOME/.local/share/virt/shared}"
mkdir -p "$GUEST_SHARED"

# CAmkES dataports
modprobe uio
modprobe connection

ulimit -c unlimited

exec /usr/bin/qemu-system-aarch64 \
	`# minicom already uses ^A, change monitor escape to ^T` \
	-echr 0x14 \
	`# do not touch these` \
	--accel sel4 \
	-M virt \
	`# use CAmkES dataport for the system memory` \
	-object memory-backend-file,id=virt.ram,size=${MEMSZ}M,mem-path=/dev/uio1,offset=4096,share=on \
	-machine memory-backend=virt.ram \
	-m ${MEMSZ}m \
	`# virtio-blk device for guest rootfs` \
	-device virtio-blk-pci,drive=drive0,id=virtblk0 \
	-drive format=qcow2,file=${GUEST_IMG},if=none,id=drive0 \
	`# share directory to guest over 9P` \
	-virtfs local,path=${GUEST_SHARED},mount_tag=shared,security_model=mapped-xattr \
	`# debug messages, use monitor to read them` \
	-chardev ringbuf,id=debug,size=64K \
	`# enable multiplexer on stdio to have both guest and monitor` \
	-chardev stdio,id=mon,mux=on,signal=off \
	`# enable monitor` \
	-mon chardev=mon,mode=readline \
	`# virtio-console` \
	-device virtio-serial-pci \
	-device virtconsole,chardev=mon,id=console0,name=qemu.uservm \
	`# no need for these` \
	-vga none \
	-display none \
	-serial none \
	`# that's it!`
