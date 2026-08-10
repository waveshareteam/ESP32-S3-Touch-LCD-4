# 硬件参考

[English](HARDWARE_REFERENCE.md)

本文汇总 [`hardware/schematics/ESP32-S3-Touch-LCD-4 V4.0.pdf`](<schematics/ESP32-S3-Touch-LCD-4 V4.0.pdf>) 中的 ESP32-S3-Touch-LCD-4 V4.0 硬件信息。涉及电气连接时，请以 PDF 和实际板卡版本为准。

## 板卡版本

- ESP32-S3-Touch-LCD-4 装配 GT911 电容触摸控制器。
- ESP32-S3-LCD-4 共用显示和外设设计，但不装配 GT911。带触摸示例在 CI 中编译成功，不代表该版本具备指针输入。

## V4.0 已确认模块

| 模块 | 原理图标识 | 说明 |
| --- | --- | --- |
| 主控模组 | ESP32-S3-WROOM-1-N16R8 | 16 MB Flash，8 MB Octal PSRAM |
| 显示 | 4 英寸 480 x 480 RGB LCD | RGB 数据/时序，加串行控制和复位 |
| 触摸 | GT911 接口 | 仅 Touch 版本装配 |
| 辅助控制器 | CH32V003F4U6 | 背光、复位、蜂鸣器、电源控制和电池 ADC 辅助逻辑 |
| RTC | PCF85063ATL | 32.768 kHz 晶振和中断 |
| CAN/TWAI | TJA1051T/3/1J | CANH/CANL 保护和可切换 120 欧姆终端电阻 |
| RS485 | SP3485EN | 差分接口和可切换 120 欧姆终端电阻 |
| 存储 | microSD | 1-bit SDMMC 接口卡槽 |
| 外部接口 | VIN、GND、RS485、CAN、I2C | 接线前请在实体板上确认连接器方向 |

原理图将 DC 输入标为 5 至 36 V。接入外部电源前，仍需确认产品说明和实际板卡版本。

## 共享控制总线

板卡软件与原理图中的主共享 I2C 信号一致：

| 信号 | ESP32-S3 GPIO |
| --- | --- |
| SCL | GPIO7 |
| SDA | GPIO15 |

维护中的示例和托管组件使用 `0x24` 作为 CH32V003 辅助控制器的 I2C 地址。显示复位、触摸复位、背光、蜂鸣器、系统电源和电池测量都依赖该控制器，因此 CH32 或共享总线故障可能表现为多个看似无关的外设同时异常。

## 已确认外设引脚

| 功能 | 信号 |
| --- | --- |
| RTC | PCF85063A 地址 `0x51`，SDA GPIO15/SCL GPIO7；`RTC_INT` 经 CH32V003 EXIO7 |
| TWAI/CAN | TX GPIO6，RX GPIO0 |
| RS485 | TX GPIO44，RX GPIO43 |
| microSD（1-bit SDMMC） | CLK GPIO2，CMD GPIO1，D0 GPIO4 |

## 硬件审查清单

修改板级代码或文档前：

1. 确认准确的板卡型号和硬件版本。
2. 交叉检查原理图、托管 BSP 配置、Arduino 引脚配置和受影响示例。
3. 按改动范围检查显示时序/复位、触摸 INT/RST/I2C、CH32 控制、RTC、SD、RS485、CAN/TWAI、USB、电源以及共享引脚。
4. 将工厂/恢复镜像与 CI 从源码构建的产物分开。
5. 将 CI 视为编译和打包验证；电气行为变化需要单独记录实体板验证。

## 文件

- 原理图：[`hardware/schematics/ESP32-S3-Touch-LCD-4 V4.0.pdf`](<schematics/ESP32-S3-Touch-LCD-4 V4.0.pdf>)
- 工厂/恢复镜像：[`firmware/`](../firmware/)
- 托管组件策略：[`docs/components_CN.md`](../docs/components_CN.md)
