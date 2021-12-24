/**
* @file tuya_dp_process.h
* @author ali.zhang@tuya.com
* @brief tuya_dp_process module is used to handle cloud interaction commands
* @version 1.0.0
* @date 2021-09-10
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
#ifndef __TUYA_DP_PROCESS_H__
#define __TUYA_DP_PROCESS_H__
 
#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*****************************************************************
****************************micro define**************************
*****************************************************************/
/* DPID */
#define    SWITCH_DPID                                  1
#define    PM25_DPID                                    2
#define    MODE_DPID                                    3
#define    FAN_SPEED_ENUM_DPID                          4
#define    FILTER_LIFE_DPID                             5
#define    CHILD_LOCK_DPID                              7
#define    LIGHT_DPID                                   8
#define    UV_DPID                                      9
#define    FILTER_RESET_DPID                            11
#define    TEMP_INDOOR_DPID                             12
#define    HUMIDITY_DPID                                13
#define    TVOC_DPID                                    14
#define    FILTER_DAYS_DPID                             16
#define    RUNTIME_TOTAL_DPID                           17
#define    COUNTDOWN_SET_DPID                           18
#define    COUNTDOWN_LEFT_DPID                          19
#define    PM_TOTAL_DPID                                20
#define    FAULT_DPID                                   22
#define    TEMP_UNIT_CONVERT                            23
#define    PM10_DPID                                    101
#define    FILTER_TYPE_DPID                             102
#define    SOUND_SWITCH_DPID                            103

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
VOID_T tuya_update_single_dp(IN BYTE_T dpid, IN DP_PROP_TP_E type, IN INT_T value);

/**
* @brief: tuya_update_dp_initial_value
* @desc: update dp initial value
*
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_update_dp_initial_value(VOID_T);

/**
* @function: tuya_deal_dp_proc
* @brief: DP send down processing
* @param[in]: root -> DP dpid、type、value、 time_stamp
* @return: none
*/
VOID_T tuya_deal_dp_proc(IN CONST TY_OBJ_DP_S *root);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_DP_PROCESS_H__*/