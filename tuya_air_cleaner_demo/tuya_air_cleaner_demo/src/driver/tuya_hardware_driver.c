/**
* @file tuya_hardware_driver.c
* @author ali.zhang@tuya.com
* @brief tuya_hardware_driver module is used to driver hardware
* @version 1.0.0
* @date 2021-09-08
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_hardware_driver.h"
#include "uni_log.h"
#include "uni_thread.h"
#include "tuya_dp_process.h"
#include "tuya_nfc_funtion.h"
#include "tuya_mode_funtion.h"
#include "tuya_automatic_mode_funtion.h"
 
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
****************************************************************/
/**
* @brief: tuya_uv_pin_init
* @desc: uv pin initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_uv_pin_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* uv pin set output */
    op_ret = tuya_gpio_inout_set(UV_LIGHT_PIN, FALSE);
    if (op_ret != OPRT_OK) {
        PR_ERR("uv light pin set output error:%d", op_ret);
        return op_ret;
    }

    /* uv init, set off */
    op_ret = tuya_gpio_write(UV_LIGHT_PIN, FALSE);
    if (op_ret != OPRT_OK) {
        PR_ERR("uv light init set off error:%d", op_ret);
        return op_ret;
    }

    return op_ret;
}

/**
* @brief: tuya_light_sen_pin_init
* @desc: light sensor pin initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_light_sen_pin_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* light sensor pin set input */
    op_ret = tuya_gpio_inout_set(LIGHT_SEN_PIN, TRUE);
    if (op_ret != OPRT_OK) {
        PR_ERR("light sensor pin set input error:%d", op_ret);
        return op_ret;
    }

    return op_ret;
}

/**
* @brief: tuya_door_sen_pin_init
* @desc: door sensor pin initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_door_sen_pin_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* door sen irq init, rising edge */
    op_ret = tuya_gpio_irq_init(DODR_SEN_PIN, tuya_door_sen_pin_irq_cb, TY_IRQ_RISE, 1);
    if (op_ret != OPRT_OK) {
        PR_ERR("door_sen_irq_init error:%d", op_ret);
        return op_ret;
    }

    return op_ret;
}

/**
* @brief: tuya_hardware_gpio_init
* @desc: hardware gpio initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_hardware_gpio_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = tuya_uv_pin_init();
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_uv_pin_init error:%d", op_ret);
        return op_ret;
    }

    op_ret = tuya_light_sen_pin_init();
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_light_sen_pin_init error:%d", op_ret);
        return op_ret;
    }

    op_ret = tuya_door_sen_pin_init();
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_door_sen_pin_init error:%d", op_ret);
        return op_ret;
    }

    PR_NOTICE("tuya_hardware_gpio_init sucess !!!");
    return op_ret;
}

/**
* @brief: tuya_uv_set_on
* @param[in] mode:select uv light set on or off
* @ref            TRUE:set on
* @ref            FALSE:set off
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_uv_set_on(IN CONST BOOL_T mode)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* uv light set on or off */
    op_ret = tuya_gpio_write(UV_LIGHT_PIN, mode);
    if (op_ret != OPRT_OK) {
        PR_ERR("uv light set on error:%d", op_ret);
        return op_ret;
    }

    tuya_set_uv_status(mode);

    PR_DEBUG("ali, uv_light_status:%d", mode);

    return op_ret;
}
