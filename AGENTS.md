# Project Context

ESP8266-based Sonoff Mini smart switch with HomeKit support, UDP logging.

## Critical Constraints

- **Always propose changes and ask permission before applying.** Do not modify files, restart containers, or change config without explicit approval.
- Explain exactly which file will be changed and what the change does before making it.

## Device Info

- Manufacturer: itead, Model: Sonoff Mini
- FW_VERSION: `1.12` in `src/main.c`
- Relay: GPIO 12, Button: GPIO 0, S2 button: GPIO 4, Status LED: GPIO 13 (off=1)
- Flash: 1MB, dout, 40MHz
- Flash layout: rboot at `0x0`, blank at `0x1000`, app at `0x2000`

## Build & Flash

```bash
# Build inside Docker
./build.sh firmware esp-homekit-sonoff-mini

# Sign firmware
openssl sha384 -binary -out src/firmware/main.bin.sig src/firmware/main.bin
printf "%08x" $(wc -c < src/firmware/main.bin) | xxd -r -p >> src/firmware/main.bin.sig

# Flash app only
esptool.py -p /dev/cu.usbserial-XXXX -b 115200 write_flash -fs 1MB -fm dout -ff 40m 0x2000 src/firmware/main.bin
```

Docker image: `espbuilder`, SDK_PATH: `/project/esp-open-rtos`. Run from `/Users/dave/Documents/OpenCode/esp-firmwares`.

## Key Findings

### Versioning
- FW_VERSION in `src/main.c` follows semver.
- Release naming: tag `x.y.z`, title `vx.y.z-pre`, prerelease=true.
- `latest-pre-release` file (version string only, no newline) uploaded to latest stable release asset for OTA discovery.
- **IMPORTANT: Every firmware change requiring re-flash MUST bump the version (FW_VERSION + latest-pre-release) and create a new pre-release tag. OTA compares versions - same version = no update.**

## Relevant Files

- `src/main.c`: firmware entry, relay logic, FW_VERSION
- `src/Makefile`: build config, component paths, signature target
- `../../components/`: shared components (UDPlogger, esp-homekit, etc.)
