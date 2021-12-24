/***********************************************************
*  File: tm1650.c
*  Author: fengzq
*  Date: 20210501
*  Note: 
***********************************************************/

#include "tm1650.h"
#include "uni_log.h"
#include "tuya_hal_mutex.h"


STATIC MUTEX_HANDLE g_tm1650_mutex;

/**
 * @brief: tm1650 IIC delay proc
 * @param {none} 
 * @retval: none
 */
STATIC VOID tm1650_iic_delay(USHORT_T DelayTime)
{
    vSocI2CDelay(DelayTime);
}

/**
 * @brief: tm1650 IIC start transport
 * @param {SDA_IO -> SDA pin}
 * @param {SCL_IO -> SCL pin}
 * @retval: operate result
 */
STATIC OPERATE_RET tm1650_iic_start(VOID)
{
    vSocI2CSclSet();
    vSocI2CSdaSet();
    tm1650_iic_delay(10);
    vSocI2CSdaReset();
    tm1650_iic_delay(10);
    return OPRT_OK;
}

/**
 * @brief: tm1650 IIC stop transport
 * @retval: none
 */
STATIC VOID tm1650_iic_stop(VOID)
{
    vSocI2CSdaReset();
    vSocI2CSclReset();
    tm1650_iic_delay(10);
    vSocI2CSclSet();
    tm1650_iic_delay(10);
    vSocI2CSdaSet();
    tm1650_iic_delay(10);
}

/**
 * @brief: tm1650 IIC wait ACK
 * @retval: OPERATE_LIGHT
 */
STATIC OPERATE_RET tm1650_iic_wait_ack(VOID)
{
    vSocI2CSclReset();
    tm1650_iic_delay(10);
    vSocI2CSclSet();
    tm1650_iic_delay(10);
    if(ucSocI2CSdaInputRead()) {     /* receive ACK NG */     /* as SDA maybe send high or send low */
        return OPRT_COM_ERROR;
    }
    tm1650_iic_delay(10);
    return OPRT_OK;
}

/**
 * @brief: tm1650 IIC send byte
 * @param {SendByte -> send one byte}
 * @retval: none
 */
STATIC VOID tm1650_iic_send_one_byte(UCHAR_T SendByte)
{
    UCHAR_T i = 0;
    for(i = 0; i < 8; i ++)
    {
        vSocI2CSclReset();
        tm1650_iic_delay(10);
        if(SendByte & 0x80) {
            vSocI2CSdaSet();
        } else {
            vSocI2CSdaReset();
        }
        vSocI2CSclSet();
        tm1650_iic_delay(20);
        SendByte <<= 1;
   }
}

/**
 * @brief: tm1650 IIC send byte
 * @param {SendByte -> send one byte}
 * @retval: none
 */
VOID tm1650_iic_ack(VOID)
{
    vSocI2CSdaSet();
    vSocI2CSclSet();
    tm1650_iic_delay(10);
    vSocI2CSclReset();
    tm1650_iic_delay(10);
}

/**
 * @brief: tm1650 IIC send byte
 * @param {SendByte -> send one byte}
 * @retval: none
 */
UCHAR_T tm1650_iic_recv_byte(VOID)
{
    UCHAR_T bit,dat=0;
    for(bit=0;bit<8;bit++){
        dat<<=1;
        vSocI2CSclSet();    
        tm1650_iic_delay(10);
        dat |= ucSocI2CSdaInputRead();
        vSocI2CSclReset(); 
        tm1650_iic_delay(10);
    }
    return dat;
}

/**
 * @brief: tm1650 write data proc
 * @param {*pBuffer -> write data buf}
 * @param {NumByteToWrite -> write data len} 
 * @retval: OPERATE_LIGHT 
 */
STATIC OPERATE_RET tm1650_iic_write_page(UCHAR_T *pBuffer, USHORT_T NumByteToWrite)
{
    OPERATE_RET ret = OPRT_OK;
    if ( NULL == pBuffer ) {
        PR_ERR("SM2315 write data is invalid!");
        return OPRT_INVALID_PARM;
    }
    tuya_hal_mutex_lock(g_tm1650_mutex);
        ret = tm1650_iic_start();     /* start transport */
        if( OPRT_OK != ret ) {
            PR_ERR("IIC is busy!");
            return OPRT_COM_ERROR;
        }
        vSocI2CSDA_IN();
        while (NumByteToWrite --) {
            tm1650_iic_send_one_byte(*pBuffer++);
            ret = tm1650_iic_wait_ack();
            if( OPRT_OK != ret ) {
                PR_ERR("tm1650_iic_wait_ack!");
                return OPRT_COM_ERROR;
            }
        }
        vSocI2CSDA_OUT();
        tm1650_iic_stop();
        tm1650_iic_delay(10);
    tuya_hal_mutex_unlock(g_tm1650_mutex);
    return OPRT_OK;
}

/***********************************************************
*  Function: tuya_tm1650_init
*  Input: tm1650_init
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_init(IN TM1650_INIT_T *tm1650_init)
{
    OPERATE_RET op_ret;
    I2C_PIN_T i2c_config = {
        .ucSdaPin = tm1650_init ->ucSDA_IO,
        .ucSclPin = tm1650_init ->ucSCL_IO,
    };
    op_ret = opSocI2CInit(&i2c_config);          /* SDA&SCL GPIO INIT */
    if(op_ret != OPRT_OK){
        PR_ERR("opSocI2CInit error!");
        return OPRT_INVALID_PARM;
    }
    tuya_tm1650_write_raw(TM1650_WRITA_COMMAND2_CLOSE,0x68,0,0x6A,0);
    tuya_tm1650_write_raw(TM1650_WRITA_COMMAND2_CLOSE,0x6C,0,0x6E,0);

    op_ret = tuya_hal_mutex_create_init(&g_tm1650_mutex);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_hal_mutex_create_init :%d", op_ret);
        return op_ret;
    }

    return op_ret;
}

/***********************************************************
*  Function: tuya_tm1650_write_raw
*  Input: cmd2
*  Input: address
*  Input: data
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_write_raw(UCHAR_T cmd2,UCHAR_T address1,UCHAR_T data1,UCHAR_T address2,UCHAR_T data2)
{
    OPERATE_RET op_ret;
    UCHAR_T buf[2];
    buf[0] = TM1650_WRITA_COMMAND1;
    buf[1] = cmd2;
    op_ret = tm1650_iic_write_page(buf,2);
    if(op_ret != OPRT_OK){
        PR_ERR("TM1650_WRITA_COMMAND1 error!");
        return OPRT_COM_ERROR;
    }
    buf[0] = address1;
    buf[1] = data1;
    op_ret = tm1650_iic_write_page(buf,2);
    if(op_ret != OPRT_OK){
        PR_ERR("TM1650_WRITA address1 error!");
        return OPRT_COM_ERROR;
    }
    buf[0] = address2;
    buf[1] = data2;
    op_ret = tm1650_iic_write_page(buf,2);
    if(op_ret != OPRT_OK){
        PR_ERR("TM1650_WRITA address2 error!");
        return OPRT_COM_ERROR;
    }
    return op_ret;
}

/***********************************************************
*  Function: tuya_tm1650_write
*  Input: address
*  Input: data
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_write(UCHAR_T address1,UCHAR_T data1,UCHAR_T address2,UCHAR_T data2)
{
    return tuya_tm1650_write_raw(TM1650_WRITA_COMMAND2_OPEN,address1,data1,address2,data2);
}

/***********************************************************
*  Function: tuya_tm1650_write_once_raw
*  Input: cmd2
*  Input: address
*  Input: data
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_write_once_raw(UCHAR_T cmd2,UCHAR_T address,UCHAR_T data)
{
    OPERATE_RET op_ret;
    UCHAR_T buf[2];
    buf[0] = TM1650_WRITA_COMMAND1;
    buf[1] = cmd2;
    op_ret = tm1650_iic_write_page(buf,2);
    if(op_ret != OPRT_OK){
        PR_ERR("TM1650_WRITA_COMMAND1 error!");
        return OPRT_COM_ERROR;
    }
    buf[0] = address;
    buf[1] = data;
    op_ret = tm1650_iic_write_page(buf,2);
    if(op_ret != OPRT_OK){
        PR_ERR("TM1650_WRITA address1 error!");
        return OPRT_COM_ERROR;
    }
    return op_ret;
}

/***********************************************************
*  Function: tuya_tm1650_write_once
*  Input: address
*  Input: data
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_write_once(UCHAR_T address,UCHAR_T data)
{
    return tuya_tm1650_write_once_raw(TM1650_WRITA_COMMAND2_OPEN,address,data);
}


/***********************************************************
*  Function: tuya_tm1650_read
*  Input: data
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_tm1650_read(UCHAR_T *data)
{
    OPERATE_RET op_ret;
    if( NULL == data  ) {
        PR_ERR("1tuya_tm1650_read data is null");
        return OPRT_INVALID_PARM;
    }
    op_ret = tm1650_iic_start();     /* start transport */
    if( OPRT_OK != op_ret ) {
        PR_ERR("1IIC is busy!");
        return OPRT_COM_ERROR;
    }
    tm1650_iic_send_one_byte(TM1650_READ_COMMAND);
    vSocI2CSDA_IN();
    op_ret = tm1650_iic_wait_ack();
    if( OPRT_OK != op_ret ) {
        PR_ERR("1tm1650_iic_wait_ack!");
        return OPRT_COM_ERROR;
    }
    *data = tm1650_iic_recv_byte();
    vSocI2CSDA_OUT();
    tm1650_iic_ack();
    tm1650_iic_stop();
    return op_ret;
}

/**
* @brief: tuya_tm1650_light_enable
* @desc:  add by ali.zhang@tuya.com
* @param[in] on:set light enable on or off
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_tm1650_light_enable(IN CONST BOOL_T on)
{
    OPERATE_RET op_ret = OPRT_OK;

    UCHAR_T buf[2];
    buf[0] = TM1650_WRITA_COMMAND1;
    if (TRUE == on) {
        buf[1] = TM1650_WRITA_COMMAND2_OPEN;
    } else {
        buf[1] = TM1650_WRITA_COMMAND2_CLOSE;
    }
    
    op_ret = tm1650_iic_write_page(buf, 2);
    if (op_ret != OPRT_OK) {
        PR_ERR("tm1650_iic_write_page error!:%d", op_ret);
        return op_ret;
    }
    return op_ret;
}
