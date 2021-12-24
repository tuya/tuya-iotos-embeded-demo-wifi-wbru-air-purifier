/**
* @file tuya_key_funtion.h
* @author ali.zhang@tuya.com
* @brief tuya_key_funtion module is used to driver key
* @version 1.0.0
* @date 2021-09-08
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
#ifndef __TUYA_KEY_FUNTION_H__
#define __TUYA_KEY_FUNTION_H__

#include "tuya_cloud_types.h"
#include "tuya_pin.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
#define    TY_KEY1_PIN                       TY_GPIOA_20
#define    TY_KEY1_LONG_TIME_MS              (3000)  
#define    TY_KEY2_PIN                       TY_GPIOA_19
#define    TY_KEY2_LONG_TIME_MS              (3000) 
 
/***********************************************************
***********************typedef define***********************
***********************************************************/
 
 
/***********************************************************
***********************variable define**********************
***********************************************************/
 
 
/***********************************************************
***********************function define**********************
***********************************************************/
/** 
 * @brief: tuya_key1_funtion_init
 * @desc: tuya Infrared function initialization
 * @param[in] tuya_pin_name_t: The IO port of the module
 * @param[in] long_key_time_ms: Long press to take effect in milliseconds
 * @return OPERATE_RET  OPRT_OK is sucess, other is fail
 * @note none
 */
OPERATE_RET tuya_key1_funtion_init(IN tuya_pin_name_t key_pin,IN USHORT_T long_key_time_ms);

/** 
 * @brief: tuya_key2_funtion_init
 * @desc: tuya Infrared function initialization
 * @param[in] tuya_pin_name_t: The IO port of the module
 * @param[in] long_key_time_ms: Long press to take effect in milliseconds
 * @return OPERATE_RET  OPRT_OK is sucess, other is fail
 * @note none
 */
OPERATE_RET tuya_key2_funtion_init(IN tuya_pin_name_t key_pin,IN USHORT_T long_key_time_ms);

/** 
 * @brief: tuya_key_funtion_init
 * @desc: tuya Infrared function initialization
 * @return OPERATE_RET  OPRT_OK is sucess, other is fail
 * @note none
 */
OPERATE_RET tuya_key_funtion_init(VOID_T);

/**
* @brief: tuya_set_child_lock_value
* @desc: set child_lock value
* @param[in] child_lock
* @return OPERATE_RET
*/
OPERATE_RET tuya_set_child_lock_value(IN CONST BOOL_T child_lock);

/**
* @brief: tuya_set_sound_switch_value
* @desc: set key_sound_close value
* @param[in] key_sound_close
* @return OPERATE_RET
*/
OPERATE_RET tuya_set_key_sound_value(IN CONST BOOL_T key_sound_close);

/**
* @brief: tuya_set_key_power_flag
* @desc: set key_power flag
* @param[in] key_power
* @return OPERATE_RET
*/
OPERATE_RET tuya_set_key_power_flag(IN CONST BOOL_T key_power);

 
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_KEY_FUNTION_H__*/

