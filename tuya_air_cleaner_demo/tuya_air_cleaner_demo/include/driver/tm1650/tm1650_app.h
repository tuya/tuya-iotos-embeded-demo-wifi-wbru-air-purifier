/***********************************************************
*  File: tm1650_app.h
*  Author: fengzq
*  Date: 20210502
*  Note: 
***********************************************************/

#ifndef __TM1650_APP_H
#define __TM1650_APP_H

#include "tm1650.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/***********************************************************
*  Function: tuya_tm1650_app_read
*  Input: data
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_app_read(OUT UCHAR_T *data);

/***********************************************************
*  Function: tuya_tm1650_app_write
*  Input: address data
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_app_write(IN UCHAR_T address,IN UCHAR_T data);

/***********************************************************
*  Function: tuya_tm1650_app_init
*  Input: scl_io sda_io
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_app_init(UCHAR_T scl_io,UCHAR_T sda_io);

/**
* @brief: tuya_tm1650_app_light_enable
* @desc:  add by ali.zhang@tuya.com
* @param[in] on:set light enable on or off
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_tm1650_app_light_enable(IN CONST BOOL_T on);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __tm1650_H__ */




