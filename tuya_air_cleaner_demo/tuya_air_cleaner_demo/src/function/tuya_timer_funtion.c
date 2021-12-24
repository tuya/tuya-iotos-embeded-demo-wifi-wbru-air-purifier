/**
* @file tuya_timer_funtion.c
* @author ali.zhang@tuya.com
* @brief tuya_timer_funtion module is used to 
* @version 1.0.0
* @date 2021-09-14
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_timer_funtion.h"
#include "uni_log.h"
#include "sys_timer.h"
#include "tuya_buz_driver.h"
#include "tuya_cloud_wifi_defs.h"
#include "tuya_dp_process.h"
#include "tuya_automatic_mode_funtion.h"
#include "tuya_hardware_driver.h"
 
/*********************************************************************
******************************micro define****************************
*********************************************************************/

 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/
typedef struct {
    BOOL_T pm25_scan;
    BOOL_T dark_scan;
} TIMER_START_FLAG_T;
 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
STATIC TIMER_ID timer_wf_status;
STATIC TIMER_ID timer_pm25_scan;
STATIC TIMER_ID timer_uv_close;
STATIC TIMER_ID timer_dark_scan;

STATIC TIMER_START_FLAG_T timer_start_flag;

/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_wf_status_timeout_cb
* @desc: wifi status timeout callback function
* 
* @return none
*/
STATIC VOID_T tuya_wf_status_timeout_cb()
{
    OPERATE_RET op_ret = OPRT_OK;
    GW_WIFI_NW_STAT_E wifi_state = STAT_LOW_POWER;
    
    op_ret = get_wf_gw_nw_status(&wifi_state);
    if (OPRT_OK != op_ret) {
        PR_ERR("get_wf_gw_nw_status error:%d", op_ret);
        return;
    }
    
    PR_DEBUG("*******************wifi_state:%d", wifi_state);
    if (wifi_state <= STAT_AP_STA_DISC || wifi_state == STAT_STA_DISC) {
        return;
    }  

    tuya_update_dp_initial_value();    //update dp

    sys_stop_timer(timer_wf_status);   //delay timer
    sys_delete_timer(timer_wf_status); //delete timer
    
    return;
}

/**
* @brief: tuya_pm25_scan_timer_cb
* @desc: pm2.5 sacn callback function
* 
* @return none
*/
STATIC VOID_T tuya_pm25_scan_timer_cb()
{
    tuya_pm25_scan();
    return;
}

/**
* @brief: tuya_dark_scan_timer_cb
* @desc: dark sacn callback function
* 
* @return none
*/
STATIC VOID_T tuya_dark_scan_timer_cb()
{
    if (TRUE == DARK_FLAG) {
        tuya_hal_system_sleep(1000);
        if (TRUE == DARK_FLAG) {    //into dark
            tuya_dark_scan(TRUE);
        }
    } else {
        tuya_hal_system_sleep(1000);
        if (FALSE == DARK_FLAG) {   //into bright
            tuya_dark_scan(FALSE);
        }
    }   
    return;
}

/**
* @brief: tuya_uv_close_timer_cb
* @desc: uv close callback function
* 
* @return none
*/
STATIC VOID_T tuya_uv_close_timer_cb()
{
    tuya_uv_set_on(FALSE);

    sys_stop_timer(timer_uv_close);

    return;
}

/**
* @brief: tuya_wf_status_timeout_start
* @desc: start wifi status timeout, cycle count 2000ms
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_wf_status_timeout_start(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = sys_add_timer(tuya_wf_status_timeout_cb, NULL, &timer_wf_status);
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_add_timer error:%d", op_ret);
        return op_ret;
    } 

    op_ret = sys_start_timer(timer_wf_status, 2000, TIMER_CYCLE); //Start the timer, cycle count 2000ms 
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_start_timer error:%d", op_ret);
        return op_ret;
    }  

    return op_ret;
}

/**
* @brief: tuya_pm25_scan_timer_start
* @desc: start pm25 scan, cycle count 30s
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_pm25_scan_timer_start(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    if (TRUE == timer_start_flag.pm25_scan) {
        PR_DEBUG("pm25 scan is already start, dont need start again");
        return OPRT_OK;
    }

    op_ret = sys_add_timer(tuya_pm25_scan_timer_cb, NULL, &timer_pm25_scan);
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_add_timer error:%d", op_ret);
        return op_ret;
    } 

    op_ret = sys_start_timer(timer_pm25_scan, 30*1000, TIMER_CYCLE); //Start the timer, cycle count 30s
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_start_timer error:%d", op_ret);
        return op_ret;
    }  

    timer_start_flag.pm25_scan = TRUE;

    return op_ret;
}

/**
* @brief: tuya_pm25_scan_timer_stop
* @desc: tuya_pm25_scan_timer_stop
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_pm25_scan_timer_stop(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = sys_stop_timer(timer_pm25_scan);
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_stop_timer error:%d", op_ret);
        return op_ret;
    }

    op_ret = sys_delete_timer(timer_pm25_scan);
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_delete_timer error:%d", op_ret);
        return op_ret;
    }

    timer_start_flag.pm25_scan = FALSE;

    return op_ret;
}

/**
* @brief: tuya_dark_scan_timer_start
* @desc: start dark scan, cycle count 10s
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_dark_scan_timer_start(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    if (TRUE == timer_start_flag.dark_scan) {
        PR_DEBUG("dark scan is already start, dont need start again");
        return OPRT_OK;
    }

    op_ret = sys_add_timer(tuya_dark_scan_timer_cb, NULL, &timer_dark_scan);
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_add_timer error:%d", op_ret);
        return op_ret;
    } 

    op_ret = sys_start_timer(timer_dark_scan, 3*1000, TIMER_CYCLE); //Start the timer, cycle count 3s 
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_start_timer error:%d", op_ret);
        return op_ret;
    }

    timer_start_flag.dark_scan = TRUE;

    return op_ret;
}

/**
* @brief: tuya_dark_scan_timer_stop
* @desc: tuya_dark_scan_timer_stop
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_dark_scan_timer_stop(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = sys_stop_timer(timer_dark_scan);
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_stop_timer error:%d", op_ret);
        return op_ret;
    }

    op_ret = sys_delete_timer(timer_dark_scan);
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_delete_timer error:%d", op_ret);
        return op_ret;
    }

    timer_start_flag.dark_scan = FALSE;

    return op_ret;
}

/**
* @brief: tuya_uv_close_timer_start
* @desc: start uv close timer, count 60min
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_uv_close_timer_start(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = sys_add_timer(tuya_uv_close_timer_cb, NULL, &timer_uv_close);
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_add_timer error:%d", op_ret);
        return op_ret;
    } 

    op_ret = sys_start_timer(timer_uv_close, 3600*1000, TIMER_ONCE); //Start the timer, count 60min 
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_start_timer error:%d", op_ret);
        return op_ret;
    }  

    return op_ret;
}

