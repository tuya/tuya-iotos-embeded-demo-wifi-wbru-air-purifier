/***********************************************************
*  File: tm1650_app.c
*  Author: fengzq
*  Date: 20210502
*  Note: 
***********************************************************/
#include "tm1650_app.h"
#include "uni_log.h"
#include "uni_thread.h"
#include "tuya_hal_system.h"
#include <string.h>
#include "tuya_hal_mutex.h"

#define TM1650_DIG1 0x68
#define TM1650_DIG2 0x6A
#define TM1650_DIG3 0x6C
#define TM1650_DIG4 0x6E

STATIC UCHAR_T dig1 = 0,dig2 = 0,dig3 = 0,dig4 = 0;
STATIC MUTEX_HANDLE g_tm1650_app_mutex;

/***********************************************************
*  Function: tuya_tm1650_app_read
*  Input: data
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_app_read(OUT UCHAR_T *data)
{
    OPERATE_RET op_ret;
    tuya_hal_mutex_lock(g_tm1650_app_mutex);
    op_ret = tuya_tm1650_read(data);
    tuya_hal_mutex_unlock(g_tm1650_app_mutex);
    return op_ret;
}

/***********************************************************
*  Function: tuya_tm1650_app_write
*  Input: address data
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_app_write(IN UCHAR_T address, IN UCHAR_T data)
{
    OPERATE_RET op_ret = OPRT_OK;
    switch (address) {
    case TM1650_DIG1:
        if (dig1 == data) {
            return op_ret;
        } else {
            dig1 = data;
        }
        break;
    case TM1650_DIG2:
        if (dig2 == data) {
            return op_ret;
        } else {
            dig2 = data;
        }
        break;
    case TM1650_DIG3:
        if (dig3 == data) {
            return op_ret;
        } else {
            dig3 = data;
        }
        break;
    case TM1650_DIG4:
        if (dig4 == data) {
            return op_ret;
        } else {
            dig4 = data;
        }
        break;
    default:
        return OPRT_INVALID_PARM;
    }
    tuya_hal_mutex_lock(g_tm1650_app_mutex);
        op_ret = tuya_tm1650_write_once(address, data);
    tuya_hal_mutex_unlock(g_tm1650_app_mutex);
    return op_ret;
}

/***********************************************************
*  Function: tuya_tm1650_app_init
*  Input: scl_io sda_io
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_app_init(UCHAR_T scl_io,UCHAR_T sda_io)
{
    OPERATE_RET op_ret;
    TM1650_INIT_T tm1650_init;
	op_ret = tuya_hal_mutex_create_init(&g_tm1650_app_mutex);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_hal_mutex_create_init :%d", op_ret);
        return op_ret;
    }
    tm1650_init.ucSCL_IO = scl_io;
    tm1650_init.ucSDA_IO = sda_io;
    op_ret = tuya_tm1650_init(&tm1650_init);
    if(OPRT_OK != op_ret) {
        PR_ERR("tm1650_init err:%d",op_ret);
        return op_ret;
    }
    return op_ret;
}

/**
* @brief: tuya_tm1650_app_light_enable
* @desc:  add by ali.zhang@tuya.com
* @param[in] on:set light enable on or off
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_tm1650_app_light_enable(IN CONST BOOL_T on)
{
    OPERATE_RET op_ret;

    op_ret = tuya_tm1650_light_enable(on);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_tm1650_light_enable err:%d", op_ret);
        return op_ret;
    }
    return op_ret;
}
