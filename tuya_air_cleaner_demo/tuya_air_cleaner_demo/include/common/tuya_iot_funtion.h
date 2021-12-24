/**
 * @file tuya_iot_funtion.h
 * @author wangwei@tuya.com
 * @brief Tuya Infrared application module Demo
 * @version 1.0.0
 * @date 2021-05-25
 * @copyright Copyright (c) tuya.inc 2021
 */

#ifndef _TUYA_IOT_FUNTION_H
#define _TUYA_IOT_FUNTION_H

#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"

#ifdef __cplusplus
extern "C" {
#endif


/** 
 * @brief: tuya IOT initialization
 * @desc: LED lamp control interface
 * @param[in] cfg_mode:Initialization mode
 * @param[in] p_product_id:Product ID pointer
 * @return OPERATE_RET  OPRT_OK is sucess, other is fail
 * @note none
 */
OPERATE_RET ty_iot_funtion_start_init(IN CONST GW_WF_CFG_MTHD_SEL cfg_mode,IN CONST CHAR_T *p_product_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
