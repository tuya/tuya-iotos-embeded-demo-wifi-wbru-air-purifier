/**
* @file tuya_air_quality_funtion.c
* @author ali.zhang@tuya.com
* @brief tuya_air_quality_funtion module is used to air quality funtion
* @version 1.0.0
* @date 2021-09-17
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_air_quality_funtion.h"
#include "uni_log.h"
#include "tuya_uart.h"
#include "uni_thread.h"
#include "tuya_lcd_display.h"
#include "tuya_dp_process.h"
#include "tuya_automatic_mode_funtion.h"
#include "tuya_filter_funtion.h"

/*********************************************************************
******************************micro define****************************
*********************************************************************/
#define    UART0_BAUDRATE                   9600
#define    UART_BUFSZ                       22

#define    TVOC_PPB_TRANS_RATIO            (1.36)  //ug/m3 = 1.36ppb            
 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/
 
 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
//uart command, get air quality index
CONST UCHAR_T uart_buf_tx[4] = {0x11, 0x01, 0x16, 0xD8};

STATIC tuya_uart_t  *p_uart0 = NULL;

STATIC AIR_QUALITY_INDEX_T  g_air_quality_index;
STATIC LCD_SHOW_TYPE_E  g_lcd_show_type;

STATIC BOOL_T g_light_switch = FALSE;

/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_get_check_sum
* @desc: get check sum
* @param[in] pack:data source pointer
* @param[in] pack_len:data len
* @return none
*/
STATIC UCHAR_T tuya_get_check_sum(IN UCHAR_T *pack, IN UINT_T pack_len)
{
    UINT_T i;
    UCHAR_T check_sum = 0;

    for (i = 0; i < pack_len; i ++) {
        check_sum += *pack ++;
    }
    
    return check_sum;
}

/**
* @brief: tuya_uart0_init
* @desc: uart0 initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_uart0_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* find uart0 handle */
    p_uart0 = (tuya_uart_t *) tuya_driver_find(TUYA_DRV_UART, TUYA_UART0);
    if (NULL == p_uart0) {
        PR_ERR("p_uart0 find failed");
        return OPRT_COM_ERROR;
    }

    /* uart0 init and config */
    TUYA_UART_8N1_CFG(p_uart0, UART0_BAUDRATE, 256, 0);
    op_ret = tuya_uart_init(p_uart0);
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_uart_init error:%d", op_ret);
        return op_ret;
    }  

    PR_NOTICE("tuya_uart0_init succes");
    return op_ret;
}

/**
* @brief: tuya_get_air_quality_index
* @desc: get air quality index
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_get_air_quality_index(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T tmp_len = 0, read_len = 0, read_time = 0;
    UCHAR_T uart_buf_rx[UART_BUFSZ];
    UINT_T cs_check_num = 0;

    /* write command 11 01 16 D8 */
    tuya_uart_write(p_uart0, uart_buf_tx, sizeof(uart_buf_tx));

    /* read the return data */
    do {
        tmp_len = tuya_uart_read(p_uart0, &uart_buf_rx[read_len], UART_BUFSZ - read_len);
        read_len += tmp_len;
        if (read_time++ > 20) {
            PR_ERR("uart no return data");
            return OPRT_COM_ERROR;
        }
        tuya_hal_system_sleep(10);
    } while (read_len < UART_BUFSZ);

    /* check frame header */
    if ((uart_buf_rx[0] != 0x16) || (uart_buf_rx[1] != 0x13) || (uart_buf_rx[2] != 0x16)) {
        PR_ERR("uart receive data error, frame header");
        return OPRT_COM_ERROR;
    }

    /* check cs check num */
    cs_check_num = 256 - tuya_get_check_sum(uart_buf_rx, UART_BUFSZ-1);
    if (uart_buf_rx[UART_BUFSZ-1] != cs_check_num) {
        PR_ERR("uart receive data error, cs check num");
        return OPRT_COM_ERROR;
    }

    /* set air quality index value */
    g_air_quality_index.pm25 = uart_buf_rx[9]*256 + uart_buf_rx[10];
    g_air_quality_index.pm10 = uart_buf_rx[11]*256 + uart_buf_rx[12];
    g_air_quality_index.tvoc = (uart_buf_rx[3]*256 + uart_buf_rx[4]) * TVOC_PPB_TRANS_RATIO;  //ppb -> ug/m3
    g_air_quality_index.temperature = (uart_buf_rx[13]*256 + uart_buf_rx[14] - 500)/10;
    g_air_quality_index.humidity = (uart_buf_rx[15]*256 + uart_buf_rx[16])/10;

#if 0 //DEBUG
    PR_INFO("------------------------------data---------------------------------");
    PR_INFO("pm25:%d", g_air_quality_index.pm25);
    PR_INFO("pm10:%d", g_air_quality_index.pm10);
    PR_INFO("tvoc:%d", g_air_quality_index.tvoc);
    PR_INFO("temperature:%d", g_air_quality_index.temperature);
    PR_INFO("humidity:%d", g_air_quality_index.humidity);

    PR_INFO("--------------------uart_buf_rx data:");
    int i;
    for (i = 0; i < UART_BUFSZ; i++) {
        PR_DEBUG_RAW("%02X", uart_buf_rx[i]);
    }
#endif //DEBUG

    return op_ret;
}

/**
* @brief: tuya_show_air_quality
* @desc: lcd show air quality index
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_show_air_quality(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    INT_T show_num = 0;

    /* check if light switch is open */
    if (FALSE == g_light_switch) {
        return op_ret;
    }

    switch (g_lcd_show_type) {
    case SHOW_PM25:
        show_num = g_air_quality_index.pm25;
        break;
    case SHOW_PM10:
        show_num = g_air_quality_index.pm10;
        break;
    case SHOW_TVOC:
        show_num = g_air_quality_index.tvoc;
        break;
    case SHOW_TEMPERATURE:
        show_num = g_air_quality_index.temperature;
        break;
    case SHOW_HUMIDITY:
        show_num = g_air_quality_index.humidity;
        break;
    default:
        break;
    }

    op_ret = tuya_lcd_show_num(show_num);
    if(op_ret != OPRT_OK) {
        PR_ERR("tuya_lcd_show_num error");
        return op_ret;
    }

    op_ret = tuya_led_show(g_lcd_show_type);
    if(op_ret != OPRT_OK) {
        PR_ERR("tuya_led_show error");
        return op_ret;
    }
    return op_ret;
}

/**
* @brief: tuya_switch_show_air_quality_type
* @desc: switch air quality type show on lcd
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_switch_show_air_quality_type(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    g_lcd_show_type++;
    if (g_lcd_show_type < 5) {
        tuya_light_switch_set(TRUE);
    } else if (g_lcd_show_type == 5) {
        tuya_light_switch_set(FALSE);
    } else {
        g_lcd_show_type = 0;
        tuya_light_switch_set(TRUE);
    }

    return op_ret;
}

/**
* @brief: tuya_get_air_quality_task_cb
* @desc: get air quality task thread start cb
* 
* @return none
*/
STATIC VOID_T tuya_get_air_quality_task_cb(VOID_T)
{

    while(1)
    {
        //get air quality data
        tuya_get_air_quality_index();

        //show on lcd
        tuya_show_air_quality();

        //dp report
        tuya_air_quality_dp_updata();

        tuya_set_air_quality_index(g_air_quality_index); //ali_review

        tuya_hal_system_sleep(GET_AIR_QUALITY_TIMER); //4s  
    }
}

/**
* @brief: tuya_get_air_quality_task_init
* @desc: get air quality task thread start
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_get_air_quality_task_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    
    THRD_HANDLE ty_user_handle = NULL;
    THRD_PARAM_S thrd_param;
    thrd_param.priority = TRD_PRIO_3; 
    thrd_param.stackDepth = 515*4;
    thrd_param.thrdname = "ty_get_air_quality_task";
    op_ret = CreateAndStart(&ty_user_handle, NULL, NULL, tuya_get_air_quality_task_cb, NULL, &thrd_param);      
    if(op_ret != OPRT_OK) {
        PR_ERR("start tuya_get_air_quality_task_cb ty_task_cb err");
        return op_ret;
    }
    
    PR_NOTICE("tuya_get_air_quality_task_init success !!!");
    return op_ret;
}

/**
* @brief: tuya_air_quality_dp_updata
* @desc: updata air quality dp value
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_air_quality_dp_updata(VOID_T)
{
    if (g_air_quality_index.pm25 > PM25_MAX) {
        g_air_quality_index.pm25 = PM25_MAX;
    }
    if (g_air_quality_index.pm10 > PM10_MAX) {
        g_air_quality_index.pm10 = PM10_MAX;
    }
    if (g_air_quality_index.tvoc > TVOC_MAX) {
        g_air_quality_index.tvoc = TVOC_MAX;
    }
    if (g_air_quality_index.humidity > HUMIDITY_MAX) {
        g_air_quality_index.humidity = HUMIDITY_MAX;
    }
    if (g_air_quality_index.temperature > TEMPERATURE_MAX) {
        g_air_quality_index.temperature = TEMPERATURE_MAX;
    }
    if (g_air_quality_index.temperature < TEMPERATURE_MIN) {
        g_air_quality_index.temperature = TEMPERATURE_MIN;
    }

    tuya_update_single_dp(PM25_DPID, PROP_VALUE, g_air_quality_index.pm25);
    tuya_update_single_dp(PM10_DPID, PROP_VALUE, g_air_quality_index.pm10);
    tuya_update_single_dp(TVOC_DPID, PROP_VALUE, g_air_quality_index.tvoc);
    tuya_update_single_dp(TEMP_INDOOR_DPID, PROP_VALUE, g_air_quality_index.temperature);
    tuya_update_single_dp(HUMIDITY_DPID, PROP_VALUE, g_air_quality_index.humidity);

    return OPRT_OK;
}

/**
* @brief: tuya_light_switch_set
* @desc: set light switch
* @param[in] light_switch:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_light_switch_set(IN CONST BOOL_T light_switch)
{
    OPERATE_RET op_ret = OPRT_OK;

    g_light_switch = light_switch;

    tuya_update_single_dp(LIGHT_DPID, PROP_BOOL, light_switch);

    op_ret = tuya_lcd_show_light_enable(light_switch);
    if(op_ret != OPRT_OK) {
        PR_ERR("tuya_lcd_show_light_enable error:%d", op_ret);
        return op_ret;
    }

    op_ret = tuya_show_air_quality();
    if(op_ret != OPRT_OK) {
        PR_ERR("tuya_show_air_quality error:%d", op_ret);
        return op_ret;
    }

    return op_ret;
}
