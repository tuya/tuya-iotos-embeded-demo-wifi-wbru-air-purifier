/**
 * @file tuya_iot_funtion.c
 * @author wangwei@tuya.com
 * @brief Tuya Infrared application module Demo
 * @version 1.0.0
 * @date 2021-05-26
 * @copyright Copyright (c) tuya.inc 2021
 */

#include "uni_log.h"
#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"
#include "ty_cJSON.h"
#include "tuya_device.h"
#include "tuya_gpio.h"
#include "tuya_iot_wifi_api.h"
#include "tuya_iot_funtion.h"

#include "gw_intf.h"
#include "tuya_dp_process.h"
//#include "tuya_led_funtion.h"

STATIC VOID ty_iot_network_status_cb(IN CONST GW_WIFI_NW_STAT_E gw_stat)
{
    PR_NOTICE("wifi status is :%d",gw_stat);
    OPERATE_RET op_ret = OPRT_OK;

    switch(gw_stat) {

        case STAT_UNPROVISION: {
            //op_ret = tuya_led_funtion_set_status(LED_FAST_BLINK_LIGHT);
            break;
        }

        case STAT_AP_STA_UNCFG: {
            //op_ret = tuya_led_funtion_set_status(LED_SLOW_BLINK_LIGHT);
            break;
        }

        case STAT_LOW_POWER:
        case STAT_AP_STA_DISC:
        case STAT_STA_DISC: {
            //op_ret = tuya_led_funtion_set_status(LED_OFF_LIGHT);
            break;
        }

        case STAT_AP_STA_CONN:
        case STAT_STA_CONN:
        case STAT_CLOUD_CONN:
        case STAT_AP_CLOUD_CONN:{
            //op_ret = tuya_led_funtion_set_status(LED_ON_LIGHT);
            break;
        }

        default:{
            //op_ret = tuya_led_funtion_set_status(LED_OFF_LIGHT);
            break;
        }
    }

    if(OPRT_OK != op_ret) {
        PR_ERR("tuya set led status is err:%d --- status:%d",op_ret,gw_stat);
    }
}

STATIC VOID ty_iot_status_changed_cb(IN CONST GW_STATUS_E status)
{
    PR_DEBUG("gw status changed to %d", status);

}

STATIC OPERATE_RET ty_iot_gw_ug_inform_cb(IN CONST FW_UG_S *fw)
{
  
    
}


STATIC VOID ty_iot_dev_obj_dp_cb(IN CONST TY_RECV_OBJ_DP_S *dp)
{
    PR_DEBUG("dp->cid:%s dp->dps_cnt:%d",dp->cid,dp->dps_cnt);
    UCHAR_T i = 0;

    for(i = 0; i < dp->dps_cnt; i++) {
        tuya_deal_dp_proc(&(dp->dps[i]));
        dev_report_dp_json_async(get_gw_cntl()->gw_if.id, dp->dps, dp->dps_cnt);
    }
}


STATIC VOID ty_iot_dev_raw_dp_cb(IN CONST TY_RECV_RAW_DP_S *dp)
{

}


STATIC VOID ty_iot_dev_dp_query_cb(IN CONST TY_DP_QUERY_S *dp_qry)
{
    PR_DEBUG("Recv DP Query Cmd");
}

STATIC VOID ty_iot_dev_ug_inform_cb(IN CONST CHAR_T *dev_id,IN CONST FW_UG_S *fw)
{
    PR_DEBUG("Rev GW Upgrade Info");
    PR_DEBUG("fw->fw_url:%s", fw->fw_url);
    PR_DEBUG("fw->fw_hmac:%s", fw->fw_hmac);
    PR_DEBUG("fw->sw_ver:%s", fw->sw_ver);
    PR_DEBUG("fw->file_size:%d", fw->file_size);
}



/** 
 * @brief: tuya IOT initialization
 * @desc: LED lamp control interface
 * @param[in] cfg_mode:Initialization mode
 * @param[in] p_product_id:Product ID pointer
 * @return OPERATE_RET  OPRT_OK is sucess, other is fail
 * @note none
 */
OPERATE_RET ty_iot_funtion_start_init(IN CONST GW_WF_CFG_MTHD_SEL cfg_mode,IN CONST CHAR_T *p_product_id)
{
    OPERATE_RET op_ret = OPRT_OK;
 
    TY_IOT_CBS_S wf_cbs = {
        ty_iot_status_changed_cb,\
        ty_iot_gw_ug_inform_cb,\
        NULL,         \
        ty_iot_dev_obj_dp_cb,\
        ty_iot_dev_raw_dp_cb,\
        ty_iot_dev_dp_query_cb,\
        ty_iot_dev_ug_inform_cb,\
        NULL,\
        NULL,
    };

    op_ret = tuya_iot_wf_soc_dev_init_param(cfg_mode, WF_START_SMART_FIRST, &wf_cbs,NULL, p_product_id, USER_SW_VER);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_wf_soc_dev_init_param err:%d",op_ret);
        return op_ret;
    }

    op_ret = tuya_iot_reg_get_wf_nw_stat_cb(ty_iot_network_status_cb);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_reg_get_wf_nw_stat_cb err:%d",op_ret);
        return op_ret;
    }
    
    PR_NOTICE("ty_iot_funtion_start_init sucess !!!");
    return op_ret;
}