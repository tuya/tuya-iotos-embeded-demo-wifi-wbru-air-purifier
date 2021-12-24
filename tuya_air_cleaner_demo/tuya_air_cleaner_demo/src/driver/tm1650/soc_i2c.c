/***********************************************************
*  File: soc_i2c.c
*  Author: fengzq
*  Date: 20210501
*  Note: 
***********************************************************/

#include "soc_i2c.h"
#include "tuya_gpio.h"
#include "uni_log.h"
#include <string.h>

STATIC UCHAR_T gucI2cInitFlag = FALSE;
STATIC I2C_PIN_T gtI2cGpio;

/**
 * @brief: SOC i2c init
 * @param {I2C_PIN_T *pI2CPinCfg --> i2c init gpio struct}
 * @retval: none
 */
OPERATE_RET opSocI2CInit(IN I2C_PIN_T *pI2CPinCfg)
{
    OPERATE_RET opRet = 0;

    if(gucI2cInitFlag != FALSE) {
        PR_NOTICE("SOC I2C already init!");
        return OPRT_OK;
    }
    opRet = tuya_gpio_inout_set(pI2CPinCfg -> ucSdaPin, FALSE);      /* output */
    if(opRet != OPRT_OK){
        PR_ERR("soc i2c sda gpio init error!");
        return OPRT_INVALID_PARM;
    }
    opRet = tuya_gpio_inout_set(pI2CPinCfg -> ucSclPin, FALSE);      /* output */
    if(opRet != OPRT_OK){
        PR_ERR("soc i2c scl gpio init error!");
        return OPRT_INVALID_PARM;
    }
    opRet = tuya_gpio_write(pI2CPinCfg -> ucSdaPin, TRUE);
    if(opRet != OPRT_OK) {
        PR_ERR("I2C SDA gpio set error!");
        return OPRT_INVALID_PARM;
    }
    opRet = tuya_gpio_write(pI2CPinCfg -> ucSclPin, TRUE);
    if(opRet != OPRT_OK) {
        PR_ERR("I2C SCL gpio set error!");
        return OPRT_INVALID_PARM;
    }
    memcpy(&gtI2cGpio, pI2CPinCfg, SIZEOF(I2C_PIN_T));
    gucI2cInitFlag = TRUE;
    return OPRT_OK;
}

/**
 * @brief: SOC i2c SDA reset proc
 * @param {none}
 * @retval: none
 */
VOID vSocI2CSdaReset(VOID)
{
    OPERATE_RET opRet = 0;
    
    if(gucI2cInitFlag != TRUE) {
        PR_ERR("I2C is not't init!");
        return ;
    }
    opRet = tuya_gpio_inout_set(gtI2cGpio.ucSdaPin, FALSE);      /* output */
    if(opRet != OPRT_OK) {
        PR_ERR("soc i2c sda gpio set output error!");
        return ;
    }
    opRet = tuya_gpio_write(gtI2cGpio.ucSdaPin, FALSE);
    if(opRet != OPRT_OK) {
        PR_ERR("I2C SDA gpio set error!");
        return ;
    }
}

/**
 * @brief: SOC i2c SDA set proc
 * @param {none}
 * @retval: none
 */
VOID vSocI2CSdaSet(VOID)
{
    OPERATE_RET opRet = 0;
    
    if(gucI2cInitFlag != TRUE) {
        PR_ERR("I2C is not't init!");
        return ;
    }
    opRet = tuya_gpio_inout_set(gtI2cGpio.ucSdaPin, FALSE);      /* output */
    if(opRet != OPRT_OK) {
        PR_ERR("soc i2c sda gpio set output error!");
        return ;
    }
    
    opRet = tuya_gpio_write(gtI2cGpio.ucSdaPin, TRUE);
    if(opRet != OPRT_OK) {
        PR_ERR("I2C SDA gpio set error!");
        return ;
    }
}

/**
 * @brief: SOC i2c SCL reset proc
 * @param {none}
 * @retval: none
 */
VOID vSocI2CSclReset(VOID)
{
    OPERATE_RET opRet = 0;
    if(gucI2cInitFlag != TRUE){
        PR_ERR("I2C is not't init!");
        return ;
    }
    opRet = tuya_gpio_inout_set(gtI2cGpio.ucSclPin, FALSE);      /* output */
    if(opRet != OPRT_OK){
        PR_ERR("soc i2c scl gpio set output error!");
        return ;
    }
    opRet = tuya_gpio_write(gtI2cGpio.ucSclPin, FALSE);
    if(opRet != OPRT_OK) {
        PR_ERR("I2C scl gpio set error!");
        return ;
    }
}

/**
 * @brief: SOC i2c SCL set proc
 * @param {none}
 * @retval: none
 */
VOID vSocI2CSclSet(VOID)
{
    OPERATE_RET opRet = 0;
    if(gucI2cInitFlag != TRUE){
        PR_ERR("I2C is not't init!");
        return ;
    }
    opRet = tuya_gpio_inout_set(gtI2cGpio.ucSclPin, FALSE);      /* output */
    if(opRet != OPRT_OK){
        PR_ERR("soc i2c scl gpio set output error!");
        return ;
    }
    opRet = tuya_gpio_write(gtI2cGpio.ucSclPin, TRUE);
    if(opRet != OPRT_OK) {
        PR_ERR("I2C scl gpio set error!");
        return ;
    }
}

/**
 * @brief: SOC i2c SDA(input) read
 * @param {none}
 * @retval: none
 */
UCHAR_T ucSocI2CSdaInputRead(VOID)
{
    return tuya_gpio_read(gtI2cGpio.ucSdaPin);
}

/**
 * @brief: SOC i2c delay
 * @param {none}
 * @attention: soc i2c software delay, this maybe need adjust
 * @retval: none
 */
VOID vSocI2CDelay(USHORT_T usTime)
{
    /* usTime = usTime * gain, gain need set by test! */
    while(usTime --);
}

/**
 * @brief: SOC i2c SDA io set
 * @param {none}
 * @retval: none
 */
VOID vSocI2CSDA_IN(VOID)
{
    tuya_gpio_inout_set(gtI2cGpio.ucSdaPin, TRUE);
}

/**
 * @brief: SOC i2c SDA io set
 * @param {none}
 * @retval: none
 */
VOID vSocI2CSDA_OUT(VOID)
{
    tuya_gpio_inout_set(gtI2cGpio.ucSdaPin, FALSE);
}