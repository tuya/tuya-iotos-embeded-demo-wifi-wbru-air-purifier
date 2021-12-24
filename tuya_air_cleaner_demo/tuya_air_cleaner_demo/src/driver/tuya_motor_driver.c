/**
* @file tuya_motor_driver.c
* @author ali.zhang@tuya.com
* @brief tuya_motor_driver module is used to drive motor
* @version 1.0.0
* @date 2021-09-14
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_motor_driver.h"
#include "uni_log.h"
#include "tuya_gpio.h"
#include "tuya_pin.h"
#include "tuya_pwm.h"
#include "tuya_dp_process.h"
#include "tuya_filter_funtion.h"

/*********************************************************************
******************************micro define****************************
*********************************************************************/

 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/

 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
STATIC tuya_pwm_t *p_pwm_motor = NULL;

STATIC FAN_SPEED_E g_fan_speed = SPEED_SLEEP;

STATIC FLOAT_T g_fan_speed_percent = 0;

STATIC BOOL_T g_fault_to_stop_motor_flag = FALSE;
 
/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_motor_pwm_init
* @desc: motor pwm pin and BRK pin initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_motor_pwm_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* motor BRK pin init, set output */
    op_ret = tuya_gpio_inout_set(MOROR_BRK_PIN, FALSE);
    if (op_ret != OPRT_OK) {
        PR_ERR("motor BRK pin set output error:%d", op_ret);
        return op_ret;
    }
    tuya_gpio_write(MOROR_BRK_PIN, FALSE);

    /* find pwm1 handle */
    p_pwm_motor = tuya_driver_find(TUYA_DRV_PWM, TUYA_PWM1);
    if (NULL == p_pwm_motor) {
        PR_ERR("p_pwm_motor find failed");
    }
    
    /* pwm1 init and config */
    TUYA_PWM_CFG(p_pwm_motor, MOROR_PWM_PIN, 50, 0.5); //frequency:50hz, duty:0.5
    op_ret = tuya_pwm_init(p_pwm_motor);
    if (op_ret != OPRT_OK) {
        PR_ERR("motor pwm pin init set pwm error:%d", op_ret);
        return op_ret;
    }  

    PR_NOTICE("tuya_motor_pwm_init succes");
    return op_ret;
}

/**
* @brief: tuya_motor_pwm_start
* @desc: start motor pwm
* @param[in] frequency:set pwm frequency, 80~430 hz
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_motor_pwm_start(IN CONST FLOAT_T frequency)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* set pwm frequency */
    op_ret = tuya_pwm_frequency_set(p_pwm_motor, frequency);
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_pwm_frequency_set error:%d", op_ret);
        return op_ret;
    }

    /* start pwm */
    op_ret = tuya_pwm_start(p_pwm_motor);
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_pwm_start error:%d", op_ret);
        return op_ret;
    }

    return op_ret;
}

/**
* @brief: _tuya_motor_start
* @desc: start motor to control fan speed
* @param[in] percent:set fan speed percent, 0.2~1
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET _tuya_motor_start(IN CONST FLOAT_T percent)
{
    OPERATE_RET op_ret = OPRT_OK;
    FLOAT_T speed_percent = 0;

    speed_percent = percent;

    //Check the input parameter, percent: 0.2~1
    if (speed_percent < 0.2) {
        speed_percent = 0.2;
    }
    if (speed_percent > 1) {
        speed_percent = 1;
    }

    /* Break pin off */
    tuya_gpio_write(MOROR_BRK_PIN, TRUE);

    /* Start motor pwm */
    op_ret = tuya_motor_pwm_start(430 * speed_percent); //430hz is max frequency
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_motor_pwm_start error:%d", op_ret);
        return op_ret;
    }

    return op_ret;
}

/**
* @brief: tuya_motor_start
* @desc: start motor to control fan speed
* @param[in] percent:set fan speed percent, 0.2~1
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_motor_start(IN CONST FLOAT_T percent)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T filter_life = 0;
    FLOAT_T speed_percent = 0;

    if (TRUE == g_fault_to_stop_motor_flag) {
        return op_ret;
    }

    /* get filter life */
    filter_life = tuya_get_filter_life();

    /* figure fen_speed increase percent */
    if (filter_life >= 80) {
        speed_percent = percent;
    } else if (filter_life >= 50) {
        speed_percent = percent * SPEED_INCREASE_10_PERC;
    } else if (filter_life >= 20) {
        speed_percent = percent * SPEED_INCREASE_20_PERC;
    } else {
        speed_percent = percent * SPEED_INCREASE_30_PERC;
    }

    /* start fan motor */
    op_ret = _tuya_motor_start(speed_percent); 
    if (op_ret != OPRT_OK) {
        PR_ERR("_tuya_motor_start error:%d", op_ret);
        return op_ret;
    }

    /* set g_fan_speed_percent value */
    g_fan_speed_percent = speed_percent;

    PR_NOTICE("tuya_motor_start suc, percent:%f", speed_percent);
    return op_ret;
}

/**
* @brief: tuya_motor_stop
* @desc: stop motor
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_motor_stop(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* Break pin on */
    tuya_gpio_write(MOROR_BRK_PIN, FALSE);

    /* stop motor pwm */
    op_ret = tuya_pwm_stop(p_pwm_motor);
    if (op_ret != OPRT_OK) {
        PR_ERR("stop motor pwm error:%d", op_ret);
        return op_ret;
    }

    /* set g_fan_speed_percent value*/
    g_fan_speed_percent = 0;

    PR_DEBUG("ali, tuya_motor_stop suc");
    return op_ret;
}

/**
* @brief: tuya_get_fan_speed_percent
* @desc: get fan speed percent
* 
* @return fan_speed_percent, 0-100
*/
UINT_T tuya_get_fan_speed_percent(VOID_T)
{
    UINT_T fan_speed_percent = 0;
    fan_speed_percent = g_fan_speed_percent * 100;
    return fan_speed_percent;
}

/**
* @brief: tuya_fan_speed_dp_handle
* @desc: fan_speed DP send down handle
* @param[in] fan_speed:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_fan_speed_dp_handle(IN CONST FAN_SPEED_E fan_speed)
{
    OPERATE_RET op_ret = OPRT_OK;
    FLOAT_T speed_percent = 0;

    switch (fan_speed) {
    case SPEED_SLEEP:
        speed_percent = 0.2;
        break;
    case SPEED_LOW:
        speed_percent = 0.4;
        break;
    case SPEED_MID:
        speed_percent = 0.7;
        break;
    case SPEED_HIGH:
        speed_percent = 1;
        break;
    default:
        break;
    }

    op_ret = tuya_motor_start(speed_percent);
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_motor_start error:%d", op_ret);
        return op_ret;
    }
    g_fan_speed = fan_speed;
    
    return op_ret;
}

/**
* @brief: tuya_fault_to_stop_motor
* @desc: air clean fault, then stop motor
* @param[in] fault: TRUE or FALSE
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_fault_to_stop_motor(IN CONST BOOL_T fault)
{
    OPERATE_RET op_ret = OPRT_OK;

    g_fault_to_stop_motor_flag = fault;

    if (fault == TRUE) {
        tuya_motor_stop();
    }

    return op_ret;
}

/**
* @brief: tuya_manual_mode_motor_start
* @desc: manual mode, start motor
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_manual_mode_motor_start(VOID_T)
{
    tuya_fan_speed_dp_handle(g_fan_speed);
    return OPRT_OK;
}

