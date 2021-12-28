# TuyaOS Embedded Wi-Fi & Bluetooth Air Purifier

[English](./README.md) | [中文](./README_zh.md)

<br>

## Overview

In this demo, we will show you how to retrofit an ordinary air purifier and make it IoT-enabled. Based on the [Tuya IoT Development Platform](https://iot.tuya.com/), we use Tuya's Wi-Fi and Bluetooth Low Energy combo module, SDK, and the Smart Life app to connect the air purifier to the cloud. This smart air purifier features auto wind speed and filter status notifications.

<br>

## Get started

### Compile and flash

- Create a product on the [Tuya IoT Development Platform](https://iot.tuya.com/) and download the SDK in the third step of **Hardware Development**.

- Clone this demo to the `apps` folder in the downloaded SDK

   ```bash
   $ cd apps
  $ git clone github URL
   ```

- Run the following command in the SDK root directory to start compiling.

   ```bash
   sh build_app.sh apps/tuya_air_cleaner_demo tuya_air_cleaner_demo 1.0.0
   ```

- For more information about flashing and authorization, see [General Wi-Fi SDK Burning and Authorization Instruction](https://developer.tuya.com/en/docs/iot/tuya-common-wifi-sdk-burning-and-authorization?id=K9ip0gbawnkn7).

<br>

### File introduction

```c
tuya_air_cleaner_demo
├── platform             /* Development environment and toolchain */
├── sdk                  /* Header files and libraries */
└──  app                 /* The demo */
    ├── src              /* Source code files */
    │   ├── common
    │   │   ├── tuya_device.c                     /* Entry to application layer */
    │   │   ├── tuya_dp_process.c                 /* Process DP data reporting and receiving  */
    │   │   ├── tuya_iot_funtion.c                /* Connect to the Tuya IoT cloud */
    │   │   └── tuya_key_funtion.c                /* Process keypress events */
    │   ├── driver
    │   │   ├── mfrc522
    │   │   │   ├── tuya_mfrc522_app.c            /* The middle layer driver for MFRC522 NFC chip */
    │   │   │   └── tuya_mfrc522.c                /* The driver for MFRC522 NFC chip */
    │   │   ├── tm1650
    │   │   │   ├── soc_i2c.c                     /* The I2C simulator driver */
    │   │   │   ├── tm1650_app.c                  /* The middle layer driver for TM1650 segment LED display */
    │   │   │   └── tm1650.c                      /* The driver for TM1650 segment LED display */
    │   │   ├── tuya_buz_driver.c                 /* The driver for buzzer */
    │   │   ├── tuya_hardware_driver.c            /* The hardware GPIO driver */
    │   │   ├── tuya_lcd_display.c                /* The application layer driver for LED display */
    │   │   └── tuya_motor_driver.c               /* The driver for fan motor */
    │   └── function
    │       ├── tuya_air_quality_funtion.c        /* Get air quality and feed it to the display */
    │       ├── tuya_automatic_mode_funtion.c     /* Features in auto mode */
    │       ├── tuya_countdown_funtion.c          /* Countdown timer */
    │       ├── tuya_filter_funtion.c             /* Filter life related*/
    │       ├── tuya_mode_funtion.c               /* Mode switching */
    │       ├── tuya_nfc_funtion.c                /* NFC detection for filter*/
    │       └── tuya_timer_funtion.c              /* Timer  */
    └── include
        ├── common
        │   ├── tuya_device.h
        │   ├── tuya_dp_process.h
        │   ├── tuya_iot_funtion.h
        │   └── tuya_key_funtion.h
        ├── driver
        │   ├── mfrc522
        │   │   ├── tuya_mfrc522_app.h
        │   │   └── tuya_mfrc522.h
        │   ├── tm1650
        │   │   ├── soc_i2c.h
        │   │   ├── tm1650_app.h
        │   │   └── tm1650.h
        │   ├── tuya_buz_driver.h
        │   ├── tuya_hardware_driver.h
        │   ├── tuya_lcd_display.h
        │   └── tuya_motor_driver.h
        └── function
            ├── tuya_air_quality_funtion.h
            ├── tuya_automatic_mode_funtion.h
            ├── tuya_countdown_funtion.h
            ├── tuya_filter_funtion.h

```

<br>

### Entry to application

Entry file: `/apps/src/tuya_device.c`

Main function: `device_init()`

+ Call `tuya_iot_wf_soc_dev_init_param()` for SDK initialization to configure working mode and pairing mode, register callbacks, and save the firmware key and PID.
+ Call `tuya_iot_reg_get_wf_nw_stat_cb()` to register callback of device network status.

<br>

### Data point (DP)

- Report DP data: `dev_report_dp_json_async()`

| Function | OPERATE_RET dev_report_dp_json_async(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt) |
| ------- | ------------------------------------------------------------ |
| devid | For gateways and devices built with the MCU or SoC, the `devid` is NULL. For sub-devices, the `devid` is `sub-device_id`. |
| dp_data | The name of DP struct array |
| cnt | The number of elements in the DP struct array |
| Return | `OPRT_OK`: success. Other values: failure. |

<br>

### Pin configuration

| Peripherals | I/O | Peripherals | I/O |
| ------------------ | ---- | ------------- | ---- |
| TXD1 of the air quality sensor | TX | The SCL of the segment LCD | PA2 |
| RXD1 of the air quality sensor | RX | The SDA of the segment LCD | PA3 |
| Key 1 | PA20 | The SCK of the NFC reader | PA7 |
| Key 2 | PA19 | The MOSI of the NFC reader | PA8 |
| Buzzer | PA11 | The MISO of the NFC reader | PA9 |
| UV disinfection light | PA10 | The NSS of the NFC reader | PA15 |
| The clock of the fan motor | PA12 | Photosensitive sensor | PA16 |
| The brake of the fan motor | PA18 | The buckle of the filter compartment cover | PA4 |
| The frequency of the fan motor | PA17 |               |      |

<br>

## Reference

- [Wi-Fi SDK Guide](https://developer.tuya.com/en/docs/iot/tuya-common-wifi-sdk?id=K9glcmvw4u9ml)
- [Wi-Fi SDK Demo](https://developer.tuya.com/en/docs/iot/tuya-wifi-sdk-demo-instructions?id=K9oce5ayw5xem)
- [Tuya Project Hub](https://developer.tuya.com/demo)

<br>


## Technical support

You can get support from Tuya with the following methods:

+ [Tuya Developer Platform](https://developer.tuya.com/en/)
+ [Help Center](https://support.tuya.com/en/help)
+ [Service & Support](https://service.console.tuya.com)

<br>
