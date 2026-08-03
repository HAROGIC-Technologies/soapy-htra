#!/bin/bash

#############################################################
# GNU Radio WiFi Demodulation Extension Installation Script
#
# This script installs:
# 1. gr-foo
# 2. gr-ieee802-11
#
# Run the HAROGIC SoapySDR installation script first.
# Tested on Ubuntu 22.04.
#############################################################

set -e

echo "=============================================="
echo " Installing WiFi Demodulation Modules"
echo "=============================================="

# Check root permission
if [ "$EUID" -ne 0 ]; then
    echo "Error: Please run this script with sudo."
    echo "Example: sudo ./install_wifi.sh"
    exit 1
fi

WORK_DIR="$(cd "$(dirname "$0")" && pwd)"

echo ""
echo "Installation directory:"
echo "$WORK_DIR"

#############################################################
# 1. Install dependencies
#############################################################

echo ""
echo "[1/4] Installing WiFi dependencies..."

apt-get update

apt-get install -y \
    git \
    build-essential \
    cmake \
    pkg-config \
    libboost-all-dev \
    libgmp-dev \
    swig \
    python3-dev \
    python3-numpy \
    python3-scipy \
    python3-pybind11 \
    libfftw3-dev \
    libcppunit-dev \
    libgsl-dev \
    libsndfile1-dev \
    libvolk2-dev

#############################################################
# 2. Install gr-foo
#############################################################

echo ""
echo "[2/4] Installing gr-foo..."

cd "$WORK_DIR"

if [ ! -d "gr-foo" ]; then
    git clone https://github.com/bastibl/gr-foo.git
fi

cd "$WORK_DIR/gr-foo"

# Remove an incomplete or outdated CMake build cache
rm -rf build
mkdir build
cd build

cmake ..
make -j"$(nproc)"
make install
ldconfig

#############################################################
# 3. Install gr-ieee802-11
#############################################################

echo ""
echo "[3/4] Installing gr-ieee802-11..."

cd "$WORK_DIR"

if [ ! -d "gr-ieee802-11" ]; then
    git clone https://github.com/bastibl/gr-ieee802-11.git
fi

cd "$WORK_DIR/gr-ieee802-11"

# Remove an incomplete or outdated CMake build cache
rm -rf build
mkdir build
cd build

cmake ..
make -j"$(nproc)"
make install
ldconfig

#############################################################
# 4. Configure shared memory
#############################################################

echo ""
echo "[4/4] Configuring shared memory..."

sysctl -w kernel.shmmax=2147483648

if grep -qE '^[[:space:]]*kernel\.shmmax[[:space:]]*=' /etc/sysctl.conf; then
    sed -i 's/^[[:space:]]*kernel\.shmmax[[:space:]]*=.*/kernel.shmmax=2147483648/' /etc/sysctl.conf
else
    echo "kernel.shmmax=2147483648" >> /etc/sysctl.conf
fi

echo ""
echo "=============================================="
echo " WiFi modules installed successfully."
echo " Restart GNU Radio before use."
echo "=============================================="