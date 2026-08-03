#!/bin/bash

#############################################################
# HAROGIC SoapySDR Installation Script
#
# This script installs:
# 1. HAROGIC Device API SDK
# 2. HAROGIC SoapySDR driver
# 3. Calibration file configuration
#
# Tested on Ubuntu 22.04
#############################################################

set -e

echo "=============================================="
echo " HAROGIC SoapySDR Installation"
echo "=============================================="

# Check root permission
if [ "$EUID" -ne 0 ]; then
    echo "Error: Please run this script with sudo."
    echo "Example: sudo ./install.sh"
    exit 1
fi


# Get package path
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo ""
echo "Installation directory:"
echo "$SCRIPT_DIR"


#############################################################
# 1. Install dependencies
#############################################################

echo ""
echo "[1/5] Installing dependencies..."

apt-get update

apt-get install -y \
    build-essential \
    cmake \
    libsoapysdr-dev \
    soapysdr-tools \
    gnuradio


#############################################################
# 2. Install HAROGIC API SDK
#############################################################

echo ""
echo "[2/5] Installing HAROGIC API SDK..."

SDK_PATH="$SCRIPT_DIR/htra_sdk"

if [ ! -d "$SDK_PATH" ]; then
    echo "Error: htra_sdk folder not found."
    exit 1
fi


cd "$SDK_PATH"

chmod +x install_htraapi_lib.sh

./install_htraapi_lib.sh


#############################################################
# 3. Build SoapyHarogic driver
#############################################################

echo ""
echo "[3/5] Building SoapyHarogic driver..."


DRIVER_PATH="$SCRIPT_DIR/soapy-htra"


if [ ! -d "$DRIVER_PATH" ]; then
    echo "Error: soapy-htra folder not found."
    exit 1
fi


cd "$DRIVER_PATH"


mkdir -p build

cd build


cmake ..

make -j$(nproc)

make install


#############################################################
# 4. Configure calibration files
#############################################################

echo ""
echo "[4/5] Configuring calibration files..."


CAL_PATH="$SCRIPT_DIR/CalFile"
TARGET="/usr/bin/CalFile"


if [ ! -d "$CAL_PATH" ]; then
    echo "Error: CalFile folder not found."
    exit 1
fi


if [ -e "$TARGET" ] || [ -L "$TARGET" ]; then
    echo "Removing existing CalFile link..."
    rm -rf "$TARGET"
fi


ln -s "$CAL_PATH" "$TARGET"


echo "Calibration files linked:"
echo "$TARGET -> $CAL_PATH"


#############################################################
# 5. Verify installation
#############################################################

echo ""
echo "[5/5] Updating library cache..."

ldconfig


echo ""
echo "Checking SoapySDR installation..."

SoapySDRUtil --info


echo ""
echo "Searching HAROGIC device..."

SoapySDRUtil --find="driver=harogic" || true


echo ""
echo "=============================================="
echo " Installation completed successfully."
echo "=============================================="

echo ""
echo "You can verify the device by running:"
echo ""
echo "SoapySDRUtil --find=\"driver=harogic\""
echo ""