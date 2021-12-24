/**
* @file tuya_filter_funtion.h
* @author ali.zhang@tuya.com
* @brief tuya_filter_funtion module is used to filter funtion
* @version 1.0.0
* @date 2021-10-14
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
#ifndef __TUYA_FILTER_FUNTION_H__
#define __TUYA_FILTER_FUNTION_H__
 
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
typedef BYTE_T FILTER_TYPE_E;
#define FILTER_STANDARD           0x00  //standard
#define FILTER_ANTIBACTERIAL      0x01  //antibacterial
#define FILTER_ALDEHYDE_REMOVAL   0x02  //aldehyde removal
#define FILTER_PROFESSIONAL       0x03  //professional

typedef struct {       
    UCHAR_T year;         //0x00 means year 2000
    UCHAR_T month;        //0x01~0x0C, 1~12 month
    UCHAR_T day;          //0x01~0x1F, 1~31 day
}FILTER_PRODUCT_DATE_T;

typedef struct {       
    FILTER_TYPE_E          type;
    UINT_T                 ID;
    FILTER_PRODUCT_DATE_T  product_date;
}FILTER_CARD_MSG_T;

typedef struct {
    UINT_T filter_life;        //0~100, %
    UINT_T filter_days;        //0~1000, day
    UINT_T pm_total;           //0~100000000, ug
}TY_FILTER_MSG_T;
 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
#define    FILTER_TASK_DELAY_TIME                 60000  //1min，60000, alitest
#define    FILTER_DAYS_DEFAULT                    180    //filter default can use 180 days
  
/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_read_save_data_from_flash
* @desc: read save data from flash, then set value
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_read_save_data_from_flash(VOID_T);

/**
* @brief: tuya_write_save_data_in_flash
* @desc: write save data in flash
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_write_save_data_in_flash(VOID_T);

/**
* @brief: tuya_filter_reset
* @desc: reset filter
* @param[in] filter_reset:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_filter_reset(IN CONST BOOL_T filter_reset);

/**
* @brief: tuya_pm_total_dp_updata
* @desc: updata pm_total dp value
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_pm_total_dp_updata(VOID_T);

/**
* @brief: tuya_runtime_total_dp_updata
* @desc: updata runtime_total dp value
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_runtime_total_dp_updata(VOID_T);

/**
* @brief: tuya_filter_life_dp_updata
* @desc: updata filter_life dp value
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_filter_life_dp_updata(VOID_T);

/**
* @brief: tuya_filter_days_dp_updata
* @desc: updata filter_days dp value
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_filter_days_dp_updata(VOID_T);

/**
* @brief: tuya_get_filter_life
* @desc: get filter life
* 
* @return filter life
*/
UINT_T tuya_get_filter_life(VOID_T);

/**
* @brief: tuya_filter_life_figure_task_init
* @desc: filter_life_figure task thread start
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_filter_life_figure_task_init(VOID_T);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_FILTER_FUNTION_H__*/