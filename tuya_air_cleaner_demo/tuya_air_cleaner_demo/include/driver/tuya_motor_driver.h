/**
* @file tuya_motor_driver.h
* @author ali.zhang@tuya.com
* @brief tuya_motor_driver module is used to drive motor
* @version 1.0.0
* @date 2021-09-14
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
#ifndef __TUYA_MOTOR_DRIVER_H__
#define __TUYA_MOTOR_DRIVER_H__
 
#include "tuya_cloud_types.h"
#include "tuya_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif
 
/*********************************************************************
******************************micro define****************************
*********************************************************************/
#define    MOROR_PWM_PIN                           TY_GPIOA_12
#define    MOROR_BRK_PIN                           TY_GPIOA_18
#define    MOROR_FG_PIN                            TY_GPIOA_17
 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/
/* DP fen_speed type */
typedef BYTE_T FAN_SPEED_E;
#define SPEED_SLEEP    0  //fan speed: 20%
#define SPEED_LOW      1  //fan speed: 40%
#define SPEED_MID      2  //fan speed: 70%
#define SPEED_HIGH     3  //fan speed: 100%

/* fen_speed increase percent */
#define SPEED_INCREASE_10_PERC      (1.1)  //When filter life 50~80%, fan speed increase 10%,
#define SPEED_INCREASE_20_PERC      (1.2)  //When filter life 20~50%, fan speed increase 20%,
#define SPEED_INCREASE_30_PERC      (1.3)  //When filter life 0~20%, fan speed increase 30%,

/*********************************************************************
****************************variable define***************************
*********************************************************************/
 
 
/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_motor_pwm_init
* @desc: motor pwm pin initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_motor_pwm_init(VOID_T);

/**
* @brief: tuya_motor_start
* @desc: start motor to control fan speed
* @param[in] percent:set fan speed percent, 0.2 ~ 1
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_motor_start(IN CONST FLOAT_T percent);

/**
* @brief: tuya_motor_stop
* @desc: stop motor
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_motor_stop(VOID_T);

/**
* @brief: tuya_get_fan_speed_percent
* @desc: get fan speed percent
* 
* @return fan_speed_percent
*/
UINT_T tuya_get_fan_speed_percent(VOID_T);

/**
* @brief: tuya_fan_speed_dp_handle
* @desc: fan_speed DP send down handle
* @param[in] fan_speed:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_fan_speed_dp_handle(IN CONST FAN_SPEED_E fan_speed);

/**
* @brief: tuya_fault_to_stop_motor
* @desc: air clean fault, then stop motor
* @param[in] fault: TRUE or FALSE
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_fault_to_stop_motor(IN CONST BOOL_T fault);

/**
* @brief: tuya_manual_mode_motor_start
* @desc: manual mode, start motor
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_manual_mode_motor_start(VOID_T);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_MOTOR_DRIVER_H__*/