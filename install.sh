#!/bin/bash

#############################################################
# HAROGIC SoapySDR Installation Script
#
# This edition installs only the SoapyHarogic device driver.
# GNU Radio uses its built-in Soapy Custom Source block.
#############################################################

set -e

echo "=============================================="
echo " HAROGIC SoapySDR Installation"
echo " Soapy Custom Source Edition"
echo "=============================================="

if [ "$EUID" -ne 0 ]; then
    echo "Error: Please run this script with sudo."
    echo "Example: sudo ./install.sh"
    exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo ""
echo "Installation directory:"
echo "$SCRIPT_DIR"

#############################################################
# 1. Install dependencies
#############################################################

echo ""
echo "[1/6] Installing dependencies..."

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
echo "[2/6] Installing HAROGIC API SDK..."

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
echo "[3/6] Building SoapyHarogic driver..."

DRIVER_PATH="$SCRIPT_DIR/soapy-htra"
if [ ! -d "$DRIVER_PATH" ]; then
    echo "Error: soapy-htra folder not found."
    exit 1
fi

cd "$DRIVER_PATH"
rm -rf build
cmake -S . -B build
cmake --build build --parallel "$(nproc)"
cmake --install build

#############################################################
# 4. Remove legacy HTRA GRC wrapper
#############################################################

echo ""
echo "[4/6] Removing legacy HTRA GNU Radio wrapper if present..."

GRC_FILE="soapy_harogic_source.block.yml"
rm -f "/usr/share/gnuradio/grc/blocks/$GRC_FILE"
rm -f "/usr/local/share/gnuradio/grc/blocks/$GRC_FILE"

if command -v gnuradio-config-info >/dev/null 2>&1; then
    GNU_RADIO_PREFIX="$(gnuradio-config-info --prefix 2>/dev/null || true)"
    if [ -n "$GNU_RADIO_PREFIX" ]; then
        rm -f "$GNU_RADIO_PREFIX/share/gnuradio/grc/blocks/$GRC_FILE"
    fi
fi

echo "GNU Radio will use: Core -> Soapy -> Source -> Soapy Custom Source"

#############################################################
# 5. Configure calibration files
#############################################################

echo ""
echo "[5/6] Configuring calibration files..."

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
# 6. Verify installation
#############################################################

echo ""
echo "[6/6] Updating library cache and verifying SoapySDR..."

ldconfig

SoapySDRUtil --info

echo ""
echo "Searching HAROGIC devices (USB + ETH)..."
SoapySDRUtil --find="driver=harogic" || true

echo ""
echo "=============================================="
echo " Installation completed successfully."
echo "=============================================="

echo ""
echo "GNU Radio block:"
echo "  Core -> Soapy -> Source -> Soapy Custom Source"
echo ""
echo "Driver:"
echo "  harogic"
echo ""
echo "USB Device arguments:"
echo "  interface=USB,device=0"
echo ""
echo "Ethernet Device arguments:"
echo "  interface=ETH,ip=192.168.1.100"
echo ""
echo "Probe examples:"
echo "  SoapySDRUtil --probe=\"driver=harogic,interface=USB,device=0\""
echo "  SoapySDRUtil --probe=\"driver=harogic,interface=ETH,ip=192.168.1.100\""
echo ""
