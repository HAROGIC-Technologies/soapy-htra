# SoapyHarogic

SoapyHarogic integrates **HAROGIC HTRA-series receivers** with the vendor-neutral **SoapySDR** hardware abstraction layer. It translates standard SoapySDR operations such as device discovery, frequency and gain control, sample-rate configuration, and IQ streaming into HAROGIC HTRA API calls.

This allows compatible applications such as **GNU Radio**, Gqrx, SDR++, and custom SoapySDR programs to use a HAROGIC receiver through a common SDR interface instead of calling the device API directly.

## Contents

* [How it works](#how-it-works)
* [Features](#features)
* [System requirements](#system-requirements)
* [Repository layout](#repository-layout)
* [Before installation](#before-installation)
* [Installation](#installation)
* [Verify the installation](#verify-the-installation)
* [GNU Radio usage](#gnu-radio-usage)
* [GNU Radio examples](#gnu-radio-examples)
* [Driver configuration](#driver-configuration)
* [WLAN extension](#wlan-extension)
* [ADS-B extension and web map](#ads-b-extension-and-web-map)
* [Performance tuning](#performance-tuning)
* [Uninstallation](#uninstallation)
* [Troubleshooting](#troubleshooting)
* [License](#license)

## How it works

SoapySDR provides a standardized interface between SDR applications and hardware drivers. The software stack can be viewed as four layers:

1. **Application layer** — GNU Radio, Gqrx, SDR++, or a custom signal-processing application.
2. **SoapySDR interface layer** — common APIs for device discovery, tuning, gain, sample rate, bandwidth, and IQ streaming.
3. **SoapyHarogic and HTRA API layer** — converts SoapySDR calls into HAROGIC-specific API calls.
4. **HAROGIC hardware layer** — receives RF signals, tunes the receiver, samples IQ data, and transfers the samples to the host.

The result is a HAROGIC receiver that can be accessed as a standard SoapySDR RX device.

GNU Radio uses its built-in **Soapy Custom Source** block. No separate HTRA-specific GNU Radio source block is required.

## Features

* HAROGIC receiver support through SoapySDR
* USB and Ethernet physical interfaces
* USB device selection by device number or serial number
* Ethernet device selection by IPv4 address or serial number
* Ethernet network discovery through `Device\_GetNetworkDeviceList()`
* Direct Ethernet connection by IP address
* Receive-only IQ streaming through SoapySDR
* Configurable center frequency and device-supported sample rates
* Native IQ formats: `CS32`, `CS16`, and `CS8`
* Automatic IQ format selection based on sample rate
* Reference-level, preamplifier, and IF AGC controls
* Selectable RF input/antenna ports
* Low-noise and high-linearity gain strategies
* LO optimization modes for speed, spurs, or phase noise
* GNU Radio examples for basic reception and several demodulation workflows
* Optional WLAN and ADS-B extension installers

## System requirements

The reference environment is:

|Item|Reference requirement|
|-|-|
|Operating system|Ubuntu 22.04 or later|
|GNU Radio|3.9 or later|
|SoapySDR|0.8 or compatible|
|CPU architecture|x86\_64 or AArch64/ARM64|
|USB connection|USB 3.0-capable cable and port|
|Ethernet connection|Host and receiver on a reachable IPv4 network|

The included HTRA SDK installer currently supports:

* `x86\_64`
* `aarch64` / `arm64`

### Virtual machine users

When using Ubuntu in a virtual machine:

1. Confirm that the physical USB cable and host USB port support USB 3.0.
2. Configure the virtual machine USB controller for USB 3.1/3.2 when using a USB receiver.
3. Attach the HAROGIC USB device to the guest operating system rather than leaving it connected to the host.
4. For Ethernet devices, use a VM network configuration that can reach the receiver subnet. Bridged networking is normally more suitable for local network discovery than NAT.

For VMware USB operation, the setting is typically:

```text
VM > Settings > Hardware > USB Controller > USB Compatibility > USB 3.2
```

## Repository layout

```text
soapy-htra/
├── install.sh
├── uninstall.sh
├── README.md
├── DEVICE\_ARGUMENTS.md
├── CalFile/
├── htra\_sdk/
│   ├── install\_htraapi\_lib.sh
│   └── htraapi/
│       ├── configs/
│       ├── inc/
│       └── lib/
│           ├── x86\_64/
│           ├── aarch64/
│           └── armv7/
└── soapy-htra/
    ├── CMakeLists.txt
    ├── HarogicDevice.cpp
    ├── HarogicDevice.hpp
    ├── performance\_mode.sh
    └── examples/
        ├── basic\_example/
        ├── AM\_demod/
        ├── FM\_demod/
        ├── QPSK\_demod/
        ├── QAM\_demod/
        ├── WLAN/
        └── ADS-B/
```

## Before installation

### 1\. Add the calibration files

Copy the calibration files supplied with the receiver into:

```text
CalFile/
```

The installer creates the following symbolic link:

```text
/usr/bin/CalFile -> <repository>/CalFile
```

Do not move or delete the extracted repository after installation unless the `CalFile` link is recreated.

### 2\. Check the system architecture

```bash
uname -m
```

Supported results are:

```text
x86\_64
aarch64
arm64
```

### 3\. Connect the receiver

For USB devices, use a USB 3.0-capable cable and port. For devices that require an external power supply, connect the required power before starting IQ streaming.

For Ethernet devices, ensure that the host can reach the receiver IP address. The examples use:

```text
192.168.1.100
```

Change this value when your receiver uses another address.

## Installation

The current `install.sh` includes the required dependency installation. A separate dependency script is not required.

From the extracted project directory:

```bash
chmod +x install.sh
chmod +x uninstall.sh
sudo ./install.sh
```

The installer performs the following steps:

1. Runs `apt-get update`.
2. Installs the required build and runtime dependencies:

   * `build-essential`
   * `cmake`
   * `libsoapysdr-dev`
   * `soapysdr-tools`
   * `gnuradio`
3. Detects the host CPU architecture.
4. Installs the matching HAROGIC HTRA API runtime and SDK files.
5. Copies the HTRA SDK to `/opt/htraapi/`.
6. Installs the HTRA USB configuration and udev rule when supplied by the SDK.
7. Builds and installs the SoapyHarogic driver.
8. Removes the legacy `HTRA: Soapy IQ Source` GRC wrapper if it is still present.
9. Creates `/usr/bin/CalFile` as a symbolic link to the project `CalFile` directory.
10. Runs `ldconfig` and SoapySDR verification commands.

The installation can modify these system locations:

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

The SoapyHarogic module should be listed among the available SoapySDR modules.

### Search all HAROGIC devices

```bash
SoapySDRUtil --find="driver=harogic"
```

The driver searches both USB and Ethernet devices.

### Search USB only

```bash
SoapySDRUtil --find="driver=harogic,interface=USB"
```

### Search Ethernet only

```bash
SoapySDRUtil --find="driver=harogic,interface=ETH"
```

Ethernet automatic discovery uses the HTRA API function `Device\_GetNetworkDeviceList()`.

### Probe a USB receiver

```bash
SoapySDRUtil --probe="driver=harogic,interface=USB,device=0"
```

### Probe an Ethernet receiver

```bash
SoapySDRUtil --probe="driver=harogic,interface=ETH,ip=192.168.1.100"
```

Direct IP mode can be used even when Ethernet broadcast/network discovery does not return the receiver.

### Check the HTRA API library

```bash
ldconfig -p | grep htraapi
ls -l /usr/local/lib/libhtraapi.so\*
```

If the USB udev rules were installed while the receiver was already connected, unplug and reconnect the USB cable before testing again.

## GNU Radio usage

GNU Radio uses the built-in block:

```text
Core
└── Soapy
    └── Source
        └── Soapy Custom Source
```

Set:

```text
Driver = harogic
```

### USB

Set **Device arguments** to:

```text
interface=USB,device=0
```

### Ethernet

Set **Device arguments** to:

```text
interface=ETH,ip=192.168.1.100
```

No separate `HTRA: Soapy IQ Source` block is required.

## GNU Radio examples

Open the basic example with:

```bash
gnuradio-companion soapy-htra/examples/basic\_example/harogic\_example.grc
```

Other included examples are:

```bash
gnuradio-companion soapy-htra/examples/AM\_demod/AM\_demod.grc
gnuradio-companion soapy-htra/examples/FM\_demod/FM\_demod.grc
gnuradio-companion soapy-htra/examples/QPSK\_demod/QPSK\_demod.grc
gnuradio-companion soapy-htra/examples/QAM\_demod/QAM\_demod.grc
gnuradio-companion soapy-htra/examples/WLAN/WLAN\_demod.grc
gnuradio-companion soapy-htra/examples/ADS-B/ADSB\_demod.grc
```

Before running a flowgraph, verify:

* Device arguments
* Center frequency
* Sample rate
* Reference level
* RF input/antenna port
* Resampling ratio
* Demodulator parameters

### Example overview

|Example|Processing overview|
|-|-|
|Basic receiver|HAROGIC Soapy source with spectrum/time-domain visualization|
|AM|Rational resampling, spectrum/IQ display, AM demodulation, audio output|
|FM|Rational resampling, FM demodulation, spectrum/time display, audio output|
|QPSK|Resampling, filtering, symbol synchronization, constellation processing|
|16-QAM|Resampling, filtering, AGC, symbol synchronization, carrier recovery, constellation display|
|IEEE 802.11a WLAN|Packet detection, synchronization, FFT, equalization, MAC decoding|
|ADS-B 1090ES|Frame detection, pulse demodulation, message decoding, optional web-map display|

The demodulation examples are reference flowgraphs. Their parameters may need adjustment for the connected receiver, RF environment, signal level, and GNU Radio version.

## Driver configuration

### Device arguments

|Key|Example|Description|
|-|-|-|
|`driver`|`harogic`|Selects the HAROGIC SoapySDR driver.|
|`interface`|`USB` / `ETH`|Selects the physical interface. USB is the default when no interface is specified.|
|`device`|`0`|USB device number.|
|`ip`|`192.168.1.100`|Ethernet IPv4 address.|
|`serial`|`<device-serial>`|Optional serial-number filter.|

Accepted aliases are also available:

```text
physical\_interface

device\_number
device\_num
index

ip\_address
address
```

Examples:

```text
driver=harogic,interface=USB,device=0
```

```text
driver=harogic,interface=ETH,ip=192.168.1.100
```

```text
driver=harogic,interface=USB,serial=<device-serial>
```

```text
driver=harogic,interface=ETH,serial=<device-serial>
```

When Ethernet is selected by serial number without an IP address, the driver uses `Device\_GetNetworkDeviceList()` to resolve the receiver address.

### Stream arguments

|Key|Values|Default|Description|
|-|-|-|-|
|`native\_format`|`AUTO`, `CS32`, `CS16`, `CS8`, `CF32`|`AUTO`|Requests the native IQ format. `CF32` falls back to a device-supported integer format when necessary.|

Recommended general-purpose setting:

```text
native\_format=CS16
```

In `AUTO` mode, the current driver selects a native IQ format according to the requested sample rate.

### RF ports

The driver exposes the following receive-port names:

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

The driver exposes reference-level, preamplifier, and IF AGC controls through SoapySDR.

For strong signals, increase the reference level to reduce internal gain and avoid overload. For weak signals, use a lower reference level to increase sensitivity.

### Device-specific settings

|Key|Values|Description|
|-|-|-|
|`gain\_strategy`|`Low Noise`, `High Linearity`|Selects sensitivity-oriented or strong-signal-oriented gain distribution.|
|`lo\_mode`|`Auto`, `Speed`, `Spurs`, `Phase Noise`|Selects the local-oscillator optimization strategy.|

Example settings string:

```text
gain\_strategy=High Linearity,lo\_mode=Spurs
```

## WLAN extension

The WLAN example requires additional GNU Radio out-of-tree modules.

Run the main SoapyHarogic installation first, then:

```bash
cd soapy-htra/examples/WLAN
chmod +x install\_wifi.sh
sudo ./install\_wifi.sh
```

Restart GNU Radio Companion after installation.

## ADS-B extension and web map

Run the main SoapyHarogic installation first, then:

```bash
cd soapy-htra/examples/ADS-B
chmod +x install\_adsb.sh
./install\_adsb.sh
```

Do not run `install\_adsb.sh` as root unless the script specifically requests it internally.

Open the flowgraph with:

```bash
gnuradio-companion ADSB\_demod.grc
```

When the bundled web application is installed, start it from the ADS-B extension directory according to the installer output.

## Performance tuning

High sample rates place additional load on the USB controller, memory subsystem, and CPU. The included script can apply temporary performance-related settings:

```bash
cd soapy-htra
chmod +x performance\_mode.sh
sudo ./performance\_mode.sh
```

The script can apply options such as CPU performance mode, USB autosuspend control, increased usbfs memory, and interrupt-affinity adjustments.

Most runtime settings can be restored by rebooting.

## Uninstallation

To remove the SoapyHarogic driver while keeping the HTRA API runtime and SDK:

```bash
chmod +x uninstall.sh
sudo ./uninstall.sh
```

Do not run the script with `sudo sh ./uninstall.sh`; it is a Bash script and should be executed directly or with `sudo bash ./uninstall.sh`.

To also remove the HTRA SDK/runtime installed by this project:

```bash
sudo ./uninstall.sh --purge-sdk
```

The uninstaller intentionally keeps generic system dependency libraries that may be used by other applications.

## Troubleshooting

### No HAROGIC USB device is found

1. Confirm the receiver is powered and connected to USB 3.0.
2. In a virtual machine, confirm that the device is attached to the guest OS.
3. Reload the udev rules:

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

4. Check:

```bash
lsusb
SoapySDRUtil --find="driver=harogic,interface=USB"
```

### No HAROGIC Ethernet device is found automatically

First verify basic network reachability:

```bash
ping 192.168.1.100
```

Then try direct-IP probing:

```bash
SoapySDRUtil --probe="driver=harogic,interface=ETH,ip=192.168.1.100"
```

Automatic Ethernet discovery uses `Device\_GetNetworkDeviceList()` and may be affected by the host network interface, firewall, virtual-machine NAT, or broadcast/network-discovery restrictions. Direct IP operation does not require automatic discovery to succeed.

### The SoapyHarogic module is not listed

```bash
SoapySDRUtil --info
find /usr/local -type f -name 'libHarogicSupport.so\*' 2>/dev/null
```

Rebuild from a clean directory if necessary:

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
ls -l /usr/local/lib/libhtraapi.so\*
ls -l /opt/htraapi/lib/x86\_64/libhtraapi.so\* 2>/dev/null
ls -l /opt/htraapi/lib/aarch64/libhtraapi.so\* 2>/dev/null
```

### Calibration files are not found

```bash
ls -ld /usr/bin/CalFile
readlink -f /usr/bin/CalFile
```

If the repository was moved, recreate the link from the project root:

```bash
sudo rm -rf /usr/bin/CalFile
sudo ln -s "$(pwd)/CalFile" /usr/bin/CalFile
```

### IF overflow or repeated overflow messages

Increase the receiver reference level until the overflow stops. Also consider disabling the preamplifier or selecting the `High Linearity` gain strategy.

### Timeouts, overflows, or interrupted IQ streaming

* Use a direct USB 3.0 port rather than an unpowered hub.
* Close other high-bandwidth USB applications.
* Try `native\_format=CS16`, or a lower-bit-width format at very high sample rates.
* Reduce the sample rate.
* Run `performance\_mode.sh` before starting the flowgraph.
* Confirm that the VM or host USB controller is operating in USB 3.x mode.

## License

The SoapyHarogic driver is distributed under the **GNU Lesser General Public License v2.1 or later**. See [`soapy-htra/LICENSE`](soapy-htra/LICENSE) for the complete license text.

