# HAROGIC SoapySDR Device Arguments

## GNU Radio Soapy Custom Source

Set `Driver` to:

```text
harogic
```

Use one of the following Device arguments strings.

### USB

```text
interface=USB,device=0
```

### Ethernet

```text
interface=ETH,ip=192.168.1.100
```

## Accepted keys

| Purpose | Primary key | Accepted aliases |
|---|---|---|
| Physical interface | `interface` | `physical_interface` |
| USB device number | `device` | `device_number`, `device_num`, `index` |
| Ethernet IPv4 address | `ip` | `ip_address`, `address` |
| Optional serial filter | `serial` | - |

## Examples

```text
driver=harogic,interface=USB,device=0
```

```text
driver=harogic,interface=ETH,ip=192.168.1.100
```

```text
driver=harogic,interface=USB,serial=<serial>
```

```text
driver=harogic,interface=ETH,serial=<serial>
```

When Ethernet is selected by serial only, the driver uses
`Device_GetNetworkDeviceList()` to resolve the device IP address.

When an Ethernet IP is explicitly supplied, direct-IP operation remains
available even if automatic network discovery does not return the device.
Use `SoapySDRUtil --probe=...` to validate that the HTRA API can actually open
the target receiver.
