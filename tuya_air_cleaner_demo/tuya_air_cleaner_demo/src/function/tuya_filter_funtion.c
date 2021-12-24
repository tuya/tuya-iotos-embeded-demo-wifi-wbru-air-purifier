/**
* @file tuya_filter_funtion.c
* @author ali.zhang@tuya.com
* @brief tuya_filter_funtion module is used to filter funtion
* @version 1.0.0
* @date 2021-10-14
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_filter_funtion.h"
#include "uni_log.h"
#include "tuya_motor_driver.h"
#include "tuya_dp_process.h"
#include "tuya_mode_funtion.h"
#include "tuya_ws_db.h"
#include "uni_time.h"
#include "uni_thread.h"
#include "tuya_automatic_mode_funtion.h"
 
/*********************************************************************
******************************micro define****************************
*********************************************************************/
#define   FILTER_FIGURE_CONST            (327)       //filter pm total figure const
#define   FILTER_MAX_PM_CAN_ABSORB       (12000000)  //max pm, 12000000ug

#define   PM_TOTAL_SAVE                  "pm_total_save"
#define   FILTER_PM_TOTAL_SAVE           "filter_pm_total_save"
#define   FILTER_USE_DAYS_SAVE           "filter_use_days_save" 
#define   RUNTIME_TOTAL_SAVE             "runtime_total_save"
#define   NOW_DAYS_SAVE                  "now_days_save"
#define   RUN_DAYS_SAVE                  "run_days_save"


 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/


/*********************************************************************
****************************variable define***************************
*********************************************************************/
STATIC TY_FILTER_MSG_T  g_filter_msg;
STATIC POWER_TURN_E     g_power = POWER_OFF;

STATIC UINT_T g_now_days        = 0;
STATIC UINT_T g_run_days        = 0;
STATIC UINT_T g_runtime_total   = 0;
STATIC UINT_T g_filter_pm_total = 0;
STATIC UINT_T g_filter_use_days = 0;


/*********************************************************************
**********************private function prototypes*********************
*********************************************************************/
STATIC VOID_T tuya_filter_1hour_task(VOID_T);

/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_write_pm_total
* @desc: write pm_total in flash
* @param[in] pm_total:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_write_pm_total(IN UINT_T pm_total)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T value_in = pm_total;

    op_ret = wd_common_write(PM_TOTAL_SAVE, &value_in, sizeof(UINT_T));
    if (OPRT_OK != op_ret) {
        PR_ERR("wd_common_write err:%d", op_ret);
        return op_ret;
    }

    PR_DEBUG("ali, flash write, pm_total:%d", value_in);
    return OPRT_OK;
}

/**
* @brief: tuya_read_pm_total
* @desc: read pm_total from flash
* @param[in] pm_total:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_read_pm_total(IN OUT UINT_T *pm_total)
{
    OPERATE_RET op_ret = OPRT_OK;
    BYTE_T *buff = NULL;
    UINT_T buff_len = 0;

    op_ret = wd_common_read(PM_TOTAL_SAVE, &buff, &buff_len);
    if (OPRT_OK != op_ret) {
        PR_NOTICE("wd_common_read, pm_total is null!!");
        return OPRT_OK;
    }

    UINT_T *temp = (UINT_T *)buff;
    *pm_total = *temp;

    buff = NULL;
    temp = NULL;

    PR_DEBUG("ali, flash read, pm_total:%d", *pm_total);
    return OPRT_OK;
}

/**
* @brief: tuya_write_filter_pm_total
* @desc: write filter_pm_total in flash
* @param[in] filter_pm_total:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_write_filter_pm_total(IN UINT_T filter_pm_total)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T value_in = filter_pm_total;

    op_ret = wd_common_write(FILTER_PM_TOTAL_SAVE, &value_in, sizeof(UINT_T));
    if (OPRT_OK != op_ret) {
        PR_ERR("wd_common_write err:%d", op_ret);
        return op_ret;
    }

    PR_DEBUG("ali, flash write, filter_pm_total:%d", value_in);
    return OPRT_OK;
}

/**
* @brief: tuya_read_filter_pm_total
* @desc: read filter_pm_total from flash
* @param[in] filter_pm_total:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_read_filter_pm_total(IN OUT UINT_T *filter_pm_total)
{
    OPERATE_RET op_ret = OPRT_OK;
    BYTE_T *buff = NULL;
    UINT_T buff_len = 0;

    op_ret = wd_common_read(FILTER_PM_TOTAL_SAVE, &buff, &buff_len);
    if (OPRT_OK != op_ret) {
        PR_NOTICE("wd_common_read, filter_pm_total is null!!");
        return OPRT_OK;
    }

    UINT_T *temp = (UINT_T *)buff;
    *filter_pm_total = *temp;

    buff = NULL;
    temp = NULL;

    PR_DEBUG("ali, flash read, filter_pm_total:%d", *filter_pm_total);
    return OPRT_OK;
}

/**
* @brief: tuya_write_filter_use_days
* @desc: write filter_use_days in flash
* @param[in] filter_use_days:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_write_filter_use_days(IN UINT_T filter_use_days)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T value_in = filter_use_days;

    op_ret = wd_common_write(FILTER_USE_DAYS_SAVE, &value_in, sizeof(UINT_T));
    if (OPRT_OK != op_ret) {
        PR_ERR("wd_common_write err:%d", op_ret);
        return op_ret;
    }

    PR_DEBUG("ali, flash write, filter_use_days:%d", value_in);
    return OPRT_OK;
}

/**
* @brief: tuya_read_filter_use_days
* @desc: read filter_use_days from flash
* @param[in] filter_use_days:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_read_filter_use_days(IN OUT UINT_T *filter_use_days)
{
    OPERATE_RET op_ret = OPRT_OK;
    BYTE_T *buff = NULL;
    UINT_T buff_len = 0;

    op_ret = wd_common_read(FILTER_USE_DAYS_SAVE, &buff, &buff_len);
    if (OPRT_OK != op_ret) {
        PR_NOTICE("wd_common_read, filter_use_days is null!!");
        return OPRT_OK;
    }

    UINT_T *temp = (UINT_T *)buff;
    *filter_use_days = *temp;

    buff = NULL;
    temp = NULL;

    PR_DEBUG("ali, flash read, filter_use_days:%d", *filter_use_days);
    return OPRT_OK;
}

/**
* @brief: tuya_write_runtime_total
* @desc: write runtime_total in flash
* @param[in] runtime_total:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_write_runtime_total(IN UINT_T runtime_total)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T value_in = runtime_total;

    op_ret = wd_common_write(RUNTIME_TOTAL_SAVE, &value_in, sizeof(UINT_T));
    if (OPRT_OK != op_ret) {
        PR_ERR("wd_common_write err:%d", op_ret);
        return op_ret;
    }

    PR_DEBUG("ali, flash write, runtime_total:%d", value_in);
    return OPRT_OK;
}

/**
* @brief: tuya_read_runtime_total
* @desc: read runtime_total from flash
* @param[in] runtime_total:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_read_runtime_total(IN OUT UINT_T *runtime_total)
{
    OPERATE_RET op_ret = OPRT_OK;
    BYTE_T *buff = NULL;
    UINT_T buff_len = 0;

    op_ret = wd_common_read(RUNTIME_TOTAL_SAVE, &buff, &buff_len);
    if (OPRT_OK != op_ret) {
        PR_NOTICE("wd_common_read, runtime_total is null!!");
        return OPRT_OK;
    }

    UINT_T *temp = (UINT_T *)buff;
    *runtime_total = *temp;

    buff = NULL;
    temp = NULL;

    PR_DEBUG("ali, flash read, runtime_total:%d", *runtime_total);
    return OPRT_OK;
}

/**
* @brief: tuya_write_now_days
* @desc: write now_days in flash
* @param[in] now_days:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_write_now_days(IN UINT_T now_days)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T value_in = now_days;

    op_ret = wd_common_write(NOW_DAYS_SAVE, &value_in, sizeof(UINT_T));
    if (OPRT_OK != op_ret) {
        PR_ERR("wd_common_write err:%d", op_ret);
        return op_ret;
    }

    PR_DEBUG("ali, flash write, now_days:%d", value_in);
    return OPRT_OK;
}

/**
* @brief: tuya_read_now_days
* @desc: read now_days from flash
* @param[in] now_days:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_read_now_days(IN OUT UINT_T *now_days)
{
    OPERATE_RET op_ret = OPRT_OK;
    BYTE_T *buff = NULL;
    UINT_T buff_len = 0;

    op_ret = wd_common_read(NOW_DAYS_SAVE, &buff, &buff_len);
    if (OPRT_OK != op_ret) {
        PR_NOTICE("wd_common_read, now_days is null!!");
        return OPRT_OK;
    }

    UINT_T *temp = (UINT_T *)buff;
    *now_days = *temp;

    buff = NULL;
    temp = NULL;

    PR_DEBUG("ali, flash read, now_days:%d", *now_days);
    return OPRT_OK;
}

/**
* @brief: tuya_write_run_days
* @desc: write run_days in flash
* @param[in] run_days:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_write_run_days(IN UINT_T run_days)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T value_in = run_days;

    op_ret = wd_common_write(RUN_DAYS_SAVE, &value_in, sizeof(UINT_T));
    if (OPRT_OK != op_ret) {
        PR_ERR("wd_common_write err:%d", op_ret);
        return op_ret;
    }

    PR_DEBUG("ali, flash write, run_days:%d", value_in);
    return OPRT_OK;
}

/**
* @brief: tuya_read_run_days
* @desc: read run_days from flash
* @param[in] run_days:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_read_run_days(IN OUT UINT_T *run_days)
{
    OPERATE_RET op_ret = OPRT_OK;
    BYTE_T *buff = NULL;
    UINT_T buff_len = 0;

    op_ret = wd_common_read(RUN_DAYS_SAVE, &buff, &buff_len);
    if (OPRT_OK != op_ret) {
        PR_NOTICE("wd_common_read, run_days is null!!");
        return OPRT_OK;
    }

    UINT_T *temp = (UINT_T *)buff;
    *run_days = *temp;

    buff = NULL;
    temp = NULL;

    PR_DEBUG("ali, flash read, run_days:%d", *run_days);
    return OPRT_OK;
}

/**
* @brief: tuya_filter_1minute_task
* @desc: filter 1minute task
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC VOID_T tuya_filter_1minute_task(VOID_T)
{
    STATIC UINT_T uv_close_time = 240;
    STATIC UINT_T s_tick;

    while(1) 
    {
        UINT_T fan_speed_percent = 0;
        
        /* check whether air cleaner power on */
        if (POWER_ON == tuya_get_power_status()) {
            //get current fan_speed
            fan_speed_percent = tuya_get_fan_speed_percent();
            //figure pm_total
            g_filter_pm_total = g_filter_pm_total + (FLOAT_T)fan_speed_percent/100 * FILTER_FIGURE_CONST;
            g_filter_msg.pm_total = g_filter_msg.pm_total + (FLOAT_T)fan_speed_percent/100 * FILTER_FIGURE_CONST;

            //DP report, pm_total
            tuya_update_single_dp(PM_TOTAL_DPID, PROP_VALUE, g_filter_msg.pm_total/1000);  //ug -> mg
            //DP report, runtime_total
            tuya_update_single_dp(RUNTIME_TOTAL_DPID, PROP_VALUE, ++g_runtime_total);  //runtime_total add
        }

        /* enter every 60 minutes, 1hour */
        if (s_tick++ >= 60){
            s_tick = 0;
            tuya_filter_1hour_task();
        }

        /* if uv close time more than 240min, scan for uv funtion */
        if (FALSE == tuya_get_uv_status) {
            if (uv_close_time++ >= 240) {
                tuya_uv_funtion_scan();
                if (uv_close_time > 1000) {
                    uv_close_time = 1000;
                }
            }
        } else {
            uv_close_time = 0; 
        }

        tuya_hal_system_sleep(FILTER_TASK_DELAY_TIME); //1min   
    }
}

/**
* @brief: tuya_filter_1hour_task
* @desc: filter 1hour task
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC VOID_T tuya_filter_1hour_task(VOID_T)
{
    POSIX_TM_S local_time;
    UINT_T now_days = 0;

    uni_local_time_get(&local_time);
    now_days = local_time.tm_year*365 + local_time.tm_mon*30 + local_time.tm_mday;

    if (g_now_days == 0) {
        g_now_days = now_days;
    }

    /* if days pass */
    if (g_now_days < now_days) {
        /* figure filter_use_days */
        g_filter_use_days = g_filter_use_days + (now_days - g_now_days);
        /* figure air cleaner run_days */
        g_run_days = g_run_days + (now_days - g_now_days);

        /* if air cleaner run_days more than 7 days */
        if (g_run_days >= 7) {
            //figure filter_life
            g_filter_msg.filter_life = (1 - (FLOAT_T)g_filter_pm_total / FILTER_MAX_PM_CAN_ABSORB) * 100;
            //figure filter_days
            if (g_filter_pm_total == 0) {
                g_filter_msg.filter_days = 180;
            } else {
                g_filter_msg.filter_days = ((FILTER_MAX_PM_CAN_ABSORB * g_filter_use_days) / g_filter_pm_total) - g_filter_use_days;
            }
            
            //DP report
            if (g_filter_msg.filter_days > 1000) {
                g_filter_msg.filter_days = 1000;
            }
            tuya_update_single_dp(FILTER_DAYS_DPID, PROP_VALUE, g_filter_msg.filter_days);  //filter_days
            tuya_update_single_dp(FILTER_LIFE_DPID, PROP_VALUE, g_filter_msg.filter_life);  //filter_life

            g_run_days = 0;
        }

        g_now_days = now_days;

        /* write save data in flash */
        tuya_write_save_data_in_flash();
    }
}

/**
* @brief: tuya_read_save_data_from_flash
* @desc: read save data from flash, then set value
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_read_save_data_from_flash(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T pm_total        = 0;
    UINT_T filter_pm_total = 0;
    UINT_T filter_use_days = 0;
    UINT_T runtime_total   = 0;
    UINT_T now_days        = 0;
    UINT_T run_days        = 0;

    op_ret = tuya_read_pm_total(&pm_total);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_read_pm_total err:%d", op_ret);
        return op_ret;
    }

    op_ret = tuya_read_filter_pm_total(&filter_pm_total);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_read_filter_pm_total err:%d", op_ret);
        return op_ret;
    }

    op_ret = tuya_read_filter_use_days(&filter_use_days);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_read_filter_use_days err:%d", op_ret);
        return op_ret;
    }

    op_ret = tuya_read_runtime_total(&runtime_total);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_read_runtime_total err:%d", op_ret);
        return op_ret;
    }

    op_ret = tuya_read_now_days(&now_days);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_read_now_days err:%d", op_ret);
        return op_ret;
    }

    op_ret = tuya_read_run_days(&run_days);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_read_run_days err:%d", op_ret);
        return op_ret;
    }

    g_filter_msg.pm_total = pm_total;
    g_filter_pm_total = filter_pm_total;
    g_filter_use_days = filter_use_days;
    g_runtime_total = runtime_total;
    g_now_days = now_days;
    g_run_days = run_days;

    return op_ret;
}

/**
* @brief: tuya_write_save_data_in_flash
* @desc: write save data in flash
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_write_save_data_in_flash(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = tuya_write_pm_total(g_filter_msg.pm_total);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_write_pm_total err:%d", op_ret);
        return op_ret;
    }

    op_ret = tuya_write_filter_pm_total(g_filter_pm_total);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_write_filter_pm_total err:%d", op_ret);
        return op_ret;
    }

    op_ret = tuya_write_filter_use_days(g_filter_use_days);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_write_filter_use_days err:%d", op_ret);
        return op_ret;
    }

    op_ret = tuya_write_runtime_total(g_runtime_total);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_write_runtime_total err:%d", op_ret);
        return op_ret;
    }

    op_ret = tuya_write_now_days(g_now_days);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_write_now_days err:%d", op_ret);
        return op_ret;
    }

    op_ret = tuya_write_run_days(g_run_days);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_write_run_days err:%d", op_ret);
        return op_ret;
    }

    return op_ret;
}

/**
* @brief: tuya_filter_reset
* @desc: reset filter
* @param[in] filter_reset:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_filter_reset(IN CONST BOOL_T filter_reset)
{
    OPERATE_RET op_ret = OPRT_OK;

    if (FALSE == filter_reset) {
        return op_ret;
    }

    g_filter_pm_total = 0;
    g_filter_use_days = 0;
    g_now_days = 0;
    g_run_days = 0;

    //DP report
    tuya_filter_life_dp_updata();  //filter life
    tuya_filter_days_dp_updata();  //filter days

    //write filter_pm_total in flash
    op_ret = tuya_write_filter_pm_total(g_filter_pm_total);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_write_filter_pm_total err:%d", op_ret);
        return op_ret;
    }
    //write filter_use_days in flash
    op_ret = tuya_write_filter_use_days(g_filter_use_days);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_write_filter_use_days err:%d", op_ret);
        return op_ret;
    }
    //write now_days in flash
    op_ret = tuya_write_now_days(g_now_days);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_write_now_days err:%d", op_ret);
        return op_ret;
    }
    //write filter run_days in flash
    op_ret = tuya_write_run_days(g_run_days);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_write_run_days err:%d", op_ret);
        return op_ret;
    }

    return op_ret;
}

/**
* @brief: tuya_pm_total_dp_updata
* @desc: updata pm_total dp value
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_pm_total_dp_updata(VOID_T)
{
    tuya_update_single_dp(PM_TOTAL_DPID, PROP_VALUE, g_filter_msg.pm_total/1000);  //ug -> mg
    return OPRT_OK;
}

/**
* @brief: tuya_runtime_total_dp_updata
* @desc: updata runtime_total dp value
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_runtime_total_dp_updata(VOID_T)
{
    tuya_update_single_dp(RUNTIME_TOTAL_DPID, PROP_VALUE, g_runtime_total); 
    return OPRT_OK;
}

/**
* @brief: tuya_filter_life_dp_updata
* @desc: updata filter_life dp value
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_filter_life_dp_updata(VOID_T)
{
    //figure filter_life
    g_filter_msg.filter_life = (1 - (FLOAT_T)g_filter_pm_total / FILTER_MAX_PM_CAN_ABSORB) * 100;
    //DP report
    tuya_update_single_dp(FILTER_LIFE_DPID, PROP_VALUE, g_filter_msg.filter_life);

    return OPRT_OK;
}

/**
* @brief: tuya_filter_days_dp_updata
* @desc: updata filter_days dp value
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_filter_days_dp_updata(VOID_T)
{
    //figure filter_days
    if (g_filter_use_days < 7) {
        g_filter_msg.filter_days = FILTER_DAYS_DEFAULT;
    } else {
        g_filter_msg.filter_days = ((FILTER_MAX_PM_CAN_ABSORB * g_filter_use_days) / g_filter_pm_total) - g_filter_use_days;
    }

    //DP report
    if (g_filter_msg.filter_days > 1000) {
        g_filter_msg.filter_days = 1000;
    }
    tuya_update_single_dp(FILTER_DAYS_DPID, PROP_VALUE, g_filter_msg.filter_days);

    return OPRT_OK;
}

/**
* @brief: tuya_get_filter_life
* @desc: get filter life
* 
* @return filter life
*/
UINT_T tuya_get_filter_life(VOID_T)
{
    return g_filter_msg.filter_life;
}

/**
* @brief: tuya_filter_life_figure_task_init
* @desc: filter_life_figure task thread start
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_filter_life_figure_task_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    
    THRD_HANDLE ty_user_handle = NULL;
    THRD_PARAM_S thrd_param;
    thrd_param.priority = TRD_PRIO_4; 
    thrd_param.stackDepth = 515*4;
    thrd_param.thrdname = "ty_filter_life_figure_task";
    op_ret = CreateAndStart(&ty_user_handle, NULL, NULL, tuya_filter_1minute_task, NULL, &thrd_param);      
    if(op_ret != OPRT_OK) {
        PR_ERR("start tuya_filter_1minute_task ty_task_cb err");
        return op_ret;
    }
    
    PR_NOTICE("tuya_filter_life_figure_task_init success !!!");
    return op_ret;
}
