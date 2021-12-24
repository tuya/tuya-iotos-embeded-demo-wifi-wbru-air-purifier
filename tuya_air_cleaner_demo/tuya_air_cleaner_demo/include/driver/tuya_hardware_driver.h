/**
* @file tuya_hardware_driver.h
* @author ali.zhang@tuya.com
* @brief tuya_hardware_driver module is used to 
* @version 1.0.0
* @date 2021-09-08
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
#ifndef __TUYA_HARDWARE_DRIVER_H__
#define __TUYA_HARDWARE_DRIVER_H__
 
#include "tuya_cloud_types.h"
#include "tuya_gpio.h"
 
#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
******************************micro define****************************
*********************************************************************/
#define    UV_LIGHT_PIN                        TY_GPIOA_10 //uv
#define    LIGHT_SEN_PIN                       TY_GPIOA_16 //L_TX
#define    DODR_SEN_PIN                        TY_GPIOA_4

/*********************************************************************
****************************typedef define****************************
*********************************************************************/
 
 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
#define    DARK_FLAG                      tuya_gpio_read(LIGHT_SEN_PIN)
#define    DOOR_OPEN_FLAG                 tuya_gpio_read(DODR_SEN_PIN)

/*********************************************************************
****************************function define***************************
*********************************************************************/

/**
* @brief: tuya_hardware_gpio_init
* @desc: hardware gpio initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_hardware_gpio_init(VOID_T);

/**
* @brief: tuya_uv_set_on
* @param[in] mode:select uv light set on or off
* @ref            TRUE:set on
* @ref            FALSE:set off
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_uv_set_on(IN CONST BOOL_T mode);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_HARDWARE_DRIVER_H__*/