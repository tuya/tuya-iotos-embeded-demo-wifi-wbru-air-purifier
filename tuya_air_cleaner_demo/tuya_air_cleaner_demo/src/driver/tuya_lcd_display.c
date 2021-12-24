/**
* @file tuya_lcd_display.c
* @author ali.zhang@tuya.com
* @brief tuya_lcd_display module is used to lcd driver and display
* @version 1.0.0
* @date 2021-09-23
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_lcd_display.h"
#include "uni_log.h"
#include "tm1650_app.h"

/*********************************************************************
******************************micro define****************************
*********************************************************************/


/*********************************************************************
****************************typedef define****************************
*********************************************************************/
//show minus flag
typedef enum {
    MINUS_NULL = 0,  //no minus
    MINUS_HUNDRED,   //minus show in ten bit
    MINUS_TEN,       //minus show in hundred bit
}MINUS_FLAG_E;
 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
#define    MAX_NUM                           999
#define    MIN_NUM                           -99
#define    SEG_SHOW_MINUS                    10
#define    SEG_SHOW_NULL                     11

//select bit
CONST UCHAR_T dig[4] = {
    0x68,       /* hundred */
    0x6A,       /* ten */
    0x6C,       /* one */
    0x6E,       /* led */
};

/* f a e d dp c g b */
CONST UCHAR_T seg_num[12] = {
    0xF5,       /* 0 */
    0x05,       /* 1 */
    0x73,       /* 2 */
    0x57,       /* 3 */
    0x87,       /* 4 */
    0xD6,       /* 5 */
    0xF6,       /* 6 */
    0x45,       /* 7 */
    0xF7,       /* 8 */
    0xD7,       /* 9 */
    0x02,       /* - */
    0x00        /*   */
};
 
CONST UCHAR_T seg_led[6] = {
    0x20,       /* 1th */
    0x10,       /* 2th */
    0x08,       /* 3th */
    0x04,       /* 4th */
    0x02,       /* 5th */
    0x00        /*     */
};

/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_lcd_sw_i2c_init
* @desc:  lcd software i2c initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_lcd_sw_i2c_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = tuya_tm1650_app_init(CLK_PIN, DIO_PIN);
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_tm1650_app_init error:%d", op_ret);
        return op_ret;
    }

    PR_NOTICE("tuya_lcd_sw_i2c_init succes");
    return op_ret;
}

/**
* @brief: tuya_lcd_show_num
* @desc: show num on lcd
* @param[in] number:-99<number<999
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_lcd_show_num(IN INT_T number)
{
    OPERATE_RET op_ret = OPRT_OK;
    MINUS_FLAG_E minus_flag = MINUS_NULL;
    UCHAR_T i = 0;
    UCHAR_T bit[3];

    if (number > MAX_NUM) {
        number = MAX_NUM;
    }
    if (number < MIN_NUM) {
        number = MIN_NUM;
    }
    if (number < 0) {
        if (number > -10) {  //-10<number<0
            minus_flag = MINUS_TEN;
        } else {             //-100<number<-10
            minus_flag = MINUS_HUNDRED;
        }
        number = number * (-1);
    }

    bit[0] = number / 100 % 10;  //hundred
    bit[1] = number / 10 % 10;   //ten
    bit[2] = number % 10;        //one

    /* hundred == 0 */
    if (bit[0] == 0) {
        if (minus_flag == MINUS_HUNDRED) {
            bit[0] = SEG_SHOW_MINUS;  //hundred bit show minus
        } else {
            bit[0] = SEG_SHOW_NULL;   //hundred bit show null
        }
    /* ten == 0 */
        if (bit[1] == 0) {
            if (minus_flag == MINUS_TEN) {
                bit[1] = SEG_SHOW_MINUS;  //ten bit show minus
            } else {
                bit[1] = SEG_SHOW_NULL;  //ten bit show null
            }
        }
    }

    /* i2c send command and data */
    for (i = 0; i < 3; i++) {
        op_ret = tuya_tm1650_app_write(dig[i], seg_num[bit[i]]); 
        if (op_ret != OPRT_OK) {
            PR_ERR("tuya_tm1650_app_write error:%d", op_ret);
            return op_ret;
        }
    }

    return op_ret;
}

/**
* @brief: tuya_lcd_show_null
* @desc: tuya_lcd_show_null
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_lcd_show_null(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    UCHAR_T i = 0;

    for (i = 0; i < 4; i++) {
        op_ret = tuya_tm1650_app_write(dig[i], seg_num[11]); 
        if (op_ret != OPRT_OK) {
            PR_ERR("tuya_tm1650_app_write error:%d", op_ret);
            return op_ret;
        }
    }

    return op_ret;
}

/**
* @brief: tuya_led_show
* @desc: Choose the led that led show up
* @param[in] show_led: 0,1,2,3,4,5
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_led_show(IN CONST UCHAR_T show_led)
{
    OPERATE_RET op_ret = OPRT_OK;

    //Check the input parameter, show_led: 0~5
    if (show_led > 5) {
        PR_ERR("show_led input parameter error!");
        return OPRT_INVALID_PARM;
    }
    
    /* i2c send command and data */
    op_ret = tuya_tm1650_app_write(dig[3], seg_led[show_led]); 
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_tm1650_app_write error:%d", op_ret);
        return op_ret;
    }
    return op_ret;
}

/**
* @brief: tuya_lcd_show_light_enable
* @desc:  lcd and led light enable
* @param[in] on:set light enable on or off
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_lcd_show_light_enable(IN CONST BOOL_T on)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = tuya_tm1650_app_light_enable(on);
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_tm1650_app_light_enable error:%d", op_ret);
        return op_ret;
    }
    return op_ret;
}



#if 0  //I2C API DEBUG

STATIC tuya_i2c_t  *p_i2c1;

/**
* @brief: tuya_sw_i2c0_init
* @desc:  software i2c0 initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_i2c1_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* register software i2c1 */
    tuya_i2c_sw_cfg_t  sw_cfg;
    TUYA_I2C_SW_CFG_INIT(&sw_cfg, CLK_PIN, DIO_PIN, 10);
    tuya_sw_i2c_register(TUYA_I2C1, &sw_cfg);

	/* find i2c1 handle */
    p_i2c1 = (tuya_i2c_t *) tuya_driver_find(TUYA_DRV_I2C, TUYA_I2C1);
    if (NULL == p_i2c1) {
        PR_ERR("p_i2c1 find failed");
        return OPRT_COM_ERROR;
    }
    
    /* i2c1 init and config */
    TUYA_I2C_MASTER_CFG(p_i2c1, 0x00);
    op_ret = tuya_i2c_init(p_i2c1);
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_i2c1_init error:%d", op_ret);
        return op_ret;
    }  

    PR_NOTICE("tuya_i2c1_init succes");
    return op_ret;
}



/**
* @brief: tuya_lcd_show_num
* @desc: tuya_lcd_show_num
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_ic21_wirte(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    UCHAR_T i2c_buf[2];

    memset(i2c_buf, 0, 2);
    i2c_buf[0] = 0x01;  //command1

    op_ret = tuya_i2c_master_send(p_i2c1, 0x48, TUYA_I2C_IGNORE_NACK, i2c_buf, 1);
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_i2c_master_send error:%d", op_ret);
    }

    i2c_buf[0] = 0xF5;  //command

    op_ret = tuya_i2c_master_send(p_i2c1, 0x68, TUYA_I2C_NO_START | TUYA_I2C_IGNORE_NACK, i2c_buf, 1);
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_i2c_master_send error:%d", op_ret);
    }

    PR_INFO("send1 success");
    return op_ret;
}

#endif //I2C API DEBUG
