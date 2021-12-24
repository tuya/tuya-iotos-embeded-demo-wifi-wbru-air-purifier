/**
* @file tuya_mfrc522.h
* @author ali.zhang@tuya.com
* @brief tuya_mfrc522 module is used to dirver mfrc522
* @version 1.0.0
* @date 2021-10-28
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
#ifndef __TUYA_MFRC522_H__
#define __TUYA_MFRC522_H__
 
#include "tuya_cloud_types.h"
#include "tuya_gpio.h"
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*********************************************************************
******************************micro define****************************
*********************************************************************/
#define    RF_SCK                         TY_GPIOA_7
#define    RF_MOSI                        TY_GPIOA_8
#define    RF_MISO                        TY_GPIOA_9

#define    RF_NSS                         TY_GPIOA_15  //L_RX

/*********************************************************************
****************************typedef define****************************
*********************************************************************/
 
 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
/* MF522命令字 */
#define PCD_IDLE              0x00               //取消当前命令
#define PCD_AUTHENT           0x0E               //验证密钥
#define PCD_RECEIVE           0x08               //接收数据
#define PCD_TRANSMIT          0x04               //发送数据
#define PCD_TRANSCEIVE        0x0C               //发送并接收数据
#define PCD_RESETPHASE        0x0F               //复位
#define PCD_CALCCRC           0x03               //CRC计算

/* Mifare_One卡片命令字 */
#define PICC_REQIDL           0x26               //寻天线区内未进入休眠状态
#define PICC_REQALL           0x52               //寻天线区内全部卡
#define PICC_ANTICOLL1        0x93               //防冲撞
#define PICC_ANTICOLL2        0x95               //防冲撞
#define PICC_AUTHENT1A        0x60               //验证A密钥
#define PICC_AUTHENT1B        0x61               //验证B密钥
#define PICC_READ             0x30               //读块
#define PICC_WRITE            0xA0               //写块
#define PICC_DECREMENT        0xC0               //扣款
#define PICC_INCREMENT        0xC1               //充值
#define PICC_RESTORE          0xC2               //调块数据到缓冲区
#define PICC_TRANSFER         0xB0               //保存缓冲区中数据
#define PICC_HALT             0x50               //休眠

#define PICC_UL_WRITE	      0xA2		         //Writes page to the Mifare_UltraLight Card

/* MF522 FIFO长度定义 */
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte

/* MF522寄存器定义 */
// PAGE 0
#define     RFU00                 0x00    
#define     CommandReg            0x01    
#define     ComIEnReg             0x02    
#define     DivlEnReg             0x03    
#define     ComIrqReg             0x04    
#define     DivIrqReg             0x05
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08    
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     RFU0F                 0x0F
// PAGE 1     
#define     RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
// PAGE 2    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
// PAGE 3      
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     RFU3C                 0x3C   
#define     RFU3D                 0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F		          0x3F

#define     REQ_ALL               0x52
#define     KEYA                  0x60

/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_mfrc522_pin_init
* @desc: mfrc522 pin initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_mfrc522_pin_init(VOID_T);

/**
* @brief: ReadRawRC
* @desc: read mfrc522 register
* @param[in] reg:register address
* @return read value
*/
UCHAR_T ReadRawRC(UCHAR_T reg);

/**
* @brief: WriteRawRC
* @desc: write mfrc522 register
* @param[in] reg:register address
* @param[in] value:write value
* @return none
*/
VOID_T WriteRawRC(UCHAR_T reg, UCHAR_T value);


 
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_MFRC522_H__*/