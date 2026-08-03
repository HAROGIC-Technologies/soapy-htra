# SoapyHarogic

SoapyHarogic integrates **HAROGIC HTRA-series receivers** with the vendor-neutral **SoapySDR** hardware abstraction layer. It translates standard SoapySDR operations—device discovery, frequency and gain control, sample-rate configuration, and IQ streaming—into HAROGIC HTRA API calls.

This allows compatible applications such as **GNU Radio**, Gqrx, SDR++, and custom SoapySDR programs to use a HAROGIC receiver through a common SDR interface instead of calling the device API directly.

## Contents

- [How it works](#how-it-works)
- [Features](#features)
- [System requirements](#system-requirements)
- [Repository layout](#repository-layout)
- [Before installation](#before-installation)
- [Automatic installation](#automatic-installation)
- [Verify the installation](#verify-the-installation)
- [Manual installation](#manual-installation)
- [GNU Radio examples](#gnu-radio-examples)
- [WLAN extension](#wlan-extension)
- [ADS-B extension and web map](#ads-b-extension-and-web-map)
- [Driver configuration](#driver-configuration)
- [Performance tuning](#performance-tuning)
- [Troubleshooting](#troubleshooting)
- [License](#license)

## How it works

SoapySDR provides a standardized interface between SDR applications and hardware drivers. The software stack can be viewed as four layers:

1. **Application layer** — GNU Radio, Gqrx, SDR++, or a custom signal-processing application.
2. **SoapySDR interface layer** — common APIs for device discovery, tuning, gain, sample rate, bandwidth, and IQ streaming.
3. **SoapyHarogic and HTRA API layer** — converts SoapySDR calls into HAROGIC-specific API calls.
4. **HAROGIC hardware layer** — receives RF signals, tunes the receiver, samples IQ data, and transfers the samples to the host.

The result is a HAROGIC receiver that can be accessed as a standard SoapySDR RX device.

## Features

- Automatic discovery of connected HAROGIC devices
- Receive-only IQ streaming through SoapySDR
- Device selection by serial number
- Configurable center frequency and device-supported sample rates
- Native IQ formats: `CS32`, `CS16`, and `CS8`
- Automatic format selection based on sample rate
- Reference-level, preamplifier, and IF AGC controls
- Selectable RF input/antenna ports
- Low-noise and high-linearity gain strategies
- LO optimization modes for speed, spurs, or phase noise
- GNU Radio examples for basic reception and several demodulation workflows
- Optional WLAN and ADS-B extension installers

## System requirements

The reference workflow in the accompanying guide uses the following environment:

| Item | Reference requirement |
| --- | --- |
| Operating system | Ubuntu 22.04 or later |
| GNU Radio | 3.9 or later |
| CPU architecture | x86_64 in the documented example |
| USB connection | USB 3.0-capable cable and port |

The included HTRA SDK installer currently accepts:

- `x86_64`
- `aarch64` / `arm64`

Although an `armv7` library directory is included in the package, the current `install_htraapi_lib.sh` script does not install ARMv7 libraries and will report an unsupported architecture.

The SoapyHarogic CMake project requires **SoapySDR 0.8** or a compatible installation.

### Virtual machine users

When using Ubuntu in a virtual machine:

1. Confirm that the physical USB cable and host USB port support USB 3.0.
2. Configure the virtual machine USB controller for USB 3.1/3.2.
3. Attach the HAROGIC device to the guest operating system rather than leaving it connected to the host.

For VMware, the setting is typically:

```text
VM > Settings > Hardware > USB Controller > USB Compatibility > USB 3.2
```

## Repository layout

```text
soapy-harogic/
├── install.sh
├── CalFile/
├── htra_sdk/
│   ├── install_htraapi_lib.sh
│   └── htraapi/
│       ├── configs/
│       ├── inc/
│       └── lib/
│           ├── x86_64/
│           ├── aarch64/
│           └── armv7/
└── soapy-htra/
    ├── CMakeLists.txt
    ├── HarogicDevice.cpp
    ├── HarogicDevice.hpp
    ├── performance_mode.sh
    └── examples/
        ├── basic_example/
        ├── AM_demod/
        ├── FM_demod/
        ├── QPSK_demod/
        ├── QAM_demod/
        ├── WLAN/
        └── ADS-B/
```

## Before installation

### 1. Add the calibration files

Copy the calibration files supplied with the receiver into:

```text
soapy-harogic/CalFile/
```

The main installer creates this symbolic link:

```text
/usr/bin/CalFile -> <repository>/CalFile
```

Do not move or delete the extracted repository after installation unless you recreate the link. Otherwise, the calibration path will become invalid.

> The installer removes any existing file, directory, or symbolic link at `/usr/bin/CalFile` before creating the new link. Back up an existing calibration directory first when necessary.

### 2. Check the system architecture

```bash
uname -m
```

Expected supported results are `x86_64`, `aarch64`, or `arm64`.

### 3. Connect the receiver

Use a USB 3.0-capable cable and port. For devices that require an external power connector, connect the required power supply before starting IQ streaming.

## Automatic installation

Open a terminal in the extracted repository root and run:

```bash
chmod +x install.sh
sudo ./install.sh
```

The main installer performs the following operations:

1. Installs build tools, SoapySDR development files, SoapySDR utilities, and GNU Radio.
2. Detects the host architecture and installs the matching HAROGIC HTRA API libraries.
3. Copies the architecture-specific libraries to `/usr/local/lib/`.
4. Copies the complete HTRA SDK to `/opt/htraapi/`.
5. Installs `htrausb.conf` and the HAROGIC udev rule.
6. Builds and installs the SoapyHarogic module with CMake.
7. Creates `/usr/bin/CalFile` as a symbolic link to the repository calibration directory.
8. Refreshes the dynamic linker cache and runs SoapySDR verification commands.

The installation modifies these system locations:

```text
/etc/htrausb.conf
/etc/udev/rules.d/htra-cyusb.rules
/opt/htraapi/
/usr/local/lib/
/usr/local/lib/SoapySDR/
/usr/bin/CalFile
```

## Verify the installation

### Display SoapySDR information

```bash
SoapySDRUtil --info
```

### Find a HAROGIC receiver

```bash
SoapySDRUtil --find="driver=harogic"
```

A successful result contains the HAROGIC driver, device label, and serial number, for example:

```text
Found device 0
  driver = harogic
  label  = Harogic <serial number>
  serial = <serial number>
```

### Probe the device

```bash
SoapySDRUtil --probe="driver=harogic"
```

To select one receiver when multiple units are connected:

```bash
SoapySDRUtil --probe="driver=harogic,serial=<device-serial>"
```

### Check the HTRA API library

```bash
ldconfig -p | grep htraapi
ls -l /usr/local/lib/libhtraapi.so*
```

If the udev rules were installed while the receiver was already connected, unplug and reconnect the USB cable before testing again.

## Manual installation

Use this procedure when the automatic installer stops at a specific stage.

### 1. Install the base dependencies

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libsoapysdr-dev \
    soapysdr-tools \
    gnuradio
```

### 2. Install the HAROGIC HTRA SDK

```bash
cd htra_sdk
chmod +x install_htraapi_lib.sh
sudo ./install_htraapi_lib.sh
cd ..
```

The SDK installer:

- detects `x86_64` or `aarch64/arm64`;
- selects the highest-sorted `libhtraapi.so.*` file in the matching architecture directory;
- copies the architecture libraries to `/usr/local/lib/`;
- copies the complete SDK to `/opt/htraapi/`;
- creates the required `libhtraapi.so` and `libusb-1.0.so` symbolic links;
- installs the USB configuration and udev rule;
- runs `ldconfig` and reloads udev rules.

### 3. Build the SoapyHarogic module

```bash
cd soapy-htra
rm -rf build
cmake -S . -B build
cmake --build build --parallel "$(nproc)"
sudo cmake --install build
sudo ldconfig
cd ..
```

### 4. Configure the calibration path

From the repository root:

```bash
sudo rm -rf /usr/bin/CalFile
sudo ln -s "$(pwd)/CalFile" /usr/bin/CalFile
ls -ld /usr/bin/CalFile
```

## GNU Radio examples

The included flowgraphs use the HAROGIC receiver as an IQ source through the SoapySDR interface. Open a `.grc` file with GNU Radio Companion:

```bash
gnuradio-companion soapy-htra/examples/basic_example/harogic_example.grc
```

Other flowgraphs can be opened in the same way:

```bash
gnuradio-companion soapy-htra/examples/AM_demod/AM_demod.grc
gnuradio-companion soapy-htra/examples/FM_demod/FM_demod.grc
gnuradio-companion soapy-htra/examples/QPSK_demod/QPSK_demod.grc
gnuradio-companion soapy-htra/examples/QAM_demod/QAM_demod.grc
gnuradio-companion soapy-htra/examples/WLAN/WLAN_demod.grc
gnuradio-companion soapy-htra/examples/ADS-B/ADSB_demod.grc
```

Before running a flowgraph, check its center frequency, sample rate, reference level, antenna port, resampling ratio, and demodulator parameters.

### Example overview

| Example | Processing overview | Reference signal used in the guide |
| --- | --- | --- |
| Basic receiver | HAROGIC Soapy source with spectrum/time-domain visualization | User-defined |
| AM | Rational resampling, spectrum and IQ display, AM demodulation, audio output | 1 GHz, -20 dBm, 1 kHz sine modulation, 50% depth |
| FM | Rational resampling, FM demodulation, spectrum/time display, audio output | 1 GHz, -20 dBm, 3 kHz sine modulation, 75 kHz deviation |
| QPSK | Resampling, root-raised-cosine filtering, symbol synchronization, constellation decoding | QPSK-modulated RF input |
| 16-QAM | Resampling, RRC filtering, AGC, symbol synchronization, carrier recovery, constellation display | 1 GHz, -80 dBm, 500 kSymbols/s, RRC roll-off 0.35 |
| IEEE 802.11a WLAN | Packet detection, short/long synchronization, FFT, equalization, MAC decoding | 2.412 GHz, -40 dBm, 12 Mb/s, QPSK with BCC 1/2 |
| ADS-B 1090ES | Frame detection, pulse demodulation, message decoding, ZMQ publication, web-map display | 1090 MHz ADS-B |

The demodulation examples are reference flowgraphs. Their parameters may need to be adjusted for the connected receiver model, RF environment, signal level, and GNU Radio version.

## WLAN extension

The WLAN installer builds and installs `gr-foo` and `gr-ieee802-11`, then sets the System V shared-memory maximum to 2 GiB.

Run the main SoapyHarogic installation first. Then execute:

```bash
cd soapy-htra/examples/WLAN
chmod +x install_wifi.sh
sudo ./install_wifi.sh
```

The script must be run with `sudo`. It installs dependencies, clones the two GNU Radio out-of-tree modules when they are not already present, rebuilds them from clean `build` directories, and updates `/etc/sysctl.conf` with:

```text
kernel.shmmax=2147483648
```

Restart GNU Radio Companion after installation.

Decoded WLAN packets can be exported to Wireshark or saved as a PCAP file when the flowgraph is configured accordingly.

## ADS-B extension and web map

The ADS-B installer builds `gr-adsb`, installs the GNU Radio module, creates a Python virtual environment for the bundled web application, and installs compatible Flask/Socket.IO dependencies.

> Run this installer as a normal user. Do **not** run it with `sudo`; the script calls `sudo` only for the operations that require elevated privileges.

```bash
cd soapy-htra/examples/ADS-B
chmod +x install_adsb.sh
./install_adsb.sh
```

By default, the installer checks out the `maint-3.10` branch. You can override the branch or build parallelism before running it:

```bash
GR_ADSB_REF=maint-3.10 BUILD_JOBS=4 ./install_adsb.sh
```

Open and run the ADS-B flowgraph:

```bash
gnuradio-companion ADSB_demod.grc
```

In another terminal, start the web server:

```bash
cd gr-adsb/web
source venv/bin/activate
python3 webserver.py
```

Then open:

```text
http://127.0.0.1:5000
```

The decoded ADS-B messages can include the ICAO address, callsign, altitude, speed, heading, and position. The web interface displays available aircraft positions on a map in real time.

## Driver configuration

### Device arguments

| Key | Example | Description |
| --- | --- | --- |
| `driver` | `harogic` | Selects the HAROGIC SoapySDR driver. |
| `serial` | `5430500300470031` | Selects a specific receiver when multiple devices are connected. |
| `label` | `Harogic ...` | Read-only label returned during discovery. |

Example:

```text
driver=harogic,serial=5430500300470031
```

### Stream arguments

| Key | Values | Default | Description |
| --- | --- | --- | --- |
| `native_format` | `AUTO`, `CS32`, `CS16`, `CS8`, `CF32` | `AUTO` | Requests the native IQ data format. `CF32` is not supported by HTRA IQ streaming and falls back to `CS16`. |

Recommended general-purpose setting:

```text
native_format=CS16
```

In `AUTO` mode, the current driver selects:

- `CS16` at sample rates up to and including 60 MS/s;
- `CS8` above 60 MS/s to reduce USB bandwidth.

### RF ports

The current driver exposes these receive-port names:

```text
External
Internal
ANT
T/R
SWR
INT
```

Available physical ports depend on the connected HAROGIC model.

### Gain controls

| Element | Current driver range | Description |
| --- | --- | --- |
| `REF` | `-100` to `+7` dBm | Receiver reference level and primary front-end level control. |
| `PREAMP` | `0` or `1` | Enables automatic preamplifier use or forces it off. |
| `IF_AGC` | `0` or `1` | Enables or disables IF automatic gain control. |

For strong signals, increase the reference level to reduce internal gain and avoid overload. For weak signals, use a lower reference level to increase sensitivity.

### Device-specific settings

| Key | Values | Description |
| --- | --- | --- |
| `gain_strategy` | `Low Noise`, `High Linearity` | Selects sensitivity-oriented or strong-signal-oriented gain distribution. |
| `lo_mode` | `Auto`, `Speed`, `Spurs`, `Phase Noise` | Selects the local-oscillator optimization strategy. |

Example GNU Radio settings string:

```text
gain_strategy=High Linearity,lo_mode=Spurs
```

## Performance tuning

High sample rates place heavy load on the USB controller, memory subsystem, and CPU. The included interactive script can apply several temporary performance optimizations:

```bash
cd soapy-htra
chmod +x performance_mode.sh
sudo ./performance_mode.sh
```

The menu can:

- set all CPU cores to the `performance` governor;
- disable USB autosuspend;
- increase the usbfs memory limit to 1000 MB;
- pin the first detected xHCI interrupt to CPU core 1;
- disable DRM KMS polling;
- apply all changes together;
- revert selected settings toward common power-saving defaults.

Most changes are runtime settings and can be reset by rebooting. The script also notes that IRQ affinity and the usbfs memory setting are best restored by a reboot.

## Troubleshooting

### No HAROGIC device is found

1. Confirm that the device is powered and connected through USB 3.0.
2. In a virtual machine, confirm that the USB device is attached to the guest and that USB 3.1/3.2 compatibility is selected.
3. Reload the udev rules and reconnect the device:

   ```bash
   sudo udevadm control --reload-rules
   sudo udevadm trigger
   ```

4. Check USB enumeration and SoapySDR discovery:

   ```bash
   lsusb
   SoapySDRUtil --find="driver=harogic"
   ```

### The SoapyHarogic module is not listed

```bash
SoapySDRUtil --info
find /usr/local/lib -type f -iname '*Harogic*' -o -iname '*harogic*'
```

Rebuild from a clean CMake directory when necessary:

```bash
cd soapy-htra
rm -rf build
cmake -S . -B build
cmake --build build --parallel "$(nproc)"
sudo cmake --install build
sudo ldconfig
```

### The HTRA API library cannot be loaded

```bash
ldconfig -p | grep htraapi
ls -l /usr/local/lib/libhtraapi.so*
ls -l /opt/htraapi/lib/x86_64/libhtraapi.so* 2>/dev/null
ls -l /opt/htraapi/lib/aarch64/libhtraapi.so* 2>/dev/null
```

Run the SDK installer again if the links are missing.

### Calibration files are not found

```bash
ls -ld /usr/bin/CalFile
readlink -f /usr/bin/CalFile
```

If the repository was moved, recreate the link from the new repository root:

```bash
sudo rm -rf /usr/bin/CalFile
sudo ln -s "$(pwd)/CalFile" /usr/bin/CalFile
```

### IF overflow or repeated `D` messages

An IF overflow indicates that the received signal is too strong for the current level configuration. Increase `REF`—for example, change it from `-50` dBm toward `-30` dBm—until the overflow stops. Also consider disabling the preamplifier or selecting `High Linearity` gain strategy.

### Timeouts, overflows, or interrupted IQ streaming

- Use a direct USB 3.0 port rather than an unpowered hub.
- Close other high-bandwidth USB applications.
- Try `native_format=CS16`, or `CS8` at very high sample rates.
- Reduce the sample rate.
- Run `performance_mode.sh` before starting the flowgraph.
- Confirm that the VM or host USB controller is operating in USB 3.x mode.

### WLAN installer permission error

Run it with `sudo`:

```bash
sudo ./install_wifi.sh
```

### ADS-B installer rejects root execution

Run it without `sudo`:

```bash
./install_adsb.sh
```

## License

The SoapyHarogic driver is distributed under the **GNU Lesser General Public License v2.1 or later**. See [`soapy-htra/LICENSE`](soapy-htra/LICENSE) for the complete license text.
