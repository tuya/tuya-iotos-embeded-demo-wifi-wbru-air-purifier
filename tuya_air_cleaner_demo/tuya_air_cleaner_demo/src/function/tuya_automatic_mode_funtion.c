/**
* @file tuya_automatic_mode_funtion.c
* @author ali.zhang@tuya.com
* @brief tuya_automatic_mode_funtion module is used to automatic mode funtion
* @version 1.0.0
* @date 2021-10-09
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_automatic_mode_funtion.h"
#include "uni_log.h"
#include "tuya_motor_driver.h"
#include "tuya_timer_funtion.h"
#include "tuya_hardware_driver.h"
#include "tuya_mode_funtion.h"

/*********************************************************************
******************************micro define****************************
*********************************************************************/
 
 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/
typedef struct {
    BOOL_T pm25_75_115;
    BOOL_T pm25_35_75;
    BOOL_T pm25_35_75_again;
} PM25_REDUCE_COMPARE_FLAG_T;
 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
STATIC AIR_QUALITY_INDEX_T  g_air_quality_index;
STATIC PM25_REDUCE_COMPARE_FLAG_T compare_flag;

STATIC INT_T   g_pm25_reduction = 0;
STATIC UINT_T  g_pm25_last = 0;
STATIC UINT_T  g_pm25_good_time = 0;
STATIC UINT_T  g_pm25_good_time_dark = 0;

STATIC BOOL_T  motor_already_stop   = FALSE;
STATIC BOOL_T  stop_auto_speed_flag = FALSE;

STATIC BOOL_T  g_uv_switch = FALSE;
STATIC BOOL_T  g_uv_status = FALSE;
STATIC BOOL_T  g_dark_flag = FALSE;

/*********************************************************************
****************************function define***************************
*********************************************************************
/**
* @brief: tuya_set_air_quality_index
* @desc: set air quality index
* @param[in] air_quality_index
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_set_air_quality_index(IN AIR_QUALITY_INDEX_T air_quality_index)
{
    g_air_quality_index.pm25 = air_quality_index.pm25;
    g_air_quality_index.pm10 = air_quality_index.pm10;
    g_air_quality_index.tvoc = air_quality_index.tvoc;
    g_air_quality_index.temperature = air_quality_index.temperature;
    g_air_quality_index.humidity = air_quality_index.humidity;
    
    return OPRT_OK;
}

/**
* @brief: tuya_pm25_compare_75_115
* @desc: tuya_pm25_compare_75_115
* 
* @return speed_percent
*/
STATIC FLOAT_T tuya_pm25_compare_75_115(VOID_T)
{
    if (g_pm25_reduction < PM25_REDUCTION_COMP) {
        return 1;
    } else {
        return 0.8;
    }
}

/**
* @brief: tuya_pm25_compare_35_75
* @desc: tuya_pm25_compare_35_75
* 
* @return speed_percent
*/
STATIC FLOAT_T tuya_pm25_compare_35_75(VOID_T)
{
    if (g_pm25_reduction < PM25_REDUCTION_COMP) {
        compare_flag.pm25_35_75_again = TRUE;
        return 0.8;
    } else {
        return 0.6;
    }
}

/**
* @brief: tuya_pm25_compare_35_75_again
* @desc: tuya_pm25_compare_35_75_again
* 
* @return speed_percent
*/
STATIC FLOAT_T tuya_pm25_compare_35_75_again(VOID_T)
{
    if (g_pm25_reduction < PM25_REDUCTION_COMP) {
        compare_flag.pm25_35_75_again = TRUE;
        return 1;
    } else {
        return 0.8;
    }
}

/**
* @brief: tuya_pm25_scan
* @desc: scan pm2.5 value once
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_pm25_scan(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    PM25_VALUE_RANGE_E pm25_value_range;
    FLOAT_T speed_percent = 0;
    UINT_T pm25_new = 0;

    pm25_new = g_air_quality_index.pm25;
    g_pm25_reduction = g_pm25_last - pm25_new;
    g_pm25_last = pm25_new;

    /* Check whether to into the dark sleep mode */
    if (TRUE == stop_auto_speed_flag) {
        return op_ret;
    }

    /* Check whether to into the environmental mode */
    if (pm25_new < 35) {
        //hold for 2 hours, 240*30 s
        if (g_pm25_good_time++ >= 240) {
            if (g_pm25_good_time > 1000) {
                g_pm25_good_time = 1000;
            }
            if (FALSE == motor_already_stop) {
                tuya_motor_stop();
                motor_already_stop = TRUE;
            }
            return op_ret;
        }
    } else {
        g_pm25_good_time = 0;
        motor_already_stop = FALSE;
    }

    /* Check pm2.5 value range */
    if (pm25_new >= 115) {
        pm25_value_range = PM25_RANGE_115;
    } else if (pm25_new >= 75) {
        pm25_value_range = PM25_RANGE_75_115;
    } else if (pm25_new >= 35) {
        pm25_value_range = PM25_RANGE_35_75;
    } else {
        pm25_value_range = PM25_RANGE_35;
    }

    /* control fan speed */
    switch (pm25_value_range)
    {
        /* pm2.5: 115+ */
        case PM25_RANGE_115:
            speed_percent = 1;
            break;
        /* pm2.5: 75-115 */
        case PM25_RANGE_75_115:
            if (TRUE == compare_flag.pm25_35_75_again) {
                speed_percent = 1;
            } else {
                speed_percent = 0.8;
            }

            if (TRUE == compare_flag.pm25_75_115) {
                speed_percent = tuya_pm25_compare_75_115();
            } else {
                compare_flag.pm25_75_115 = TRUE;
            }
            break;
        /* pm2.5: 35-75 */
        case PM25_RANGE_35_75:
            compare_flag.pm25_75_115 = FALSE;
            speed_percent = 0.6;
            if (TRUE == compare_flag.pm25_35_75) {
                speed_percent = tuya_pm25_compare_35_75();
                compare_flag.pm25_35_75 = FALSE;
            } else if ((TRUE == compare_flag.pm25_35_75_again)) {
                speed_percent = tuya_pm25_compare_35_75_again();
            } else {
                compare_flag.pm25_35_75 = TRUE;
            }
            break;
        /* pm2.5: 0-35 */
        case PM25_RANGE_35:
            compare_flag.pm25_75_115 = FALSE;
            compare_flag.pm25_35_75_again = FALSE;
            speed_percent = 0.4;
            break;  
        default:
            break;
    }

    /* start fan motor */
    op_ret = tuya_motor_start(speed_percent);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_motor_start error:%d", op_ret);
        return op_ret;
    }  

    return op_ret;
}

/**
* @brief: tuya_auto_fan_speed_start
* @desc: start auto fan speed
* @param[in] start:
*             TRUE: start
*             FLASE: stop
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_auto_fan_speed_start(IN CONST BOOL_T start)
{
    OPERATE_RET op_ret = OPRT_OK;

    if (FALSE == start) {
        tuya_pm25_scan_timer_stop();
        PR_DEBUG("stop auto fan speed");
        /* release flag */
        compare_flag.pm25_75_115 = FALSE;
        compare_flag.pm25_35_75 = FALSE;
        compare_flag.pm25_35_75_again = FALSE;
        motor_already_stop = FALSE;
        return op_ret;
    }

    /* release time */
    g_pm25_good_time = 0;

    /* scan pm2.5 value once */
    PR_DEBUG("start auto fan speed");
    tuya_pm25_scan();

    /* cycle scan pm2.5, 30s */
    tuya_pm25_scan_timer_start();

    return op_ret;
}

/**
* @brief: tuya_uv_funtion_scan
* @desc: scan uv funtion 
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_uv_funtion_scan(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    POWER_TURN_E  power_status = POWER_ON;

    power_status = tuya_get_power_status();

    if (POWER_OFF == power_status) {
        return op_ret;
    }

    if (FALSE == g_uv_switch) {
        return op_ret;
    }

    /* if temperature > 20, humidity > 65%, open uv */
    if ((g_air_quality_index.temperature > 20) && (g_air_quality_index.humidity > 65)) {
        tuya_uv_set_on(TRUE);
        tuya_uv_close_timer_start(); //count 60min, then close uv
        PR_DEBUG("uv open 60min");
    }

    return op_ret;
}

/**
* @brief: tuya_dark_scan
* @desc: scan dark funtion 
* @param[in] dark_flag
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_dark_scan(IN CONST BOOL_T dark_flag)
{
    OPERATE_RET op_ret = OPRT_OK;

    switch (dark_flag) {

    /* into bright */
    case FALSE:
        if (g_dark_flag != dark_flag) {
            tuya_light_switch_set(TRUE); //open light
        }
        g_dark_flag = dark_flag;
        /* release flag */
        g_pm25_good_time_dark = 0;
        stop_auto_speed_flag = FALSE;
        break;

    /* into dark */
    case TRUE:
        if (g_dark_flag != dark_flag) {
            tuya_light_switch_set(FALSE); //close light
        }
        g_dark_flag = dark_flag;

        /* if air quality pm2.5 is good */
        if (g_air_quality_index.pm25 < 35) {
            //hold for 30 minutes, 600*3s
            if (g_pm25_good_time_dark++ >= 600) {
                if (g_pm25_good_time_dark > 10000) {
                    g_pm25_good_time_dark = 10000;
                }
                stop_auto_speed_flag = TRUE;
                tuya_motor_start(0.2);
            }
        } else {
            /* release flag */
            g_pm25_good_time_dark = 0;
            stop_auto_speed_flag = FALSE;
        }
        break;   
    default:
        break;
    }

    return op_ret;
}

/**
* @brief: tuya_dark_scan_start
* @desc: start scan dark cycle 10s
* @param[in] start:
*             TRUE: start
*             FLASE: stop
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_dark_scan_start(IN CONST BOOL_T start)
{
    OPERATE_RET op_ret = OPRT_OK;

    if (TRUE == start) {
        PR_DEBUG("ali, start dark scan");
        tuya_dark_scan_timer_start();
        g_pm25_good_time_dark = 0;
    } else {
        PR_DEBUG("ali, stop dark scan");
        tuya_dark_scan_timer_stop();
        /* release flag */
        stop_auto_speed_flag = FALSE;
    }

    return op_ret;
}

/**
* @brief: tuya_set_uv_switch_value
* @desc: set uv_switch value
* @param[in] uv_switch
* @return OPERATE_RET
*/
OPERATE_RET tuya_set_uv_switch_value(IN CONST BOOL_T uv_switch)
{
    g_uv_switch = uv_switch;
    if (FALSE == g_uv_switch) {
        tuya_uv_set_on(FALSE);
    }
    return OPRT_OK;
}

/**
* @brief: tuya_set_uv_status
* @desc: set uv status
* @param[in] uv_status
* @return OPERATE_RET
*/
OPERATE_RET tuya_set_uv_status(IN CONST BOOL_T uv_status)
{
    g_uv_status = uv_status;
    return OPRT_OK;
}

/**
* @brief: tuya_get_uv_status
* @desc: get uv status
* @param[in] uv_status
* @return OPERATE_RET
*/
BOOL_T tuya_get_uv_status(VOID_T)
{
    return g_uv_status;
}