/**
* @file tuya_countdown_funtion.c
* @author ali.zhang@tuya.com
* @brief tuya_countdown_funtion module is used to countdown funtion
* @version 1.0.0
* @date 2021-10-13
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_countdown_funtion.h"
#include "uni_log.h"
#include "sys_timer.h"
#include "tuya_mode_funtion.h"
#include "tuya_dp_process.h"
 
/*********************************************************************
******************************micro define****************************
*********************************************************************/
 
 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/
 
 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
STATIC TIMER_ID timer_countdown_set;

STATIC UINT_T g_countdown_left_time_minute = 0;
 
/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_countdown_timer_cb
* @desc: countdown timer callback function
* 
* @return none
*/
STATIC VOID_T tuya_countdown_timer_cb()
{
    g_countdown_left_time_minute--;
    
    /* report dp countdown_left */
    tuya_update_single_dp(COUNTDOWN_LEFT_DPID, PROP_VALUE, g_countdown_left_time_minute);
    
    /* if countdown left time is 0 */
    if (0 == g_countdown_left_time_minute) {
        tuya_power_switch();  //power on off
        tuya_update_single_dp(COUNTDOWN_SET_DPID, PROP_ENUM, COUNTDOWN_SET_CANCEL);  //dp report, countdown set cancel
        sys_stop_timer(timer_countdown_set);
    }

    return;
}

/**
* @brief: tuya_countdown_timer_start
* @desc: start countdown timer
* @param[in] countdown_set
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_countdown_timer_start(IN CONST COUNTDOWN_SET_E countdown_set)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* if countdown is still working, stop timer */
    if (0 != g_countdown_left_time_minute) { 
        sys_stop_timer(timer_countdown_set);
    }

    /* set countdown_left time */
    g_countdown_left_time_minute = countdown_set * 60;

    /* report dp countdown_left */
    tuya_update_single_dp(COUNTDOWN_LEFT_DPID, PROP_VALUE, g_countdown_left_time_minute);

    /* if countdown_set is cancle, return */
    if (countdown_set == COUNTDOWN_SET_CANCEL) {
        return op_ret;
    }

    op_ret = sys_add_timer(tuya_countdown_timer_cb, NULL, &timer_countdown_set);
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_add_timer error:%d", op_ret);
        return op_ret;
    } 

    op_ret = sys_start_timer(timer_countdown_set, 60*1000, TIMER_CYCLE); //Start the timer, cycle count 1min
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_start_timer error:%d", op_ret);
        return op_ret;
    } 

    return op_ret;
}
