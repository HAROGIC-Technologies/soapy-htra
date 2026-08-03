#!/usr/bin/env bash
set -Eeuo pipefail


#############################################################
# HAROGIC HTRA API Library Installation Script
#   x86_64
#   ARM64 / aarch64
#
#############################################################


log()
{
    printf "\n[HTRA] %s\n" "$*"
}


fail()
{
    printf "\n[HTRA] ERROR: %s\n" "$*" >&2
    exit 1
}


#############################################################
# Check root
#############################################################

if [ "$EUID" -ne 0 ]; then
    fail "You have to be root to run this script."
fi



#############################################################
# Get script path
#############################################################

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"


cd "$PROJECT_ROOT"



#############################################################
# Detect architecture
#############################################################

ARCH=$(uname -m)


case "$ARCH" in

    x86_64)

        LIB_ARCH="x86_64"

        ;;


    aarch64|arm64)

        LIB_ARCH="aarch64"

        ;;


    *)

        fail "Unsupported architecture: ${ARCH}"

        ;;

esac


log "Detected architecture: ${ARCH}"



#############################################################
# Check library directory
#############################################################

LIB_DIR="${PROJECT_ROOT}/htraapi/lib/${LIB_ARCH}"


if [ ! -d "$LIB_DIR" ]; then

    fail "Library directory not found:
$LIB_DIR"

fi



#############################################################
# Find HTRA API library
#############################################################

HTRA_LIB=$(find "$LIB_DIR" \
    -maxdepth 1 \
    -name "libhtraapi.so.*" \
    | sort \
    | tail -n 1)


if [ -z "$HTRA_LIB" ]; then

    fail "Cannot find libhtraapi.so in:
$LIB_DIR"

fi



LIB_FILE=$(basename "$HTRA_LIB")


VERSION=${LIB_FILE#libhtraapi.so.}

MAJOR=${VERSION%%.*}



log "Using HTRA library:"
echo "$HTRA_LIB"



#############################################################
# Install configuration files
#############################################################

log "Installing configuration files"


if [ -f "${PROJECT_ROOT}/htraapi/configs/htrausb.conf" ]; then

    cp \
    "${PROJECT_ROOT}/htraapi/configs/htrausb.conf" \
    /etc/

fi



if [ -f "${PROJECT_ROOT}/htraapi/configs/htra-cyusb.rules" ]; then

    cp \
    "${PROJECT_ROOT}/htraapi/configs/htra-cyusb.rules" \
    /etc/udev/rules.d/

fi



#############################################################
# Install library
#############################################################

log "Installing HTRA API library"


cp -a "${LIB_DIR}/"* /usr/local/lib/



#############################################################
# Copy SDK to /opt
#############################################################

log "Copying HTRA SDK"


rm -rf /opt/htraapi

cp -r "${PROJECT_ROOT}/htraapi" /opt/



#############################################################
# Create /opt library links
#############################################################

OPT_LIB_DIR="/opt/htraapi/lib/${LIB_ARCH}"


if [ -f "${OPT_LIB_DIR}/libhtraapi.so.${VERSION}" ]; then


    ln -sfn \
    "libhtraapi.so.${VERSION}" \
    "${OPT_LIB_DIR}/libhtraapi.so.${MAJOR}"


    ln -sfn \
    "libhtraapi.so.${MAJOR}" \
    "${OPT_LIB_DIR}/libhtraapi.so"


fi



#############################################################
# Create /opt libusb links
#############################################################

if [ -f "${OPT_LIB_DIR}/libusb-1.0.so.0.2.0" ]; then


    ln -sfn \
    "libusb-1.0.so.0.2.0" \
    "${OPT_LIB_DIR}/libusb-1.0.so.0"


    ln -sfn \
    "libusb-1.0.so.0" \
    "${OPT_LIB_DIR}/libusb-1.0.so"


fi



#############################################################
# Create system library links
#############################################################

log "Creating system library links"


ln -sfn \
"libhtraapi.so.${VERSION}" \
"/usr/local/lib/libhtraapi.so.${MAJOR}"


ln -sfn \
"libhtraapi.so.${MAJOR}" \
"/usr/local/lib/libhtraapi.so"



if [ -f "/usr/local/lib/libusb-1.0.so.0.2.0" ]; then


    ln -sfn \
    "libusb-1.0.so.0.2.0" \
    "/usr/local/lib/libusb-1.0.so.0"


    ln -sfn \
    "libusb-1.0.so.0" \
    "/usr/local/lib/libusb-1.0.so"


fi



#############################################################
# Update system
#############################################################

ldconfig


if command -v udevadm >/dev/null 2>&1; then

    udevadm control --reload-rules || true

    udevadm trigger || true

fi



#############################################################
# Finished
#############################################################

echo ""
echo "=============================================="
echo " HTRA API installation completed successfully"
echo "=============================================="
echo ""

echo "Architecture:"
echo "${ARCH}"

echo ""

echo "Library:"
echo "${LIB_FILE}"

echo ""

echo "Installed path:"
echo "/usr/local/lib"

echo ""
