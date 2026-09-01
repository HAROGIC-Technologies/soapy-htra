# SoapyHarogic - USB / Ethernet Device-Arguments Edition

This package integrates HAROGIC HTRA receivers with SoapySDR and GNU Radio.

The GNU Radio integration uses the built-in **Soapy Custom Source** block. No
HTRA-specific GNU Radio source block is installed by this package.

## Architecture

```text
GNU Radio
  -> Soapy Custom Source
  -> SoapySDR
  -> HarogicSupport module
  -> HTRA API
  -> HAROGIC receiver
```

The SoapySDR driver is implemented mainly in:

```text
soapy-htra/HarogicDevice.cpp
soapy-htra/HarogicDevice.hpp
```

CMake builds these sources into the loadable SoapySDR module
`libHarogicSupport.so`.

## Supported physical interfaces

The device type is selected through the GNU Radio **Device arguments** field or
through normal SoapySDR keyword arguments.

### USB

```text
Driver: harogic
Device arguments: interface=USB,device=0
```

### Ethernet

```text
Driver: harogic
Device arguments: interface=ETH,ip=192.168.1.100
```

Accepted aliases:

```text
interface / physical_interface

device / device_number / device_num / index

ip / ip_address / address

serial
```

Selection rules:

- `interface=USB` selects USB.
- `interface=ETH` selects Ethernet.
- If `interface` is omitted but `ip` is present, Ethernet is selected.
- If neither `interface` nor `ip` is present, USB is the backward-compatible default.

## Ethernet discovery

Automatic Ethernet discovery uses the HTRA API function:

```text
Device_GetNetworkDeviceList()
```

Direct-IP operation is also supported. This is useful when broadcast/network
discovery is unavailable, for example in some virtual-machine network setups.

A direct-IP `--find` accepts the requested IP as a candidate; `--probe` is the
command that actually opens the HTRA device and validates the connection.

## Installation

```bash
chmod +x install.sh
sudo ./install.sh
```

The installer:

1. installs build dependencies and GNU Radio;
2. installs the bundled HTRA SDK/runtime for the host architecture;
3. builds and installs the `HarogicSupport` SoapySDR module;
4. configures `/usr/bin/CalFile`;
5. refreshes the dynamic linker cache;
6. runs basic SoapySDR checks.

## Verify the SoapySDR driver

```bash
SoapySDRUtil --info
```

Search all HAROGIC interfaces:

```bash
SoapySDRUtil --find="driver=harogic"
```

USB device 0:

```bash
SoapySDRUtil --find="driver=harogic,interface=USB,device=0"
SoapySDRUtil --probe="driver=harogic,interface=USB,device=0"
```

Ethernet device at `192.168.1.100`:

```bash
SoapySDRUtil --find="driver=harogic,interface=ETH,ip=192.168.1.100"
SoapySDRUtil --probe="driver=harogic,interface=ETH,ip=192.168.1.100"
```

A successful probe reports hardware information including:

```text
driver_revision = custom-source-usb-eth-1.0
interface = USB or ETH
```

## GNU Radio usage

Open GNU Radio Companion and use:

```text
Core
  -> Soapy
     -> Source
        -> Soapy Custom Source
```

Set:

```text
Driver = harogic
```

Then set **Device arguments** to either:

```text
interface=USB,device=0
```

or:

```text
interface=ETH,ip=192.168.1.100
```

The example `.grc` files use GNU Radio's standard `soapy_custom_source` block.
Their default Device arguments are `interface=USB,device=0`; change this field
to the Ethernet form when using a network receiver.

## Streaming

The driver exposes receive IQ streaming through standard SoapySDR methods such
as `setupStream()`, `activateStream()`, `readStream()`, `deactivateStream()`, and
`closeStream()`.

Supported Soapy stream formats include:

```text
CF32
CS32
CS16
CS8
```

The HTRA IQ stream itself uses the native HTRA integer formats; CF32 is provided
to the application through the driver's conversion path.

## Development

For device-driver development, the main files are:

```text
soapy-htra/HarogicDevice.hpp
soapy-htra/HarogicDevice.cpp
```

Typical mapping:

```text
SoapySDR find/make       -> HTRA device discovery/open
setFrequency()           -> HTRA IQ profile center frequency
setSampleRate()          -> HTRA IQ profile decimation/sample rate
setGain()/setAntenna()   -> HTRA receive profile settings
activateStream()         -> Device_Open + IQS_Configuration + trigger start
readStream()             -> IQ data delivered to GNU Radio
```

## Uninstallation

The existing `uninstall.sh` remains compatible with this edition.

```bash
chmod +x uninstall.sh
sudo ./uninstall.sh
```

This removes the installed SoapyHarogic module and also cleans up the legacy
HTRA GRC wrapper if it was installed by the previous package. It keeps the HTRA
SDK/runtime by default.

To also purge the HTRA SDK/runtime files:

```bash
sudo ./uninstall.sh --purge-sdk
```

Do not run the script with `sudo sh ./uninstall.sh`; use `sudo ./uninstall.sh`
or `sudo bash ./uninstall.sh` because the script uses Bash features.
