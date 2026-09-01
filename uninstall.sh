#!/usr/bin/env bash
set -Eeuo pipefail

#############################################################
# HAROGIC SoapySDR Uninstallation Script
#
# Default behavior:
#   1. Remove the manually installed SoapyHarogic driver
#   2. Remove the optional GNU Radio GRC wrapper block
#   3. Remove the /usr/bin/CalFile symlink created by install.sh
#   4. Keep HTRA SDK/runtime libraries and system packages
#
# Optional:
#   sudo ./uninstall.sh --purge-sdk
#   Also removes the HTRA API SDK copy and HTRA-specific files.
#   Generic dependency libraries are intentionally preserved.
#############################################################

log()
{
    printf '\n[UNINSTALL] %s\n' "$*"
}

warn()
{
    printf '\n[UNINSTALL] WARNING: %s\n' "$*" >&2
}

fail()
{
    printf '\n[UNINSTALL] ERROR: %s\n' "$*" >&2
    exit 1
}

if [ "${EUID}" -ne 0 ]; then
    fail "Please run this script with sudo. Example: sudo ./uninstall.sh"
fi

PURGE_SDK=0
case "${1:-}" in
    "")
        ;;
    --purge-sdk)
        PURGE_SDK=1
        ;;
    -h|--help)
        cat <<'HELP'
Usage:
  sudo ./uninstall.sh
      Remove the SoapyHarogic driver and GNU Radio wrapper only.
      Keep the HTRA SDK/runtime libraries.

  sudo ./uninstall.sh --purge-sdk
      Also remove /opt/htraapi, libhtraapi files, and HTRA-specific
      configuration files. Generic dependency libraries are preserved.
HELP
        exit 0
        ;;
    *)
        fail "Unknown option: $1"
        ;;
esac

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REMOVED_COUNT=0

remove_file_if_exists()
{
    local path="$1"
    if [ -e "$path" ] || [ -L "$path" ]; then
        rm -f -- "$path"
        echo "Removed: $path"
        REMOVED_COUNT=$((REMOVED_COUNT + 1))
    fi
}

remove_tree_if_exists()
{
    local path="$1"
    if [ -d "$path" ] || [ -L "$path" ]; then
        rm -rf -- "$path"
        echo "Removed: $path"
        REMOVED_COUNT=$((REMOVED_COUNT + 1))
    fi
}

#############################################################
# 1. Remove files recorded by the local CMake install manifest
#############################################################

log "Removing installed SoapyHarogic files"

for manifest in \
    "$SCRIPT_DIR/soapy-htra/build/install_manifest.txt" \
    "$SCRIPT_DIR/build/install_manifest.txt"
do
    if [ -f "$manifest" ]; then
        echo "Using install manifest: $manifest"
        while IFS= read -r installed_path; do
            case "$installed_path" in
                *libHarogicSupport.so*|*soapy_harogic_source.block.yml|*SoapyHarogic.pc)
                    remove_file_if_exists "$installed_path"
                    ;;
            esac
        done < "$manifest"
    fi
done

#############################################################
# 2. Fallback cleanup for the old default /usr/local install
#############################################################

# SOAPY_SDR_MODULE_UTIL normally installs HarogicSupport below
# /usr/local/lib*/SoapySDR/modulesX.Y when CMAKE_INSTALL_PREFIX
# is left at its default value.
while IFS= read -r module_file; do
    remove_file_if_exists "$module_file"
done < <(
    find /usr/local/lib /usr/local/lib64 \
        -type f -o -type l 2>/dev/null | \
        grep -E '/SoapySDR/modules[^/]*/libHarogicSupport\.so([.][^/]*)?$' || true
)

# Compatibility with possible non-standard manual installs below /usr/local.
while IFS= read -r module_file; do
    remove_file_if_exists "$module_file"
done < <(
    find /usr/local \
        \( -type f -o -type l \) \
        -name 'libHarogicSupport.so*' 2>/dev/null || true
)

# Optional pkg-config file used by some older project revisions.
remove_file_if_exists "/usr/local/lib/pkgconfig/SoapyHarogic.pc"
remove_file_if_exists "/usr/local/lib64/pkgconfig/SoapyHarogic.pc"

#############################################################
# 3. Remove optional GNU Radio GRC wrapper
#############################################################

log "Removing GNU Radio wrapper block if present"

GRC_FILE="soapy_harogic_source.block.yml"

remove_file_if_exists "/usr/share/gnuradio/grc/blocks/$GRC_FILE"
remove_file_if_exists "/usr/local/share/gnuradio/grc/blocks/$GRC_FILE"

if command -v gnuradio-config-info >/dev/null 2>&1; then
    GNU_RADIO_PREFIX="$(gnuradio-config-info --prefix 2>/dev/null || true)"
    if [ -n "$GNU_RADIO_PREFIX" ]; then
        remove_file_if_exists "$GNU_RADIO_PREFIX/share/gnuradio/grc/blocks/$GRC_FILE"
    fi
fi

#############################################################
# 4. Remove the calibration link created by install.sh
#############################################################

log "Removing CalFile link"

if [ -L /usr/bin/CalFile ]; then
    echo "Current link: /usr/bin/CalFile -> $(readlink /usr/bin/CalFile)"
    rm -f /usr/bin/CalFile
    echo "Removed: /usr/bin/CalFile"
    REMOVED_COUNT=$((REMOVED_COUNT + 1))
elif [ -e /usr/bin/CalFile ]; then
    warn "/usr/bin/CalFile exists but is not a symbolic link; leaving it unchanged."
fi

#############################################################
# 5. Optional HTRA SDK cleanup
#############################################################

if [ "$PURGE_SDK" -eq 1 ]; then
    log "Purging HTRA API SDK files"

    remove_tree_if_exists "/opt/htraapi"

    # Remove only HTRA-specific runtime files. Do not remove generic
    # libusb, libgomp, MKL, liquid, etc. because other applications may
    # depend on them.
    while IFS= read -r htra_lib; do
        remove_file_if_exists "$htra_lib"
    done < <(
        find /usr/local/lib /usr/local/lib64 \
            \( -type f -o -type l \) \
            -name 'libhtraapi.so*' 2>/dev/null || true
    )

    remove_file_if_exists "/etc/htrausb.conf"
    remove_file_if_exists "/etc/udev/rules.d/htra-cyusb.rules"

    if command -v udevadm >/dev/null 2>&1; then
        udevadm control --reload-rules || true
        udevadm trigger || true
    fi
else
    log "Keeping HTRA SDK/runtime libraries"
    echo "The HTRA API under /opt/htraapi and /usr/local/lib is preserved."
    echo "This is recommended when you are about to install the new USB/ETH version."
fi

#############################################################
# 6. Refresh linker cache and verify cleanup
#############################################################

log "Refreshing library cache"
ldconfig

log "Checking for remaining HarogicSupport modules"

REMAINING="$(find /usr/local \
    \( -type f -o -type l \) \
    -name 'libHarogicSupport.so*' 2>/dev/null || true)"

if [ -n "$REMAINING" ]; then
    warn "Some HarogicSupport files still exist:"
    printf '%s\n' "$REMAINING"
else
    echo "No manually installed libHarogicSupport.so files remain under /usr/local."
fi

echo ""
echo "=============================================="
echo " SoapyHarogic uninstall completed"
echo "=============================================="
echo "Removed entries: $REMOVED_COUNT"

echo ""
if [ "$PURGE_SDK" -eq 0 ]; then
    echo "Next step: install the new USB/ETH version with:"
    echo "  sudo ./install.sh"
else
    echo "HTRA SDK files were also purged."
fi
