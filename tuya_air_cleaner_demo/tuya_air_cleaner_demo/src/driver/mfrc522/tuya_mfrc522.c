/**
* @file tuya_mfrc522.c
* @author ali.zhang@tuya.com
* @brief tuya_mfrc522 module is used to dirver mfrc522
* @version 1.0.0
* @date 2021-10-28
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_mfrc522.h"
#include "uni_log.h"
 
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
/**
* @brief: tuya_mfrc522_pin_init
* @desc: mfrc522 pin initialize
* @return OPERATE_RET
*/
OPERATE_RET tuya_mfrc522_pin_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* SCK pin set output */
    op_ret = tuya_gpio_inout_set(RF_SCK, FALSE);
    if (OPRT_OK != op_ret) {
        PR_ERR("SCK pin set output error:%d", op_ret);
        return op_ret;
    }

    /* MOSI pin set output */
    op_ret = tuya_gpio_inout_set(RF_MOSI, FALSE);
    if (OPRT_OK != op_ret) {
        PR_ERR("MOSI pin set output error:%d", op_ret);
        return op_ret;
    }

    /* NSS pin set output */
    op_ret = tuya_gpio_inout_set(RF_NSS, FALSE);
    if (OPRT_OK != op_ret) {
        PR_ERR("NSS pin set output error:%d", op_ret);
        return op_ret;
    }

    /* MISO pin set input */
    op_ret = tuya_gpio_inout_set(RF_MISO, TRUE);
    if (OPRT_OK != op_ret) {
        PR_ERR("MISO pin set input error:%d", op_ret);
        return op_ret;
    }

    /* SCK pin set low */
    op_ret = tuya_gpio_write(RF_SCK, FALSE);
    if (OPRT_OK != op_ret) {
        PR_ERR("SCK pin set error:%d", op_ret);
        return op_ret;
    }

    /* MOSI pin set high */
    op_ret = tuya_gpio_write(RF_MOSI, TRUE);
    if (OPRT_OK != op_ret) {
        PR_ERR("MOSI pin set error:%d", op_ret);
        return op_ret;
    }

    /* NSS pin set high*/
    op_ret = tuya_gpio_write(RF_NSS, TRUE);
    if (OPRT_OK != op_ret) {
        PR_ERR("NSS pin set error:%d", op_ret);
        return op_ret;
    }

    return op_ret;
}

/**
* @brief: _SCK_set
* @desc: set SCK pin
* @param[in] high:
* @return OPERATE_RET
*/
STATIC OPERATE_RET _SCK_set(IN CONST BOOL_T high)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* SCK pin set */
    op_ret = tuya_gpio_write(RF_SCK, high);
    if (OPRT_OK != op_ret) {
        PR_ERR("SCK pin set error:%d", op_ret);
        return op_ret;
    }
    return op_ret;
}

/**
* @brief: _MOSI_set
* @desc: set MOSI pin
* @param[in] high:
* @return OPERATE_RET
*/
STATIC OPERATE_RET _MOSI_set(IN CONST BOOL_T high)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* MOSI pin set */
    op_ret = tuya_gpio_write(RF_MOSI, high);
    if (OPRT_OK != op_ret) {
        PR_ERR("MOSI pin set error:%d", op_ret);
        return op_ret;
    }
    return op_ret;
}

/**
* @brief: _NSS_set
* @desc: set NSS pin
* @param[in] high:
* @return OPERATE_RET
*/
STATIC OPERATE_RET _NSS_set(IN CONST BOOL_T high)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* NSS pin set */
    op_ret = tuya_gpio_write(RF_NSS, high);
    if (OPRT_OK != op_ret) {
        PR_ERR("NSS pin set error:%d", op_ret);
        return op_ret;
    }
    return op_ret;
}

/**
* @brief: _MISO_Read
* @desc: Read MISO pin
* @return MISO
*/
STATIC UCHAR_T _MISO_Read(VOID_T)
{
    return tuya_gpio_read(RF_MISO);
}

// ---------------------------------------------------------------------------
// Register interaction functions.
// ---------------------------------------------------------------------------
/**
* @brief: ReadRawRC
* @desc: read mfrc522 register
* @param[in] reg:register address
* @return read value
*/
UCHAR_T ReadRawRC(UCHAR_T reg)
{
    UCHAR_T i, ucAddr;
    UCHAR_T ucResult = 0;

    _SCK_set(FALSE);
    _NSS_set(FALSE);
    ucAddr = ((reg << 1) & 0x7E) | 0x80;

    for (i = 0; i < 8; i++) {
        _MOSI_set((ucAddr & 0x80) == 0x80);
        _SCK_set(TRUE);
        ucAddr <<= 1;
        _SCK_set(FALSE);
    }

    for (i = 0; i < 8; i++) {
        _SCK_set(TRUE);
        ucResult <<= 1;
        ucResult |= _MISO_Read();
        _SCK_set(FALSE);
        
    }

    _NSS_set(TRUE);
    _SCK_set(TRUE);
    return ucResult;
}

/**
* @brief: WriteRawRC
* @desc: write mfrc522 register
* @param[in] reg:register address
* @param[in] value:write value
* @return none
*/
VOID_T WriteRawRC(UCHAR_T reg, UCHAR_T value)
{  
    UCHAR_T i, ucAddr;

    _SCK_set(FALSE);
    _NSS_set(FALSE);
    ucAddr = ((reg << 1) & 0x7E);

    for (i = 0; i < 8; i++) {
		_MOSI_set((ucAddr & 0x80) == 0x80);
        _SCK_set(TRUE);
        ucAddr <<= 1;
        _SCK_set(FALSE);
    }

    for (i = 0; i < 8; i++) {
		_MOSI_set((value & 0x80) == 0x80);
        _SCK_set(TRUE);
        value <<= 1;
        _SCK_set(FALSE);
    }
    
    _NSS_set(TRUE);
    _SCK_set(TRUE);
}