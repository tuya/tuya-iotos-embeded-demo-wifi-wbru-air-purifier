/***********************************************************
*  File: tm1650.h
*  Author: fengzq
*  Date: 20210501
*  Note: 
***********************************************************/

#ifndef __TM1650_H
#define __TM1650_H


#include "soc_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


#define TM1650_WRITA_COMMAND1       0x48
#define TM1650_WRITA_COMMAND2_OPEN  0x01
#define TM1650_WRITA_COMMAND2_CLOSE 0x00
#define TM1650_READ_COMMAND         0x49

/******************************************
 * command2:
______________________________________________________________________
|_B7_|_B6_|_B5_|_B4_|_B3_|_B2_|_B1_|_B0_|______功能______|_____说明____|
|_X__|_0__|_0__|_0__|____|_X__|_X__|____|                |___8级亮度___|
|_X__|_0__|_0__|_1__|____|_X__|_X__|____|                |___1级亮度___|
|_X__|_0__|_1__|_0__|____|_X__|_X__|____|                |___2级亮度___|
|_X__|_0__|_1__|_1__|____|_X__|_X__|____|                |___3级亮度___|
|_X__|_1__|_0__|_0__|____|_X__|_X__|____|    辉度设置     |___4级亮度___|
|_X__|_1__|_0__|_1__|____|_X__|_X__|____|                |___5级亮度___|
|_X__|_1__|_1__|_0__|____|_X__|_X__|____|                |___6级亮度___|
|_X__|_1__|_1__|_1__|____|_X__|_X__|____|________________|___7级亮度___|
|_X__|____|____|____|_0__|_X__|_X__|____| 7/8段显示控制位 |_8段显示方式_|
|_X__|____|____|____|_1__|_X__|_X__|____|________________|_7段显示方式_|
|_X__|____|____|____|____|_X__|_X__|_0__| 开启/关闭显示位 |____关显示___|
|_X__|____|____|____|____|_X__|_X__|_1__|________________|____开显示___|
******************************************/


typedef struct 
{
    UCHAR_T ucSDA_IO;            //< SDA pin
    UCHAR_T ucSCL_IO;            //< SCL pin
}TM1650_INIT_T;


/***********************************************************
*  Function: tuya_tm1650_init
*  Input: tm1650_init
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_init(IN TM1650_INIT_T *tm1650_init);

/***********************************************************
*  Function: tuya_tm1650_write_raw
*  Input: cmd2
*  Input: address1,address2
*  Input: data1,data2
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_write_raw(UCHAR_T cmd2,UCHAR_T address1,UCHAR_T data1,UCHAR_T address2,UCHAR_T data2);

/***********************************************************
*  Function: tuya_tm1650_write
*  Input: address1,address2
*  Input: data1,data2
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_write(UCHAR_T address1,UCHAR_T data1,UCHAR_T address2,UCHAR_T data2);

/***********************************************************
*  Function: tuya_tm1650_write_once_raw
*  Input: cmd2
*  Input: address
*  Input: data
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_write_once_raw(UCHAR_T cmd2,UCHAR_T address,UCHAR_T data);

/***********************************************************
*  Function: tuya_tm1650_write_once
*  Input: address
*  Input: data
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_write_once(UCHAR_T address,UCHAR_T data);

/***********************************************************
*  Function: tuya_tm1650_read
*  Input: data
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_read(UCHAR_T *data);

/**
* @brief: tuya_tm1650_light_enable
* @desc:  add by ali.zhang@tuya.com
* @param[in] on:set light enable on or off
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_tm1650_light_enable(IN CONST BOOL_T on);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __BP1658CJ_H__ */




















































