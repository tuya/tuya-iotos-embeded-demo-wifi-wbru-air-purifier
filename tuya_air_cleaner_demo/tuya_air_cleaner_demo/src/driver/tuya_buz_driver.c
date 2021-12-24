/**
* @file tuya_buz_driver.c
* @author ali.zhang@tuya.com
* @brief tuya_buz_driver module is used to driver motor and buz
* @version 1.0.0
* @date 2021-09-13
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_buz_driver.h"
#include "uni_log.h"
#include "tuya_pin.h"
#include "tuya_pwm.h"
#include "sys_timer.h"
#include "tuya_timer_funtion.h"

/*********************************************************************
******************************micro define****************************
*********************************************************************/

 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/
 
 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
static tuya_pwm_t *p_pwm_buz = NULL;

STATIC TIMER_ID timer_buz_keep_100ms;
STATIC TIMER_ID timer_buz_cycle_4times;

/*********************************************************************
**********************private function prototypes*********************
*********************************************************************/
STATIC OPERATE_RET tuya_buz_keep_100ms_timer_start(VOID_T);
STATIC OPERATE_RET tuya_buz_cycle_4times_timer_start(VOID_T);

/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_buz_pwm_init
* @desc: buz pwm pin initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_buz_pwm_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* find pwm0 handle */
    p_pwm_buz = tuya_driver_find(TUYA_DRV_PWM, TUYA_PWM0);
    if (NULL == p_pwm_buz) {
        PR_ERR("p_pwm_buz find failed");
        return OPRT_COM_ERROR;
    }

    /* pwm0 init and config */
    TUYA_PWM_CFG(p_pwm_buz, BUZ_PIN, 4 * 1000, 0.5); //frequency:4000hz, duty:0.5 
    op_ret = tuya_pwm_init(p_pwm_buz);
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_pwm_init error:%d", op_ret);
        return op_ret;
    }  

    PR_NOTICE("tuya_buz_pwm_init succes");
    return op_ret;
}

/**
* @brief: tuya_buz_start
* @desc: start buzzer
* @param[in] sound_type:select buzzer sound type
* @return none
*/
OPERATE_RET tuya_buz_start(IN CONST SOUND_TYPE_E sound_type)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* sound tpye is key */
    if (SOUND_KEY == sound_type) {
        tuya_pwm_start(p_pwm_buz);
        tuya_hal_system_sleep(100);
        tuya_pwm_stop(p_pwm_buz);
    }
    /* sound tpye is alarm */
    if (SOUND_ALARM == sound_type) {
        tuya_buz_cycle_4times_timer_start();
    }

    return op_ret;
}

/**
* @brief: tuya_buz_cycle_5times_timer_cb
* @desc: buz cycle 4times timer callback function
* 
* @return none
*/
STATIC VOID_T tuya_buz_cycle_4times_timer_cb()
{
    STATIC UINT_T s_tick;

    /* The buzzer rings five times */ 
    tuya_buz_start(SOUND_KEY);
    if (s_tick++ >= 3){
        s_tick = 0;
        sys_stop_timer(timer_buz_cycle_4times);
    }
    return;
}

/**
* @brief: tuya_buz_cycle_5times_timer_start
* @desc: start buz cycle 5times timer
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_buz_cycle_4times_timer_start(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = sys_add_timer(tuya_buz_cycle_4times_timer_cb, NULL, &timer_buz_cycle_4times);
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_add_timer buz_cycle_4times error:%d", op_ret);
        return op_ret;
    } 

    op_ret = sys_start_timer(timer_buz_cycle_4times, 500, TIMER_CYCLE); //Start the timer, cycle count 500ms 
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_start_timer buz_cycle_4times error:%d", op_ret);
        return op_ret;
    }  

    return op_ret;
}