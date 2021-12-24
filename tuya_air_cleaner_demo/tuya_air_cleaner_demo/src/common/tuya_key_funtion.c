/**
* @file tuya_key_funtion.c
* @author ali.zhang@tuya.com
* @brief tuya_key_funtion module is used to driver key
* @version 1.0.0
* @date 2021-09-08
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
/***********************************************************
***************************includes*************************
***********************************************************/
#include "tuya_key_funtion.h"
#include "tuya_iot_wifi_api.h"
#include "tuya_iot_com_api.h"
#include "tuya_iot_internal_api.h"
#include "tuya_cloud_wifi_defs.h"
#include "uni_log.h"
#include "tuya_led.h"
#include "tuya_key.h"
#include "tuya_gpio.h"

#include "tuya_hardware_driver.h"
#include "tuya_buz_driver.h"
#include "tuya_air_quality_funtion.h"
#include "tuya_mode_funtion.h"


/***********************************************************
*************************micro define***********************
***********************************************************/

 
/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef struct
{
    BOOL_T init_flag;
    tuya_pin_name_t pin;
}TY_KEY_MSG; 
 
/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC TY_KEY_MSG g_key1_msg = 
{
    .init_flag = FALSE,
    .pin = TY_GPIOA_5,
};

STATIC TY_KEY_MSG g_key2_msg = 
{
    .init_flag = FALSE,
    .pin = TY_GPIOA_5,
};

STATIC BOOL_T g_child_lock = FALSE;
STATIC BOOL_T g_key_sound_close = FALSE;
STATIC BOOL_T g_key_power = TRUE;
 
/***********************************************************
***********************function define**********************
***********************************************************/
/**
* @brief: tuya_key1_funtion_handle_cb
* @desc: key1 function callback
* @return none
*/
STATIC VOID_T tuya_key1_funtion_handle_cb(TY_GPIO_PORT_E port, PUSH_KEY_TYPE_E type, INT_T cnt)
{
    if (FALSE == g_key1_msg.init_flag) {
        PR_NOTICE("key1 not init");
        return;
    }

    /* check if child lock is open */
    if (TRUE == g_child_lock) {
        PR_INFO("child lock is open, key not work");
        return;
    }

    /* long key or normal key */
    PR_NOTICE("port:%d, type:%d, cnt:%d", port, type, cnt);
    if (g_key1_msg.pin == port) {
        /* long key */
        if (LONG_KEY == type) {
            PR_NOTICE("key1 long press");
            tuya_power_switch();
            /* key sound */
            if (FALSE == g_key_sound_close) {
                tuya_buz_start(SOUND_KEY);
            }
        }
        /* check if work mode is power off */
        if (FALSE == g_key_power) {
            PR_INFO("work mode is power off, key1 normal not work");
            return;
        }
        /* normal key */
        if (NORMAL_KEY == type) {
            PR_NOTICE("key1 normal press");
            tuya_work_mode_switch();
            /* key sound */
            if (FALSE == g_key_sound_close) {
                tuya_buz_start(SOUND_KEY);
            }
        }
    }
}

/**
* @brief: tuya_key2_funtion_handle_cb
* @desc: key1 function callback
* @return none
*/
STATIC VOID_T tuya_key2_funtion_handle_cb(TY_GPIO_PORT_E port, PUSH_KEY_TYPE_E type, INT_T cnt)
{
    if (FALSE == g_key2_msg.init_flag) {
        PR_NOTICE("key2 not init");
        return;
    }

    /* check if child lock is open */
    if (TRUE == g_child_lock) {
        PR_INFO("child lock is open, key not work");
        return;
    }
    
    /* check if work mode is power off */
    if (FALSE == g_key_power) {
        PR_INFO("work mode is power off, key2 not work");
        return;
    }

    /* long key or normal key */
    PR_NOTICE("port:%d, type:%d, cnt:%d", port, type, cnt);
    if (g_key2_msg.pin == port) {
        /* key event */
        if (LONG_KEY == type) {
            PR_NOTICE("key2 long press");
            tuya_iot_wf_gw_unactive(); //Remove the device and access the distribution network 
        }
        if (NORMAL_KEY == type) {
            PR_NOTICE("key2 normal press");
            tuya_switch_show_air_quality_type(); //switch air quality type show on lcd
        }
        /* key sound */
        if (FALSE == g_key_sound_close) {
            tuya_buz_start(SOUND_KEY);
        }
    }
}

/** 
 * @brief: tuya_key1_funtion_init
 * @desc: tuya Infrared function initialization
 * @param[in] tuya_pin_name_t: The IO port of the module
 * @param[in] long_key_time_ms: Long press to take effect in milliseconds
 * @return OPERATE_RET  OPRT_OK is sucess, other is fail
 * @note none
 */
OPERATE_RET tuya_key1_funtion_init(IN tuya_pin_name_t key_pin, IN USHORT_T long_key_time_ms)
{
    if(TRUE == g_key1_msg.init_flag ) {
        PR_NOTICE("tuya_key1_init is ok,not need init");
        return OPRT_OK;
    }

    OPERATE_RET op_ret = OPRT_OK;

    KEY_USER_DEF_S key_cfg = {
        .port = key_pin,
        .long_key_time = long_key_time_ms,
        .call_back = tuya_key1_funtion_handle_cb,
        .low_level_detect = TRUE,
        .lp_tp = LP_ONCE_TRIG,
        .seq_key_detect_time = 400,
    };

    op_ret = key_init(NULL,0,25);
    if(OPRT_OK != op_ret) {
        PR_ERR("key_init err:%d",op_ret);
        return op_ret;
    }

    op_ret = reg_proc_key(&key_cfg);
    if(OPRT_OK  != op_ret) {
        PR_ERR("reg_proc_key err:%d", op_ret);
        return op_ret;
    }

    g_key1_msg.pin = key_pin;
    g_key1_msg.init_flag = TRUE;

    return OPRT_OK;
}

/** 
 * @brief: tuya_key2_funtion_init
 * @desc: tuya Infrared function initialization
 * @param[in] tuya_pin_name_t: The IO port of the module
 * @param[in] long_key_time_ms: Long press to take effect in milliseconds
 * @return OPERATE_RET  OPRT_OK is sucess, other is fail
 * @note none
 */
OPERATE_RET tuya_key2_funtion_init(IN tuya_pin_name_t key_pin,IN USHORT_T long_key_time_ms)
{
    if(TRUE == g_key2_msg.init_flag ) {
        PR_NOTICE("tuya_key2_init is ok,not need init");
        return OPRT_OK;
    }

    OPERATE_RET op_ret = OPRT_OK;

    KEY_USER_DEF_S key_cfg = {
        .port = key_pin,
        .long_key_time = long_key_time_ms,
        .call_back = tuya_key2_funtion_handle_cb,
        .low_level_detect = TRUE,
        .lp_tp = LP_ONCE_TRIG,
        .seq_key_detect_time = 400,
    };
    
    op_ret = key_init(NULL,0,25);
    if(OPRT_OK != op_ret) {
        PR_ERR("key_init err:%d", op_ret);
        return op_ret;
    }

    op_ret = reg_proc_key(&key_cfg);
    if(OPRT_OK  != op_ret) {
        PR_ERR("reg_proc_key err:%d", op_ret);
        return op_ret;
    }

    g_key2_msg.pin = key_pin;
    g_key2_msg.init_flag = TRUE;

    return OPRT_OK;
}

/** 
 * @brief: tuya_key_funtion_init
 * @desc: tuya Infrared function initialization
 * @return OPERATE_RET  OPRT_OK is sucess, other is fail
 * @note none
 */
OPERATE_RET tuya_key_funtion_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = tuya_key1_funtion_init(TY_KEY1_PIN, TY_KEY1_LONG_TIME_MS);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_key1_funtion_init err:%d", op_ret);
        return op_ret;
    }
    op_ret = tuya_key2_funtion_init(TY_KEY2_PIN, TY_KEY2_LONG_TIME_MS);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_key2_funtion_init err:%d", op_ret);
        return op_ret;
    }

    PR_NOTICE("tuya_key_funtion_init sucess !!!");
    return OPRT_OK;
}

/**
* @brief: tuya_set_child_lock_value
* @desc: set child_lock value
* @param[in] child_lock
* @return OPERATE_RET
*/
OPERATE_RET tuya_set_child_lock_value(IN CONST BOOL_T child_lock)
{
    g_child_lock = child_lock;
    return OPRT_OK;
}

/**
* @brief: tuya_set_sound_switch_value
* @desc: set key_sound_close value
* @param[in] key_sound_close
* @return OPERATE_RET
*/
OPERATE_RET tuya_set_key_sound_value(IN CONST BOOL_T key_sound_close)
{
    g_key_sound_close = key_sound_close;
    return OPRT_OK;
}

/**
* @brief: tuya_set_key_power_flag
* @desc: set key_power flag
* @param[in] key_power
* @return OPERATE_RET
*/
OPERATE_RET tuya_set_key_power_flag(IN CONST BOOL_T key_power)
{
    g_key_power = key_power;
    return OPRT_OK;
}