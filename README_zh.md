# Tuya IoTOS Embeded Demo WiFi & BLE AIR Cleaner 

[English](./README.md) | [中文](./README_zh.md)

<br>

## 简介 

本 demo 基于 [涂鸦IoT平台](https://iot.tuya.com/) 、涂鸦智能APP、IoTOS Embeded WiFi & Ble SDK，使用涂鸦WiFi/WiFi+BLE系列模组快速组建一个空气净化器应用。具有自动风速调节和滤芯寿命计算的亮点功能。

<br>

## 快速上手

### 编译与烧录

- 在 [涂鸦IoT平台](https://iot.tuya.com/) 上创建产品后，在硬件开发的开发资料中下载 SDK。

- 下载Demo至SDK目录的apps目录下：

  ```bash
  $ cd apps
  $ git clone github地址
  ```

- 在SDK根目录下执行以下命令开始编译：

  ```bash
  sh build_app.sh apps/tuya_air_cleaner_demo tuya_air_cleaner_demo 1.0.0
  ```

- 固件烧录授权相关信息请参考：[通用 Wi-Fi SDK 烧录授权说明](https://developer.tuya.com/cn/docs/iot/tuya-common-wifi-sdk-burning-and-authorization?id=K9ip0gbawnkn7) 

<br>

### 文件介绍

```c
tuya_air_cleaner_demo
├── platform             /* 涂鸦通用 Tuya IoTOS SDK 的开发编译环境和工具链 */
├── sdk                  /* 存放涂鸦通用 Tuya IoTOS SDK 的头文件和库文件 */
└──  app                 /* 存放涂鸦通用 Tuya IoTOS SDK 的 demo */
    ├── src              /* 源文件目录 */
    │   ├── common
    │   │   ├── tuya_device.c                     /* 应用层入口 */
    │   │   ├── tuya_dp_process.c                 /* DP上下发处理 */
    │   │   ├── tuya_iot_funtion.c                /* 连接IOT云 */
    │   │   └── tuya_key_funtion.c                /* 按键处理 */
    │   ├── driver
    │   │   ├── mfrc522
    │   │   │   ├── tuya_mfrc522_app.c            /* NFC芯片mfrc522中间层驱动 */
    │   │   │   └── tuya_mfrc522.c                /* NFC芯片mfrc522驱动 */
    │   │   ├── tm1650
    │   │   │   ├── soc_i2c.c                     /* 软件I2C模拟驱动 */
    │   │   │   ├── tm1650_app.c                  /* 液晶显示屏芯片tm1650中间层驱动 */
    │   │   │   └── tm1650.c                      /* 液晶显示屏芯片tm1650驱动 */
    │   │   ├── tuya_buz_driver.c                 /* 蜂鸣器驱动 */
    │   │   ├── tuya_hardware_driver.c            /* 硬件GPIO驱动 */
    │   │   ├── tuya_lcd_display.c                /* 液晶屏应用层驱动 */
    │   │   └── tuya_motor_driver.c               /* 风扇电机驱动 */
    │   └── function
    │       ├── tuya_air_quality_funtion.c        /* 空气质量获取和显示功能 */
    │       ├── tuya_automatic_mode_funtion.c     /* 自动模式相关功能 */
    │       ├── tuya_countdown_funtion.c          /* 倒计时功能 */
    │       ├── tuya_filter_funtion.c             /* 滤芯寿命相关功能 */
    │       ├── tuya_mode_funtion.c               /* 模式切换相关功能 */
    │       ├── tuya_nfc_funtion.c                /* 滤芯NFC检测相关功能 */
    │       └── tuya_timer_funtion.c              /* Timer相关功能 */
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

### 应用入口

入口文件：`/apps/src/tuya_device.c`

重要函数：`device_init()`

+ 调用 `tuya_iot_wf_soc_dev_init_param()` 接口进行SDK初始化，配置了工作模式、配网模式，同时注册了各种回调函数并存入了固件key和PID。
+ 调用 `tuya_iot_reg_get_wf_nw_stat_cb()`接口注册设备网络状态回调函数。

<br>

### DP点相关

- 上报dp点接口：`dev_report_dp_json_async()`

| 函数名  | OPERATE_RET dev_report_dp_json_async(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt) |
| ------- | ------------------------------------------------------------ |
| devid   | 设备id（若为网关、MCU、SOC类设备则devid = NULL；若为子设备，则devid = sub-device_id) |
| dp_data | dp结构体数组名                                               |
| cnt     | dp结构体数组的元素个数                                       |
| Return  | OPRT_OK：成功  Other：失败                                   |

<br>

### I/O 列表

| 外设               | I/O  | 外设          | I/O  |
| ------------------ | ---- | ------------- | ---- |
| 空气质量传感器TXD1 | TX   | 段码液晶屏SCL | PA2  |
| 空气质量传感器RXD1 | RX   | 段码液晶屏SDA | PA3  |
| 按键1              | PA20 | NFC读卡器SCK  | PA7  |
| 按键2              | PA19 | NFC读卡器MOSI | PA8  |
| 蜂鸣器             | PA11 | NFC读卡器MISO | PA9  |
| UV杀菌灯           | PA10 | NFC读卡器NSS  | PA15 |
| 风扇电机CLK        | PA12 | 光敏传感      | PA16 |
| 风扇电机BRK        | PA18 | 仓门开关      | PA4  |
| 风扇电机FG         | PA17 |               |      |

<br>

## 相关文档

- [通用 Wi-Fi SDK 说明](https://developer.tuya.com/cn/docs/iot/tuya-common-wifi-sdk?id=K9glcmvw4u9ml) 
- [通用 Wi-Fi SDK Demo 说明](https://developer.tuya.com/cn/docs/iot/tuya-wifi-sdk-demo-instructions?id=K9oce5ayw5xem) 
- [涂鸦 Demo 中心](https://developer.tuya.com/demo) 

<br>


## 技术支持

您可以通过以下方法获得涂鸦的支持:

- [涂鸦 AI+IoT 开发者平台](https://developer.tuya.com)
- [帮助中心](https://support.tuya.com/help)
- [服务与支持](https://service.console.tuya.com)

<br>

