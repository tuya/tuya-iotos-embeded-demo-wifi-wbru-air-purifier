/**
* @file tuya_buz_driver.h
* @author ali.zhang@tuya.com
* @brief tuya_buz_driver module is used to driver motor and buz
* @version 1.0.0
* @date 2021-09-13
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
#ifndef __TUYA_BUZ_DRIVER_H__
#define __TUYA_BUZ_DRIVER_H__
 
#include "tuya_cloud_types.h"
#include "tuya_gpio.h"
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*********************************************************************
******************************micro define****************************
*********************************************************************/
#define    BUZ_PIN                             TY_GPIOA_11
 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/
//buzzer sound type
typedef enum {
    SOUND_KEY = 0,
    SOUND_ALARM,
}SOUND_TYPE_E;
 
/*********************************************************************
****************************variable define***************************
*********************************************************************/


/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_buz_pwm_init
* @desc: buz pwm pin initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_buz_pwm_init(VOID_T);

/**
* @brief: tuya_buz_start
* @desc: start buzzer
* @param[in] sound_type:select buzzer sound type
* @return none
*/
OPERATE_RET tuya_buz_start(IN CONST SOUND_TYPE_E sound_type);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_BUZ_DRIVER_H__*/