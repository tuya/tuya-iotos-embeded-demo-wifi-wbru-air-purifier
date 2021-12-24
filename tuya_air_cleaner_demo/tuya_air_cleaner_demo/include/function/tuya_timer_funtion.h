/**
* @file tuya_timer_funtion.h
* @author ali.zhang@tuya.com
* @brief tuya_timer_funtion module is used to 
* @version 1.0.0
* @date 2021-09-14
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
#ifndef __TUYA_TIMER_FUNTION_H__
#define __TUYA_TIMER_FUNTION_H__
 
#include "tuya_cloud_types.h"
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*********************************************************************
******************************micro define****************************
*********************************************************************/
 
 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/

 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
 
 
/*********************************************************************
****************************function define***************************
*********************************************************************/
// /**
// * @brief: tuya_buz_timer1_start
// * @desc: start buz timer1, count 100ms
// * 
// * @return OPERATE_RET  OPRT_OK is sucess, other is fail
// */
// OPERATE_RET tuya_buz_timer1_start(VOID_T);

// /**
// * @brief: tuya_buz_timer2_start
// * @desc: start buz timer2, cycle count 100ms
// * 
// * @return OPERATE_RET  OPRT_OK is sucess, other is fail
// */
// OPERATE_RET tuya_buz_timer2_start(VOID_T);

/**
* @brief: tuya_wf_status_timeout_start
* @desc: start wifi status timeout, cycle count 2000ms
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_wf_status_timeout_start(VOID_T);

/**
* @brief: tuya_pm25_scan_timer_start
* @desc: start pm25 scan, cycle count 5min
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_pm25_scan_timer_start(VOID_T);

/**
* @brief: tuya_pm25_scan_timer_stop
* @desc: tuya_pm25_scan_timer_stop
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_pm25_scan_timer_stop(VOID_T);

/**
* @brief: tuya_dark_scan_timer_start
* @desc: start dark scan, cycle count 2s
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_dark_scan_timer_start(VOID_T);

/**
* @brief: tuya_dark_scan_timer_stop
* @desc: tuya_dark_scan_timer_stop
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_dark_scan_timer_stop(VOID_T);

/**
* @brief: tuya_uv_close_timer_start
* @desc: start uv close timer, count 60min
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_uv_close_timer_start(VOID_T);
 
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_TIMER_FUNTION_H__*/