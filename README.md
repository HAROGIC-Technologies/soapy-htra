# 📡 SoapyHarogic

A SoapySDR module for Harogic HTRA series spectrum analysis devices. 🎯

This module allows software that supports the SoapySDR API (like GQRX, GNU Radio, CubicSDR, rtl_433, etc.) to use Harogic devices as a general-purpose SDR receiver. 🔧

**👨‍💻 Author:** Sébastien Dudek / [penthertz.com](https://penthertz.com)  
**📄 License:** LGPL v2.1

## ✨ Features

- 🔍 Auto-discovery of connected Harogic devices
- 📊 Supports IQ streaming in multiple native formats: **complex 32-bit signed integer (CS32)**, **complex 16-bit signed integer (CS16)**, and **complex 8-bit signed integer (CS8)**.
- ⚙️ Configurable sample rates based on device capabilities
- 📻 Full control over RF frequency
- 🎛️ Comprehensive gain control through standard SoapySDR APIs:
    - 📏 Reference Level (REF)
    - 🔊 Preamplifier (PREAMP)
    - 🤖 IF AGC (IF_AGC)
- 🛠️ Support for device-specific settings:
    - 🎯 Gain Strategy (Low Noise / High Linearity)
    - 🔄 LO Optimization Mode

## 📦 Dependencies

Before building, you must have the following installed on your system:

- **🧩 SoapySDR Development Libraries**: libsoapysdr-dev
- **📚 Harogic HTRA API**: The `libhtra_api.so` library and the `htra_api.h` header file must be installed in a system path (e.g., /usr/local/lib/, /usr/local/include/, or in /opt/htraapi/***)
- **🔨 CMake**: cmake
- **⚡ A C++ Compiler**: g++

On a Debian/Ubuntu-based system, you can install the dependencies with:

```bash
sudo apt-get update
sudo apt-get install build-essential cmake libsoapysdr-dev soapysdr-tools
```

If you have another distribution, you can follow [the steps described here for Soapy installation](https://github.com/pothosware/PothosCore/wiki/BuildGuide).

🔧 **Important**: Calibration data must be placed in `/usr/bin/CalFile` directory in order to work.

## 🚀 Installation

### Quickest way - On Debian/Ubuntu

Get the latest `.deb` package depending on your architecture from this project in [releases](https://github.com/HAROGIC-Technologies/soapy-htra/releases) and install it with `dpkg` as follows (example with AMD64):

```bash
wget https://github.com/HAROGIC-Technologies/soapy-htra/releases/download/v1.0/soapyharogic_2.0_amd64.deb
sudo dpkg -i soapyharogic_1.0_amd64.deb
sudo apt install -f # installing missing dependencies
```

After copying the content of your `CalFile` directory provided by Harogic in `/usr/bin/CalFile` in your filesystem, we can install SoapySDR tools to detect our device, or even play with it now on GNU Radio.

To install SoapySDR tools and GNU Radio:
```bash
sudo apt install soapysdr-tools gnuradio
```

### 1. 🔨 Install the SDK

To install the SDK, you can take the `Linux_Example/Install_HTRA_SDK` path of your USB stick provided by Harogic

Then entering in the `Install_HTRA_SDK` folder you can install everything as follows:

```bash
chmod +x install_htraapi_lib.sh
./install_htraapi_lib.sh
```

If you have installed the Soapy library and the SDK, you are now ready to compile the SoapyHarogic module.

### 2. 🏗️ Build the Module

Clone the repository and build the module using CMake:

```bash
git clone https://github.com/HAROGIC-Technologies/soapy-htra.git
cd soapy-htra
mkdir build
cd build
cmake ..
make && sudo make install
```

This will typically build and install `libsoapyharogic.so` to `/usr/local/lib/SoapySDR/modules0.8/`. 📂

### 3. 🎯 Calibration files

The calibration file `CalFile` directory must be placed in `/usr/bin` or you must make a symlink at `/usr/bin/CalFile` pointing to that directory:

```bash
$ ls /usr/bin/CalFile 
```

### 4. ✅ Verify Installation

Check that SoapySDR can see your device and driver:
```bash
SoapySDRUtil --find="driver=harogic"
```

You should see output similar to this:

```
Found 1 device(s)
  0: driver=Harogic, label=Harogic 313**********, serial=3132***********
```

## ⚠️ Important Usage Notes

### 🎛️ Gain Control and Error -12 (IF Overflow)

The primary gain setting for this device is the **Reference Level (REF)**. This is a critical parameter that tells the device's automatic gain control (AGC) the expected power level of the signal you want to receive. Setting this correctly is essential to avoid errors and get good performance.

The Reference Level works somewhat counter-intuitively:

- **Set a HIGH value (e.g., 7 dBm, -20 dBm)** when you are receiving **STRONG** signals. This tells the AGC to apply less gain (more attenuation) to prevent the internal electronics from being overloaded.
    
- **Set a LOW value (e.g., -30 dBm, -50 dBm)** when you are receiving **WEAK** signals. This tells the AGC to apply more gain to make the signal visible.
    

**What is Error -12?**  
If the device's internal amplifiers or ADC are saturated by a signal that is too strong for the current gain setting, the hardware will report an **IF Overflow** (error code -12).

With the current driver, this is handled as a non-fatal warning. You will see a `D` (for Dropped/Distorted packet) in your console log, and the stream will continue, but the data will be clipped and unusable.

💡 **How to fix IF Overflow (`D` spam):** If you see `D`'s in your log or your signal looks "flat-topped" in the waterfall, it means the gain is too high. **Increase the Reference Level** (e.g., from -70 to -50) until the overflow stops.

A good starting point for the Reference Level is typically between **-10 dBm and -30 dBm**. Adjust it based on your antenna and the strength of the signals in your environment.

### 🔢 Sample Format

The device can stream samples in multiple native integer formats. This driver handles the conversion to complex floats for the host application automatically. For best performance and quality, you can select the native format explicitly.

💡 **The Sweet Spot**: For most use cases, **`CS16` is the recommended format**. It provides an excellent balance of high dynamic range and manageable USB bandwidth, making it the most versatile and stable option.

- **`CS32` (Complex Signed 32-bit Integer)**: 📊 Offers the highest theoretical dynamic range. However, it requires significantly more USB bandwidth and CPU processing, which may not provide a noticeable visual improvement over `CS16` in many scenarios. Use this for specialized, high-precision measurements.
- **`CS16` (Complex Signed 16-bit Integer)**: 🎯 **(Recommended)** The best all-around choice. It provides great dynamic range and is stable across a wide range of sample rates.
- **`CS8` (Complex Signed 8-bit Integer)**: ⚡️ Best for conserving USB bandwidth, especially at very high sample rates. However, it has the lowest dynamic range, which can make it harder to see weak signals next to strong ones.
- **`CF32` (Complex Float 32-bit)**: ⚠️ This format is **not supported** by the hardware in IQ streaming mode. If selected, the driver will log a warning and safely fall back to `CS16`.

By default (`AUTO` mode), the driver will select `CS8` for sample rates **> 60 MS/s** for stability, and `CS16` for rates **<= 60 MS/s**. You can override this behavior using the `native_format` stream argument.

## SoapyHarogic Driver Options

This document outlines all the configurable options for the SoapyHarogic driver, for use in applications like GQRX, SDR++, GNU Radio, or via the command line with tools like `SoapySDRUtil`.

### Device Arguments

These arguments are used to find and select a specific Harogic device. They are entered in the "Device Arguments" field in GRC, GQRX, or as part of the device string.

|Argument Key|Example Value|Description|
|---|---|---|
|`driver`|`harogic`|**(Required)** This tells SoapySDR to load the Harogic driver.|
|`serial`|`313251180036001A`|**(Optional)** The unique serial number of the device. Use this if you have multiple Harogic devices connected and need to select a specific one. If omitted, the first device found will be used.|
|`label`|`Harogic 3132...`|**(Read-Only)** A human-readable name for the device, automatically generated by the driver. You can use this to identify the device but cannot set it as an argument.|

**Example Device String:** `driver=harogic,serial=313251180036001A`

---

### Stream Arguments

These arguments configure the data stream itself and are typically set once before the stream is activated. They should be placed in the **"Stream Arguments"** field in GRC or SDR++.

|Argument Key|Example Value|Default|Description|
|---|---|---|---|
|`native_format`|`CS16`|`AUTO`|Selects the data format requested from the hardware. **`AUTO`** selects CS8/CS16 based on the sample rate (> 60 MS/s = CS8). **`CS32`**, **`CS16`**, or **`CS8`** forces that specific format. `CF32` is not supported and will fall back to `CS16`.|

**Example Stream Arguments String:** `native_format=CS16`

---

### Channel Settings (RF and Gain)

These settings control the RF front-end and can usually be adjusted while the stream is active. They appear as sliders, dropdowns, or text boxes in SDR applications.

#### Antennas

The active antenna port can be selected from a dropdown menu.

- **Available Options:** `External`, `Internal`, `ANT`, `T/R`, `SWR`, `INT`
    
#### Gain Elements

The Harogic device uses a multi-stage gain system controlled by three separate elements.

|Gain Element|Type|Range|Description|
|---|---|---|---|
|`REF`|Integer|-100 to 7 [dBm]|**Reference Level.** This is the primary gain control. It sets the target power level for the top of the ADC's range. **Use a high value for strong signals to prevent overflow.**|
|`PREAMP`|Boolean|On/Off|Toggles the front-end low-noise preamplifier. `On` (1.0) enables it for better sensitivity on weak signals. `Off` (0.0) disables it.|
|`IF_AGC`|Boolean|On/Off|Toggles the Intermediate Frequency (IF) Automatic Gain Control.|

#### Other RF Settings

|Setting Key|Type|Options / Range|Description|
|---|---|---|---|
|`gain_strategy`|String|`Low Noise`, `High Linearity`|Optimizes the internal gain distribution. `Low Noise` is best for weak signals. `High Linearity` is better for environments with strong signals to prevent intermodulation.|
|`lo_mode`|String|`Auto`, `Speed`, `Spurs`, `Phase Noise`|Controls the Local Oscillator (LO) optimization strategy. `Auto` is recommended for general use. Other modes trade between tuning speed, spurious signal rejection, and phase noise performance.|

---

## 🎮 Usage

### 📊 GQRX

GQRX provides an intuitive interface for controlling the device. 🖥️

1. 🚀 Open GQRX.
2. ⚙️ Click the "Configure I/O devices" button (or go to `File -> I/O devices`).
3. 📋 In the "Device" dropdown, select your Harogic device. It should appear with its label.
4. ✍️ In the **"Device String"** field, you can add your stream argument. For example, to force 16-bit samples, your device string might look like this: `driver=harogic,serial=3132...,native_format=CS16`.
5. 🎛️ Go to the **"Input Controls"** tab on the right-hand side to adjust gains.

### 🔬 GNU Radio Companion

Use the **Soapy Source** block for full control. 🎛️

1. ➕ Add a "Soapy Source" block to your flowgraph.
2. 🖱️ Double-click the block to open its properties.
3. 📋 In the **General** tab:
    - **Device Arguments**: Set this to `driver=harogic`.
    - **Stream Arguments**: Set this to `native_format=CS16` (or `CS32`, `CS8`).
4. 📻 In the **RF Options** tab:
    - **Gain (dB)**: This controls the main **REF** (Reference Level) gain.
    - **Antenna**: Select your desired antenna (e.g., `ANT`).
    - **Settings**: This field is used to control other named settings as a comma-separated list of key=value pairs:
        - 🔄 To set LO Mode to "Spurs": `lo_mode=Spurs`
        - 🎯 To set Gain Strategy: `gain_strategy="High Linearity"`

---

## ⚡ Performance Tuning and Testing

High-speed SDR streaming (over 60 MS/s) is demanding on the host computer's USB subsystem and CPU. If you experience stream interruptions (often shown as `T` for timeout, `O` for overflow, or fatal errors in the console), you may need to tune your system for performance. The timeouts (`T`) are a classic symptom of this.

### Performance Tuning Script

This repository includes a Bash script, `performance_mode.sh`, to help apply common system optimizations. **Running this script is highly recommended if you see `T`'s in your log.**

**Usage:**

```bash
# Run the script with sudo
sudo ./performance_mode.sh
```

The script provides an interactive menu to:

1. **Set CPU Governor to 'performance'**: Prevents CPU from down-clocking, reducing latency.
2. **Disable USB Autosuspend**: Stops the kernel from trying to power-save the active SDR.
3. **Increase USB-FS Memory Buffer**: Allocates more RAM for USB transfers, critical for high data rates.
4. **Set USB 3.0 IRQ Affinity**: Pins the USB controller's interrupts to a specific CPU core to reduce jitter.
5. **Disable DRM KMS Polling**: Reduces display driver-related system load.
    
You can apply all tweaks at once or individually. A "Revert to Defaults" option is also available. It is recommended to apply these tweaks before running high-bandwidth applications.

🎉 **Happy SDR-ing!** If you encounter any issues, please open an issue on GitHub. 🐛➡️🔧


