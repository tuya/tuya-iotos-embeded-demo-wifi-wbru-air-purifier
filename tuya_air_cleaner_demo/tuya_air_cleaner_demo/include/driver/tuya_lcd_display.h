/**
* @file tuya_lcd_display.h
* @author ali.zhang@tuya.com
* @brief tuya_lcd_display module is used to lcd driver and display
* @version 1.0.0
* @date 2021-09-23
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
#ifndef __TUYA_LCD_DISPLAY_H__
#define __TUYA_LCD_DISPLAY_H__
 
#include "tuya_cloud_types.h"
#include "tuya_gpio.h"
 
#ifdef __cplusplus
extern "C" {
#endif
 
/*********************************************************************
******************************micro define****************************
*********************************************************************/
#define    CLK_PIN                        TY_GPIOA_2
#define    DIO_PIN                        TY_GPIOA_3
 
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
* @brief: tuya_lcd_sw_i2c_init
* @desc:  lcd software i2c initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_lcd_sw_i2c_init(VOID_T);

/**
* @brief: tuya_lcd_show_num
* @desc: tuya_lcd_show_num
* @param[in] number:-99<number<999
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_lcd_show_num(IN INT_T number);

/**
* @brief: tuya_lcd_show_null
* @desc: tuya_lcd_show_null
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_lcd_show_null(VOID_T);

/**
* @brief: tuya_led_show
* @desc: tuya_led_show
* @param[in] show_led: 0,1,2,3,4,5
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_led_show(IN CONST UCHAR_T show_led);

/**
* @brief: tuya_lcd_show_light_enable
* @desc:  lcd and led light enable
* @param[in] on:set light enable on or off
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_lcd_show_light_enable(IN CONST BOOL_T on);

 
#if 0
OPERATE_RET tuya_i2c1_init(VOID_T);

OPERATE_RET tuya_ic21_wirte(VOID_T);
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__TUYA_LCD_DISPLAY_H__*/