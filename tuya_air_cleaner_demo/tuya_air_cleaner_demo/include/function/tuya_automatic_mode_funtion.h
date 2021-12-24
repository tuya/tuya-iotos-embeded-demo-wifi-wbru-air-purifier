/**
* @file tuya_automatic_mode_funtion.h
* @author ali.zhang@tuya.com
* @brief tuya_automatic_mode_funtion module is used to automatic mode funtion
* @version 1.0.0
* @date 2021-10-09
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
#ifndef __TUYA_AUTOMATIC_MODE_FUNTION_H__
#define __TUYA_AUTOMATIC_MODE_FUNTION_H__
 
#include "tuya_cloud_types.h"
#include "tuya_air_quality_funtion.h"
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*********************************************************************
******************************micro define****************************
*********************************************************************/
 
 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/
typedef BYTE_T  PM25_VALUE_RANGE_E;
#define PM25_RANGE_115          0  //pm2.5: 115+
#define PM25_RANGE_75_115       1  //pm2.5: 75~115
#define PM25_RANGE_35_75        2  //pm2.5: 35~75
#define PM25_RANGE_35           3  //pm2.5: 35-
 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
#define    PM25_REDUCTION_COMP                          1 // 1ug/m3
 
/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_set_air_quality_index
* @desc: set air quality index
* @param[in] air_quality_index
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_set_air_quality_index(IN AIR_QUALITY_INDEX_T air_quality_index);

/**
* @brief: tuya_pm25_scan
* @desc: scan pm2.5 value once
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_pm25_scan(VOID_T);

/**
* @brief: tuya_auto_fan_speed_start
* @desc: start auto fan speed
* @param[in] start:
*             TRUE: start
*             FLASE: stop
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_auto_fan_speed_start(IN CONST BOOL_T start);
 
/**
* @brief: tuya_uv_funtion_scan
* @desc: scan uv funtion 
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_uv_funtion_scan(VOID_T);

/**
* @brief: tuya_dark_scan
* @desc: scan dark funtion 
* @param[in] dark_flag
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_dark_scan(IN CONST BOOL_T dark_flag);

/**
* @brief: tuya_dark_scan_start
* @desc: start scan dark cycle 10s
* @param[in] start:
*             TRUE: start
*             FLASE: stop
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_dark_scan_start(IN CONST BOOL_T start);

/**
* @brief: tuya_set_uv_switch_value
* @desc: set uv_switch value
* @param[in] uv_switch
* @return OPERATE_RET
*/
OPERATE_RET tuya_set_uv_switch_value(IN CONST BOOL_T uv_switch);

/**
* @brief: tuya_set_uv_status
* @desc: set uv status
* @param[in] uv_status
* @return OPERATE_RET
*/
OPERATE_RET tuya_set_uv_status(IN CONST BOOL_T uv_status);

/**
* @brief: tuya_get_uv_status
* @desc: get uv status
* @param[in] uv_status
* @return OPERATE_RET
*/
BOOL_T tuya_get_uv_status(VOID_T);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_AUTOMATIC_MODE_FUNTION_H__*/