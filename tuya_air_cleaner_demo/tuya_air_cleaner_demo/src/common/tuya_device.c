/**
* @file tuya_device.c
* @author ali.zhang@tuya.com
* @brief tuya_device module is used to 
* @version 1.0.0
* @date 2021-09-08
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_device.h"
#include "tuya_iot_wifi_api.h"
#include "tuya_iot_com_api.h"
#include "tuya_iot_internal_api.h"
#include "tuya_cloud_wifi_defs.h"
#include "uni_log.h"
#include "tuya_gpio.h"
#include "sys_timer.h"

#include "tuya_iot_funtion.h"
#include "tuya_key_funtion.h"
#include "tuya_hardware_driver.h"
#include "tuya_dp_process.h"
#include "tuya_buz_driver.h"
#include "tuya_motor_driver.h"
#include "tuya_air_quality_funtion.h"
#include "tuya_mode_funtion.h"


/*********************************************************************
******************************micro define****************************
*********************************************************************/
#define    PRODUCT_KEY                        "ragfgdqlyfikxpu7"
 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/
 
 
/*********************************************************************
****************************variable define***************************
****************************************************************/
 
 
/*********************************************************************
****************************function define***************************
*********************************************************************/
VOID_T pre_app_init(VOID_T)
{
    return;
}

VOID_T mf_user_product_test_cb(VOID_T)
{


}

VOID_T mf_user_enter_callback(VOID_T)
{


}

VOID_T mf_user_pre_gpio_test_cb(VOID_T)
{


}

VOID_T pre_device_init(VOID_T)
{
    PR_DEBUG("%s",tuya_iot_get_sdk_info());
    PR_DEBUG("%s:%s",APP_BIN_NAME,USER_SW_VER);
    SetLogManageAttr(TY_LOG_LEVEL_DEBUG);

    mf_test_ignore_close_flag(); 
}

VOID_T mf_user_callback(VOID_T)
{

}

VOID_T app_init(VOID_T)
{
    tuya_hal_log_close();

    tuya_hardware_gpio_init();
    tuya_buz_pwm_init();
    tuya_motor_pwm_init();
    tuya_uart0_init();
    tuya_lcd_sw_i2c_init();
    tuya_nfc_funtion_init();
}

BOOL_T gpio_test(VOID_T)
{
    return gpio_test_all();
}

OPERATE_RET user_product_test_cb(USHORT_T cmd,UCHAR_T *data, UINT_T len, OUT UCHAR_T **ret_data,OUT USHORT_T *ret_len)
{
    return OPRT_OK;
}


OPERATE_RET device_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = tuya_key_funtion_init();
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_key_funtion_init err:%d", op_ret);
        return op_ret;
    }

    op_ret = ty_iot_funtion_start_init(GWCM_OLD, PRODUCT_KEY);
    if (OPRT_OK != op_ret) {
        PR_ERR("ty_iot_funtion_start_init err:%d", op_ret);
        return op_ret;
    }

    op_ret = tuya_air_cleaner_init();
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_air_cleaner_init err:%d", op_ret);
        return op_ret;
    }

    op_ret = tuya_get_air_quality_task_init();
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_get_air_quality_task_init err:%d", op_ret);
        return op_ret;
    }

    op_ret = tuya_filter_life_figure_task_init();
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_filter_life_figure_task_init err:%d", op_ret);
        return op_ret;
    }

    op_ret = tuya_check_door_and_filter_task_init();
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_check_door_and_filter_task_init err:%d", op_ret);
        return op_ret;
    }

    PR_NOTICE("dev_init sucess !!!");
    return OPRT_OK;
}

