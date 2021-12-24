/**
* @file tuya_mode_funtion.c
* @author ali.zhang@tuya.com
* @brief tuya_mode_funtion module is used to mode funtion
* @version 1.0.0
* @date 2021-09-29
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_mode_funtion.h"
#include "uni_log.h"
#include "tuya_dp_process.h"
#include "tuya_motor_driver.h"
#include "tuya_hardware_driver.h"
#include "tuya_air_quality_funtion.h"
#include "tuya_key_funtion.h"
#include "tuya_timer_funtion.h"
#include "tuya_automatic_mode_funtion.h"
#include "tuya_filter_funtion.h"
#include "tuya_lcd_display.h"
 
/*********************************************************************
******************************micro define****************************
*********************************************************************/

 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/
 
 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
STATIC POWER_TURN_E  g_power     = POWER_OFF;
STATIC WORK_MODE_E   g_work_mode = MODE_MANUAL;
STATIC FAN_SPEED_E   g_fan_speed = SPEED_SLEEP;

/*********************************************************************
**********************private function prototypes*********************
*********************************************************************/
STATIC OPERATE_RET tuya_work_mode_set_manual(VOID_T);

/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_power_turn_on
* @desc: set air cleaner power turn on
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_power_turn_on(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    //check door and filter
    tuya_check_door_and_filter();

    g_power = POWER_ON;
    //open key function
    tuya_set_key_power_flag(TRUE);
    //set work mode
    tuya_work_mode_set(g_work_mode);
    //dp uptate
    tuya_update_single_dp(MODE_DPID, PROP_ENUM, g_work_mode);  //working mode

    return op_ret;
}

/**
* @brief: tuya_power_turn_off
* @desc: set air cleaner power turn off
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_power_turn_off(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    //stop fan motor
    tuya_motor_stop();
    //close light
    tuya_light_switch_set(FALSE);
    //close key function
    tuya_set_key_power_flag(FALSE);
    //close uv
    tuya_uv_set_on(FALSE);
    //dp update
    tuya_update_single_dp(FAULT_DPID, PROP_BITMAP, 0x00);
    //write pm_total and runtime_total in flash
    tuya_write_save_data_in_flash();
 
    return op_ret;
}

/**
* @brief: tuya_work_mode_set_manual
* @desc: set air cleaner work mode manual
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_work_mode_set_manual(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    PR_INFO("work mode into manual");
    
    //open light
    tuya_light_switch_set(TRUE);
    //start motor
    tuya_manual_mode_motor_start();

    return op_ret;
}

/**
* @brief: tuya_work_mode_set_auto
* @desc: set air cleaner work mode auto
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_work_mode_set_auto(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    PR_INFO("work mode into automatic");
    
    //open light
    tuya_light_switch_set(TRUE);
    //start auto fan speed
    tuya_auto_fan_speed_start(TRUE);
    //start dark scan
    tuya_dark_scan_start(TRUE);

    return op_ret;
}

/**
* @brief: tuya_work_mode_set_sleep
* @desc: set air cleaner work mode sleep
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_work_mode_set_sleep(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    
    PR_INFO("work mode into sleep");
    
    //close light
    tuya_light_switch_set(FALSE);
    //start motor
    tuya_motor_start(0.2);

    return op_ret;
}

/**
* @brief: tuya_power_set
* @desc: set air cleaner power on or off
* @param[in] power:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_power_set(IN CONST POWER_TURN_E power)
{
    OPERATE_RET op_ret = OPRT_OK;

    if ((MODE_AUTO == g_work_mode) && (POWER_OFF == power)){
        tuya_auto_fan_speed_start(FALSE);  //close pm2.5 scan
        tuya_dark_scan_start(FALSE);       //close dark  scan
    }

    if (POWER_ON == power) {
        op_ret = tuya_power_turn_on();
        if (OPRT_OK != op_ret) {
            PR_ERR("tuya_power_turn_on error:%d", op_ret);
            tuya_update_single_dp(SWITCH_DPID, PROP_BOOL, FALSE);
            return op_ret;
        }
    } else if (POWER_OFF == power) {
        op_ret = tuya_power_turn_off();
        if (OPRT_OK != op_ret) {
            PR_ERR("tuya_power_turn_off error:%d", op_ret); 
            return op_ret;
        }
    }

    g_power = power;
    return op_ret;       
}

/**
* @brief: tuya_work_mode_set
* @desc: set air cleaner work mode
* @param[in] work_mode:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_work_mode_set(IN CONST WORK_MODE_E work_mode)
{
    OPERATE_RET op_ret = OPRT_OK;

    if ((MODE_AUTO == g_work_mode) && (MODE_AUTO != work_mode)){
        tuya_auto_fan_speed_start(FALSE);  //close auto fan speed
        tuya_dark_scan_start(FALSE);       //close dark scan
    }

    g_work_mode = work_mode;

    switch (g_work_mode) {
    case MODE_MANUAL:
        op_ret = tuya_work_mode_set_manual();
        break;
    case MODE_AUTO:
        op_ret = tuya_work_mode_set_auto();
        break;
    case MODE_SLEEP:
        op_ret = tuya_work_mode_set_sleep();
        break;
    default:
        break;
    }

    return op_ret;       
}

/**
* @brief: tuya_power_switch
* @desc: switch air cleaner power on off
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_power_switch(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    g_power++;
    if (g_power > 1) {
        g_power = 0;
    }

    op_ret = tuya_power_set(g_power);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_power_set error:%d", op_ret); 
        return op_ret;
    }

    tuya_update_single_dp(SWITCH_DPID, PROP_BOOL, g_power);

    return op_ret; 
}

/**
* @brief: tuya_work_mode_switch
* @desc: switch air cleaner work mode
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_work_mode_switch(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    if (MODE_AUTO == g_work_mode) {
        tuya_auto_fan_speed_start(FALSE);  //close auto fan speed
        tuya_dark_scan_start(FALSE);       //close dark scan
    }

    g_work_mode++;
    if (g_work_mode > 2) {
        g_work_mode = 0;
    }

    op_ret = tuya_work_mode_set(g_work_mode);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_work_mode_set error:%d", op_ret); 
        return op_ret;
    }

    tuya_update_single_dp(MODE_DPID, PROP_ENUM, g_work_mode);

    return op_ret; 
}

/**
* @brief: tuya_air_cleaner_init
* @desc: air cleaner equipment init
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_air_cleaner_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    //close key function
    tuya_set_key_power_flag(FALSE);

    op_ret = tuya_read_save_data_from_flash();
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_wf_status_timeout_start error:%d", op_ret); 
        return op_ret;
    }

    op_ret = tuya_wf_status_timeout_start();
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_wf_status_timeout_start error:%d", op_ret); 
        return op_ret;
    }

    PR_NOTICE("tuya_air_cleaner_init sucess !!!");
    return op_ret;
}

/**
* @brief: tuya_recovery_from_fault
* @desc: tuya_recovery_from_fault
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_recovery_from_fault(VOID_T)
{
    if (MODE_AUTO == g_work_mode){
        tuya_auto_fan_speed_start(FALSE);  //close auto fan speed
        tuya_dark_scan_start(FALSE);       //close dark scan
    }

    tuya_work_mode_set(g_work_mode);
    
    return OPRT_OK;
}

/**
* @brief: tuya_get_power_status
* @desc: get power status
* 
* @return power_status
*/
POWER_TURN_E tuya_get_power_status(VOID_T)
{
    return g_power;
}