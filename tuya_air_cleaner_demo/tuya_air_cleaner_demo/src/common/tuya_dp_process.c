/**
* @file tuya_dp_process.c
* @author ali.zhang@tuya.com
* @brief tuya_dp_process module is used to handle cloud interaction commands
* @version 1.0.0
* @date 2021-09-10
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
/*****************************************************************
******************************includes****************************
*****************************************************************/
#include "tuya_dp_process.h"
#include "uni_log.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_cloud_wifi_defs.h"
#include "tuya_iot_com_api.h"
#include "tuya_hardware_driver.h"
#include "tuya_motor_driver.h"
#include "tuya_key_funtion.h"
#include "tuya_air_quality_funtion.h"
#include "tuya_mode_funtion.h"
#include "tuya_automatic_mode_funtion.h"
#include "tuya_countdown_funtion.h"
#include "tuya_filter_funtion.h"

/*****************************************************************
****************************micro define**************************
*****************************************************************/
 
 
/*****************************************************************
**************************typedef define**************************
*****************************************************************/
 
 
/*****************************************************************
**************************variable define*************************
*****************************************************************/


/*****************************************************************
**************************function define*************************
*****************************************************************/
/**
* @brief: tuya_update_single_dp
* @desc: single DP report, only for type of value,enum,bool
* @param[in] dpid:
* @param[in] type: The type of dpid
* @param[in] value: The value of dpid
* @return none
*/
VOID_T tuya_update_single_dp(IN BYTE_T dpid, IN DP_PROP_TP_E type, IN INT_T value)
{
    OPERATE_RET op_ret = OPRT_OK;

    INT_T dp_cnt = 1; //update DP number

    /* exit without connecting to the router */
    GW_WIFI_NW_STAT_E wifi_state = STAT_LOW_POWER;
    get_wf_gw_nw_status(&wifi_state);
    if (wifi_state <= STAT_AP_STA_DISC || wifi_state == STAT_STA_DISC) {
        return OPRT_COM_ERROR;
    }

    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)Malloc(dp_cnt*SIZEOF(TY_OBJ_DP_S));
    if (NULL == dp_arr) {
        PR_ERR("malloc failed");
        return OPRT_COM_ERROR;
    }

    /* initialize requested memory space */
    memset(dp_arr, 0, dp_cnt*SIZEOF(TY_OBJ_DP_S));

    dp_arr[0].dpid = dpid;
    dp_arr[0].type = type;
    dp_arr[0].time_stamp = 0;
    switch (type) {
    case PROP_BOOL:
        dp_arr[0].value.dp_bool = value;
        break;
    case PROP_VALUE:
        dp_arr[0].value.dp_value = value;
        break;
    case PROP_ENUM:
        dp_arr[0].value.dp_enum = value;
        break;
    case PROP_BITMAP:
        dp_arr[0].value.dp_bitmap = value;
        break;
    default:
        break;
    }

    /* report DP */
    op_ret = dev_report_dp_json_async(NULL ,dp_arr, dp_cnt);

    /* free requested memory space */
    Free(dp_arr);
    dp_arr = NULL;

    if (OPRT_OK != op_ret) {
        PR_ERR("dev_report_dp_json_async relay_config data error:%d", op_ret);
    } 
    
    //PR_NOTICE("DPID:%d, value:%d, report suc!", dpid, value);
}

/**
* @brief: tuya_update_dp_initial_value
* @desc: update dp initial value
*
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_update_dp_initial_value(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    //power switch
    PR_DEBUG("############### switch");
    tuya_update_single_dp(SWITCH_DPID, PROP_BOOL, POWER_OFF);
    //working mode
    PR_DEBUG("############### working_mode");
    tuya_update_single_dp(MODE_DPID, PROP_ENUM, MODE_MANUAL);
    //air quality index
    PR_DEBUG("############### air_quality_index");
    tuya_air_quality_dp_updata();
    //fan speed enum
    PR_DEBUG("############### fan_speed_enum");
    tuya_update_single_dp(FAN_SPEED_ENUM_DPID, PROP_ENUM, SPEED_SLEEP);
    //child lock
    PR_DEBUG("############### child_lock");
    tuya_update_single_dp(CHILD_LOCK_DPID, PROP_BOOL, FALSE);
    //light switch
    PR_DEBUG("############### light_switch");
    tuya_update_single_dp(LIGHT_DPID, PROP_BOOL, FALSE);
    //uv
    PR_DEBUG("############### uv");
    tuya_update_single_dp(UV_DPID, PROP_BOOL, FALSE);
    //countdown set
    PR_DEBUG("############### countdown_set");
    tuya_update_single_dp(COUNTDOWN_SET_DPID, PROP_ENUM, 0);
    //countdown left
    PR_DEBUG("############### countdown_left");
    tuya_update_single_dp(COUNTDOWN_LEFT_DPID, PROP_VALUE, 0);
    //temp unit convert
    PR_DEBUG("############### temp_unit_convert");
    tuya_update_single_dp(TEMP_UNIT_CONVERT, PROP_ENUM, 0);
    //sound switch
    PR_DEBUG("############### sound_switch");
    tuya_update_single_dp(SOUND_SWITCH_DPID, PROP_BOOL, FALSE);
    //filter reset
    tuya_update_single_dp(FILTER_RESET_DPID, PROP_BOOL, FALSE);
    //filter type
    PR_DEBUG("############### filter_type");
    tuya_filter_type_dp_updata();
    //fault
    PR_DEBUG("############### fault");
    tuya_update_single_dp(FAULT_DPID, PROP_BITMAP, 0x00);
    //pm total
    PR_DEBUG("############### pm_total");
    tuya_pm_total_dp_updata();
    //runtime total
    PR_DEBUG("############### runtime_total");
    tuya_runtime_total_dp_updata();
    //filter life
    PR_DEBUG("############### filter_life");
    tuya_filter_life_dp_updata();
    //filter days
    PR_DEBUG("############### filter_days");
    tuya_filter_days_dp_updata();

    return op_ret;
}

/**
* @function: tuya_deal_dp_proc
* @brief: DP send down processing
* @param[in]: root -> DP dpid、type、value、time_stamp
* @return: none
*/
VOID_T tuya_deal_dp_proc(IN CONST TY_OBJ_DP_S *root)
{
    UCHAR_T dpid;

    dpid = root->dpid;
    PR_NOTICE("dpid:%d", dpid);
    switch (dpid) {
    case SWITCH_DPID:
        tuya_update_single_dp(SWITCH_DPID, PROP_BOOL, root->value.dp_bool);
        tuya_power_set(root->value.dp_bool);
        break;
    case MODE_DPID:
        tuya_update_single_dp(MODE_DPID, PROP_ENUM, root->value.dp_enum);
        tuya_work_mode_set(root->value.dp_enum);
        break;
    case FAN_SPEED_ENUM_DPID:
        tuya_update_single_dp(FAN_SPEED_ENUM_DPID, PROP_ENUM, root->value.dp_enum);
        tuya_fan_speed_dp_handle(root->value.dp_enum);
        break;
    case CHILD_LOCK_DPID:
        tuya_update_single_dp(CHILD_LOCK_DPID, PROP_BOOL, root->value.dp_bool);
        tuya_set_child_lock_value(root->value.dp_bool);
        break;
    case LIGHT_DPID:
        //tuya_update_single_dp(LIGHT_DPID, PROP_BOOL, root->value.dp_bool);
        tuya_light_switch_set(root->value.dp_bool);
        break;
    case UV_DPID:
        tuya_update_single_dp(UV_DPID, PROP_BOOL, root->value.dp_bool);
        tuya_set_uv_switch_value(root->value.dp_bool);
        break;
    case FILTER_RESET_DPID:
        tuya_update_single_dp(FILTER_RESET_DPID, PROP_BOOL, root->value.dp_bool);
        tuya_filter_reset(root->value.dp_bool);
        break;
    case COUNTDOWN_SET_DPID:
        tuya_update_single_dp(COUNTDOWN_SET_DPID, PROP_ENUM, root->value.dp_enum);
        tuya_countdown_timer_start(root->value.dp_enum);
        break;
    case SOUND_SWITCH_DPID:
        tuya_update_single_dp(SOUND_SWITCH_DPID, PROP_BOOL, root->value.dp_bool);
        tuya_set_key_sound_value(root->value.dp_bool);
        break;
    case TEMP_UNIT_CONVERT:
        tuya_update_single_dp(TEMP_UNIT_CONVERT, PROP_ENUM, root->value.dp_enum);
        break;
    default :
        break;
    }
}
