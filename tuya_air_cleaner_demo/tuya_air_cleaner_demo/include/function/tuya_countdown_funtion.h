/**
* @file tuya_countdown_funtion.h
* @author ali.zhang@tuya.com
* @brief tuya_countdown_funtion module is used to countdown funtion
* @version 1.0.0
* @date 2021-10-13
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
#ifndef __TUYA_COUNTDOWN_FUNTION_H__
#define __TUYA_COUNTDOWN_FUNTION_H__
 
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
//countdown set type
typedef BYTE_T  COUNTDOWN_SET_E;
#define COUNTDOWN_SET_CANCEL     0  // cancel 
#define COUNTDOWN_SET_1H         1  // 1 hour
#define COUNTDOWN_SET_2H         2  // 2 hours
#define COUNTDOWN_SET_3H         3  // 2 hours
#define COUNTDOWN_SET_4H         4  // 4 hours
#define COUNTDOWN_SET_5H         5  // 5 hours
 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
 
 
/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_countdown_timer_start
* @desc: start countdown timer
* @param[in] countdown_set
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_countdown_timer_start(IN CONST COUNTDOWN_SET_E countdown_set);

 
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_COUNTDOWN_FUNTION_H__*/