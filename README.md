# Tuya IoTOS Embeded Demo WiFi & BLE AIR Cleaner  

[English](./README.md) | [中文](./README_zh.md) 

<br>

## Introduction 

This demo is based on [Tuya IoT Platform](https://iot.tuya.com/), Tuya Smart APP, IoTOS Embeded WiFi&Ble SDK, using Tuya WiFi/WiFi+Ble series modules quickly build a air cleaner application. With automatic wind speed adjustment and filter life calculation highlights.

<br>


## Quick start 

### Compile & Burn

- After creating a product on [Tuya IoT Platform](https://iot.tuya.com/), download the SDK from the development materials of hardware development.

- Download the demo to the apps directory of the SDK:

  ```bash
  $ cd apps
  $ git clone address
  ```

- Execute the following command in the SDK root directory to start compiling:

  ```bash
  sh build_app.sh apps/tuya_air_cleaner_demo tuya_air_cleaner_demo 1.0.0
  ```

- Firmware burn-in license information please refer to: [General Wi-Fi SDK programming authorization instructions](https://developer.tuya.com/cn/docs/iot/tuya-common-wifi-sdk-burning-and-authorization?id=K9ip0gbawnkn7)  

<br>

### File description 

```C
tuya_air_cleaner_demo
├── platform             
├── sdk              /* Store the header and library files of the Tuya IoTOS SDK */
└──  app             /* Store the demo of Tuya IoTOS SDK */
    ├── src          /* Source file directory */
    │   ├── common
    │   │   ├── tuya_device.c                   /* Application layer entrance */
    │   │   ├── tuya_dp_process.c               /* Process delivery on DP */
    │   │   ├── tuya_iot_funtion.c              /* Connect the IOT cloud */
    │   │   └── tuya_key_funtion.c              /* Key function process */
    │   ├── driver
    │   │   ├── mfrc522
    │   │   │   ├── tuya_mfrc522_app.c          /* MFRC522 middle layer driver */
    │   │   │   └── tuya_mfrc522.c              /* MFRC522 driver */
    │   │   ├── tm1650
    │   │   │   ├── soc_i2c.c                   /* Software I2C simulation driver */
    │   │   │   ├── tm1650_app.c                /* TM1650 middle layer driver */
    │   │   │   └── tm1650.c                    /* TM1650 driver */
    │   │   ├── tuya_buz_driver.c               /* Buzzer driver */
    │   │   ├── tuya_hardware_driver.c          /* Hardware gpio dirver */
    │   │   ├── tuya_lcd_display.c              /* LCD application layer dirver*/
    │   │   └── tuya_motor_driver.c             /* Fan motor drive */
    │   └── function
    │       ├── tuya_air_quality_funtion.c      /* Get air quality index function */
    │       ├── tuya_automatic_mode_funtion.c   /* Auto mode related function */
    │       ├── tuya_countdown_funtion.c        /* Countdown function */
    │       ├── tuya_filter_funtion.c           /* Filter life related functions */
    │       ├── tuya_mode_funtion.c             /* Switch mode function */
    │       ├── tuya_nfc_funtion.c              /* Fliter NFC detection function */
    │       └── tuya_timer_funtion.c            /* Timer related functions */
    └── include
        ├── common
        │   ├── tuya_device.h
        │   ├── tuya_dp_process.h
        │   ├── tuya_iot_funtion.h
        │   └── tuya_key_funtion.h
        ├── driver
        │   ├── mfrc522
        │   │   ├── tuya_mfrc522_app.h
        │   │   └── tuya_mfrc522.h
        │   ├── tm1650
        │   │   ├── soc_i2c.h
        │   │   ├── tm1650_app.h
        │   │   └── tm1650.h
        │   ├── tuya_buz_driver.h
        │   ├── tuya_hardware_driver.h
        │   ├── tuya_lcd_display.h
        │   └── tuya_motor_driver.h
        └── function
            ├── tuya_air_quality_funtion.h
            ├── tuya_automatic_mode_funtion.h
            ├── tuya_countdown_funtion.h
            ├── tuya_filter_funtion.h
```

<br>

### Entry to application

Entry file: /apps/src/tuya_device.c

Important function：`device_init()`

- Call the `tuya_iot_wf_soc_dev_init_param()` interface to initialize the SDK, configure the working mode, network distribution mode, and register various callback functions and save the firmware key and PID.
- Call the `tuya_iot_reg_get_wf_nw_stat_cb()` interface to register the device network status callback function.

<br>

### Data point (DP)

- Report dp point interface: dev_report_dp_json_async()

| Function name | OPERATE_RET dev_report_dp_json_async(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt) |
| ------------- | ------------------------------------------------------------ |
| devid         | device id (if gateway, MCU, SOC class device then devid = NULL; if sub-device, then devid = sub-device_id) |
| dp_data       | dp structure array name                                      |
| cnt           | Number of elements of the dp structure array                 |
| Return        | OPRT_OK: Success Other: Failure                              |

<br>

### I/O list

| Peripherals             | I/O  | Peripherals            | I/O  |
| ----------------------- | ---- | ---------------------- | ---- |
| Air quality sensor TXD1 | TX   | LCD  SCL               | PA2  |
| Air quality sensor RXD1 | RX   | LCD  SDA               | PA3  |
| Key1                    | PA20 | NFC reader SCK         | PA7  |
| Key2                    | PA19 | NFC reader MOSI        | PA8  |
| Buzzer                  | PA11 | NFC reader MISO        | PA9  |
| UV                      | PA10 | NFC reader NSS         | PA15 |
| Fan motor CLK           | PA12 | Photosensitive sensor  | PA16 |
| Fan motor BRK           | PA18 | Storehouse door switch | PA4  |
| Fan motor FG            | PA17 |                        |      |

<br>

## Reference

- [General Wi-Fi SDK Instruction](https://developer.tuya.com/en/docs/iot/tuya-common-wifi-sdk?id=K9glcmvw4u9ml) 
- [General Wi-Fi SDK Demo Instruction](https://developer.tuya.com/en/docs/iot/tuya-wifi-sdk-demo-instructions?id=K9oce5ayw5xem) 
- [Tuya Project Hub](https://developer.tuya.com/demo)

<br>

## Technical Support

You can get support from Tuya with the following methods:

- [Tuya IoT Developer Platform](https://developer.tuya.com/en/)
- [Help Center](https://support.tuya.com/en/help)
- [Service & Support](https://service.console.tuya.com)

<br>

