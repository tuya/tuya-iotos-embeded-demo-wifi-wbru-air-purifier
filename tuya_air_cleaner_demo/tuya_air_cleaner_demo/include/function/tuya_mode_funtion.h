/**
* @file tuya_mode_funtion.h
* @author ali.zhang@tuya.com
* @brief tuya_mode_funtion module is used to mode funtion
* @version 1.0.0
* @date 2021-09-29
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
#ifndef __TUYA_MODE_FUNTION_H__
#define __TUYA_MODE_FUNTION_H__
 
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
//air cleaner power turn type
typedef BYTE_T  POWER_TURN_E;
#define POWER_OFF      0  //power off
#define POWER_ON       1  //power on

//air cleaner working mode type
typedef BYTE_T  WORK_MODE_E;
#define MODE_MANUAL    0  //manual mode
#define MODE_AUTO      1  //automatic mode
#define MODE_SLEEP     2  //sleep mode

 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
 
 
/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_power_set
* @desc: set air cleaner power on or off
* @param[in] power:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_power_set(IN CONST POWER_TURN_E power);

/**
* @brief: tuya_work_mode_set
* @desc: set air cleaner work mode
* @param[in] work_mode:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_work_mode_set(IN CONST WORK_MODE_E work_mode);

/**
* @brief: tuya_power_switch
* @desc: switch air cleaner power on off
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_power_switch(VOID_T);

/**
* @brief: tuya_work_mode_switch
* @desc: switch air cleaner work mode
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_work_mode_switch(VOID_T);

/**
* @brief: tuya_air_cleaner_init
* @desc: air cleaner equipment init
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_air_cleaner_init(VOID_T);

/**
* @brief: tuya_recovery_from_fault
* @desc: tuya_recovery_from_fault
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_recovery_from_fault(VOID_T);

/**
* @brief: tuya_get_power_status
* @desc: get power status
* 
* @return power_status
*/
POWER_TURN_E tuya_get_power_status(VOID_T);

 
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_MODE_FUNTION_H__*/