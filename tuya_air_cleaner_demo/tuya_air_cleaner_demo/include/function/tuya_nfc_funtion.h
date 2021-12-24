/**
* @file tuya_nfc_funtion.h
* @author ali.zhang@tuya.com
* @brief tuya_nfc_funtion module is used to nfc funtion
* @version 1.0.0
* @date 2021-10-28
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
#ifndef __TUYA_NFC_FUNTION_H__
#define __TUYA_NFC_FUNTION_H__
 
#include "tuya_cloud_types.h"
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*********************************************************************
******************************micro define****************************
*********************************************************************/

 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/

 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
//The page address is where the filter data is stored
#define    CARD_READ_PAGE_ADDR                  12    //Page 12
//How many bytes of data are stored
#define    CARD_READ_BYTE_LEN                   8     //8 bytes

#define    CARD_READ_HEAD_TAG                   0x74  //"t", means tuya

#define    BUZ_ALARM_DELAY_TIME                 2000  //2000ms
#define    CHECK_DOOR_CYCLE_TIME                5000  //5000ms

#define    FUALT_FILTER_WRONG                   0x01  //fault DP, bit1
#define    FUALT_DOOR_OPEN                      0x02  //fault DP, bit2



/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_nfc_funtion_init
* @desc: nfc funtion initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_nfc_funtion_init(VOID_T);

/**
* @brief: tuya_check_door_and_filter
* @desc: check door and filter
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_check_door_and_filter(VOID_T);

/**
* @brief: tuya_filter_type_dp_updata
* @desc: updata filter_type dp value
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_filter_type_dp_updata(VOID_T);

/**
* @brief: tuya_door_sen_pin_irq_cb
* @desc: door sen pin callback function
* 
* @return none
*/
VOID_T tuya_door_sen_pin_irq_cb(VOID_T *args);

/**
* @brief: tuya_check_door_and_filter_task_init
* @desc: check_door_and_filter task thread start
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_check_door_and_filter_task_init(VOID_T);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_NFC_FUNTION_H__*/