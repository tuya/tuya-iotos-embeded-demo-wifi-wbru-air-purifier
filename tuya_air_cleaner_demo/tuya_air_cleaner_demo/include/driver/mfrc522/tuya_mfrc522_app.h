/**
* @file tuya_mfrc522_app.h
* @author ali.zhang@tuya.com
* @brief tuya_mfrc522_app module is used to dirver mfrc522
* @version 1.0.0
* @date 2021-10-28
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
#ifndef __TUYA_MFRC522_APP_H__
#define __TUYA_MFRC522_APP_H__
 
#include "tuya_cloud_types.h"
#include "tuya_mfrc522.h"

#ifdef __cplusplus
extern "C" {
#endif
 
/*********************************************************************
******************************micro define****************************
*********************************************************************/
//If use mfrc522 reset pin, open the define
#define    USE_MFRC522_RST_PIN                    0
 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/
 
 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
 
 
/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_mfrc522_app_init
* @desc: mfrc522 app initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_mfrc522_app_init(VOID_T);

/**
* @brief: PcdRequest
* @desc: Sends an Request Command
* @param[in] req_code: Request mode
* @ref            0x52 = Find all cards in the zone conforming to 14443A standard
* @ref            0x26 = Find the card that has not entered hibernation 
* @param[out] pTagType: The card type
* @ref            0x4400 = Mifare_UltraLight
* @ref            0x0400 = Mifare_One(S50)
* @ref            0x0200 = Mifare_One(S70)
* @ref            0x0800 = Mifare_Pro(X)
* @ref            0x4403 = Mifare_DESFire     
* @return OPERATE_RET
*/
OPERATE_RET PcdRequest(IN UCHAR_T req_code, OUT UCHAR_T *pTagType);

/**
* @brief: PcdAnticoll
* @desc: Card collision protection
* @param[out] pSnr: Card serial number, 4 bytes
* @return OPERATE_RET
*/
OPERATE_RET PcdAnticoll(OUT UCHAR_T *pSnr);

/**
* @brief: PcdSelect
* @desc: send commands to select a single Card
* @param[in] pSnr: Card serial number, 4 bytes
* @return OPERATE_RET
*/
OPERATE_RET PcdSelect(IN UCHAR_T *pSnr);

/**
* @brief: PcdAuthState
* @desc: Authentication to enable a secure communication to card
* @param[in] auth_mode:
* @ref            0x60 = authenticate A key
* @ref            0x61 = authenticate B key
* @param[in] addr: block address
* @param[in] pKey: key
* @param[in] pSnr: card serial number, 4 bytes
* @return OPERATE_RET
*/            
OPERATE_RET PcdAuthState(IN UCHAR_T auth_mode, IN UCHAR_T addr, IN UCHAR_T *pKey, IN UCHAR_T *pSnr);

/**
* @brief: PcdRead
* @desc: Read 16 bytes from the card
* @param[in] addr: block address
* @param[out] pData: read data
* @return OPERATE_RET
*/           
OPERATE_RET PcdRead(IN UCHAR_T addr, OUT UCHAR_T *pData);

/**
* @brief: PcdWrite
* @desc: write 16 bytes data in the card
* @param[in] addr: block address
* @param[in] pData: write data
* @return OPERATE_RET
*/                         
OPERATE_RET PcdWrite(IN UCHAR_T addr, IN UCHAR_T *pData);

/**
* @brief: PcdHalt
* @desc: Command card to into sleep
* @return OPERATE_RET
*/  
OPERATE_RET PcdHalt(VOID_T);

/**
* @brief: CalulateCRC
* @desc: Calculate CRC16 function with MF522
* @param[in] pIndata:
* @param[in] len:
* @param[out] pOutData:
* @return none
*/  
VOID_T CalulateCRC(IN UCHAR_T *pIndata, IN UCHAR_T len, OUT UCHAR_T *pOutData);

/**
* @brief: PcdReset
* @desc: Reset mfrc522
* @return OPERATE_RET
*/  
OPERATE_RET PcdReset(VOID_T);

/**
* @brief: M500PcdConfigISOType
* @desc: Set the working mode
* @param[in] type:
* @return OPERATE_RET
*/  
OPERATE_RET M500PcdConfigISOType(IN UCHAR_T type);

/**
* @brief: SetBitMask
* @desc: Set mfrc522 register bit
* @param[in] reg: register address
* @param[in] mask: set value
* @return none
*/  
VOID_T SetBitMask(IN UCHAR_T reg, IN UCHAR_T mask);

/**
* @brief: ClearBitMask
* @desc: Clear mfrc522 register bit
* @param[in] reg: register address
* @param[in] mask: clear value
* @return none
*/  
VOID_T ClearBitMask(IN UCHAR_T reg, IN UCHAR_T mask);

/**
* @brief: PcdComMF522
* @desc: Through mfrc522, communication with Card
* @param[in] Command: mfrc522 command
* @param[in] pInData: send data
* @param[in] InLenByte: send data len of byte
* @param[out] pOutData: receive data
* @param[out] pOutLenBit: receive data len of bit
* @return OPERATE_RET
*/  
OPERATE_RET PcdComMF522(UCHAR_T Command, UCHAR_T *pInData, UCHAR_T InLenByte, UCHAR_T *pOutData, UINT_T *pOutLenBit);

/**
* @brief: PcdAntennaOn
* @desc: Open the antenna, there should be at least a 1ms interval between each open or close
* @return none
*/  
VOID_T PcdAntennaOn();

/**
* @brief: PcdAntennaOff
* @desc: Close the antenna, there should be at least a 1ms interval between each open or close
* @return none
*/  
VOID_T PcdAntennaOff();

/**
* @brief: WaitCardOff
* @desc: Waiting for the card to leave
* @return none
*/  
VOID_T WaitCardOff(VOID_T);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_MFRC522_APP_H__*/