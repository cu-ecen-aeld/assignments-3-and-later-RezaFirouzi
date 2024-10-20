#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.15.163
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    echo "Cleaning the kernel tree - Removing the .config file with any existing configurations"
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} mrproper

    echo "Configuring for 'virt' arm dev board we will simulate in QEMU"
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig

    echo "Building a kernel image for booting with QEMU"
    make -j4 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} all

    echo "Building any kernel modules"
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} modules

    echo "Building the devicetree"
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} dtbs
fi

echo "Adding the Image in outdir"
cp ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ${OUTDIR}/

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# DONE: Create necessary base directories
echo "Creating a directory tree in ${OUTDIR}/rootfs"
mkdir -p "${OUTDIR}/rootfs"
cd "${OUTDIR}/rootfs"
mkdir -p bin dev etc home lib lib64 proc sbin sys tmp usr var
mkdir -p usr/bin usr/sbin usr/lib
mkdir -p var/log

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # DONE:  Configure busybox
    make distclean
    make defconfig
else
    cd busybox
fi

# DONE: Make and install busybox
echo "Installing BusyBox"
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
make CONFIG_PREFIX="${OUTDIR}/rootfs" ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install

cd "${OUTDIR}/rootfs"

echo "Library dependencies"
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

# DONE: Add library dependencies to rootfs
#CROSS_COMPILER_FILE=$(which "${CROSS_COMPILE}gcc")
#if [ -z "$CROSS_COMPILER_FILE" ]; then
#    echo "Failed to find the cross-compiler in the toolchain"
#    exit 1
#fi
#
#CROSS_COMPILER_PATH=$(dirname "$CROSS_COMPILER_FILE")
#TOOLCHAIN_PATH="$CROSS_COMPILER_PATH"/..
#
#cp "$TOOLCHAIN_PATH"/aarch64-none-linux-gnu/libc/lib/ld-linux-aarch64.so.1 ./lib/
#cp "$TOOLCHAIN_PATH"/aarch64-none-linux-gnu/libc/lib64/libm.so.6 ./lib64/
#cp "$TOOLCHAIN_PATH"/aarch64-none-linux-gnu/libc/lib64/libresolv.so.2 ./lib64/
#cp "$TOOLCHAIN_PATH"/aarch64-none-linux-gnu/libc/lib64/libc.so.6 ./lib64/

SYSROOT=$(aarch64-none-linux-gnu-gcc -print-sysroot)

cp ${SYSROOT}/lib/ld-linux-aarch64.so.1 ${OUTDIR}/rootfs/lib/
cp ${SYSROOT}/lib64/libm.so.6 ${OUTDIR}/rootfs/lib64/
cp ${SYSROOT}/lib64/libresolv.so.2 ${OUTDIR}/rootfs/lib64/
cp ${SYSROOT}/lib64/libc.so.6 ${OUTDIR}/rootfs/lib64/

# DONE: Make device nodes
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1

# DONE: Clean and build the writer utility
echo "Building the writer utility"
cd "$FINDER_APP_DIR"
make clean
make CROSS_COMPILE=${CROSS_COMPILE} writer

# DONE: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
cp "$FINDER_APP_DIR/finder.sh" "${OUTDIR}/rootfs/home"
cp "$FINDER_APP_DIR/finder-test.sh" "${OUTDIR}/rootfs/home"
mkdir -p "${OUTDIR}/rootfs/home/conf"
cp -r "$FINDER_APP_DIR/conf"/* "${OUTDIR}/rootfs/home/conf"
cp "$FINDER_APP_DIR/autorun-qemu.sh" "${OUTDIR}/rootfs/home"
cp "$FINDER_APP_DIR/writer" "${OUTDIR}/rootfs/home"

# DONE: Chown the root directory
sudo chown -R root:root "${OUTDIR}/rootfs"

# DONE: Create initramfs.cpio.gz
cd "$OUTDIR/rootfs"
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
gzip -f ${OUTDIR}/initramfs.cpio