/**
* @file tuya_air_quality_funtion.h
* @author ali.zhang@tuya.com
* @brief tuya_air_quality_funtion module is used to air quality funtion
* @version 1.0.0
* @date 2021-09-17
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
#ifndef __TUYA_AIR_QUALITY_FUNTION_H__
#define __TUYA_AIR_QUALITY_FUNTION_H__
 
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
//air quality type show on lcd
typedef BYTE_T  LCD_SHOW_TYPE_E;
#define  SHOW_PM25           0
#define  SHOW_PM10           1
#define  SHOW_TVOC           2
#define  SHOW_TEMPERATURE    3
#define  SHOW_HUMIDITY       4
#define  SHOW_NULL           5

#define  PM25_MAX            (699)
#define  PM10_MAX            (999)
#define  TVOC_MAX            (999)
#define  HUMIDITY_MAX        (100)
#define  TEMPERATURE_MAX     (50)
#define  TEMPERATURE_MIN     (-20)


typedef struct {
    UINT_T pm25;        //0~699, ug/m3
    UINT_T pm10;        //0~999, ug/m3
    UINT_T tvoc;        //0~999, ug/m3
    INT_T  temperature; //-20~50, ℃
    UINT_T humidity;    //0~100, %
} AIR_QUALITY_INDEX_T;


/*********************************************************************
****************************variable define***************************
*********************************************************************/
#define    AIR_QUALITY_SEN_INIT_TIMER            8000   //8s
#define    GET_AIR_QUALITY_TIMER                 4000   //4s, alitest
 
/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_uart0_init
* @desc: uart0 initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_uart0_init(VOID_T);

/**
* @brief: tuya_switch_show_air_quality_type
* @desc: switch air quality type show on lcd
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET  tuya_switch_show_air_quality_type(VOID_T);

/**
* @brief: tuya_get_air_quality_task_init
* @desc: get air quality task thread start
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_get_air_quality_task_init(VOID_T);

/**
* @brief: tuya_air_quality_dp_updata
* @desc: updata air quality dp value
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_air_quality_dp_updata(VOID_T);

/**
* @brief: tuya_light_switch_set
* @desc: set light switch
* @param[in] light_switch:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_light_switch_set(IN CONST BOOL_T light_switch);
 
 
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_AIR_QUALITY_FUNTION_H__*/