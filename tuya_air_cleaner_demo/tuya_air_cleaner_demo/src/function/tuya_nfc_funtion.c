/**
* @file tuya_nfc_funtion.c
* @author ali.zhang@tuya.com
* @brief tuya_nfc_funtion module is used to nfc funtion
* @version 1.0.0
* @date 2021-10-28
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_nfc_funtion.h"
#include "uni_log.h"
#include "tuya_mfrc522_app.h"
#include "tuya_filter_funtion.h"
#include "tuya_dp_process.h"
#include "tuya_ws_db.h"
#include "tuya_hardware_driver.h"
#include "tuya_motor_driver.h"
#include "tuya_buz_driver.h"
#include "sys_timer.h"
#include "tuya_mode_funtion.h"
#include "uni_thread.h"
#include "tuya_os_adapt_semaphore.h"

/*********************************************************************
******************************micro define****************************
*********************************************************************/
#define   FILTER_ID_SAVE             "filter_id_save"
 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/
 
 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
STATIC UCHAR_T filter_buf[CARD_READ_BYTE_LEN];

STATIC FILTER_CARD_MSG_T g_filter_card_msg;

STATIC TIMER_ID timer_check_door_filter;

STATIC SEM_HANDLE p_check_semaphore_handle;
/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_nfc_funtion_init
* @desc: nfc funtion initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_nfc_funtion_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = tuya_mfrc522_app_init();
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_mfrc522_app_init err:%d", op_ret);
        return op_ret;
    }

    PR_NOTICE("tuya_nfc_funtion_init sucess");
    return op_ret;
}

/**
* @brief: tuya_write_filter_id
* @desc: write filter_id in flash
* @param[in] filter_id:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_write_filter_id(IN UINT_T filter_id)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T value_in = filter_id;

    op_ret = wd_common_write(FILTER_ID_SAVE, &value_in, sizeof(UINT_T));
    if (OPRT_OK != op_ret) {
        PR_ERR("wd_common_write err:%d", op_ret);
        return op_ret;
    }

    PR_DEBUG("ali, flash write, filter_id:%d", value_in);
    return OPRT_OK;
}

/**
* @brief: tuya_read_filter_id
* @desc: read filter_id from flash
* @param[in] filter_id:
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_read_filter_id(IN OUT UINT_T *filter_id)
{
    OPERATE_RET op_ret = OPRT_OK;
    BYTE_T *buff = NULL;
    UINT_T buff_len = 0;

    op_ret = wd_common_read(FILTER_ID_SAVE, &buff, &buff_len);
    if (OPRT_OK != op_ret) {
        PR_NOTICE("ali, wd_common_read, filter_id is null!!");
        return OPRT_OK;
    }

    UINT_T *temp = (UINT_T *)buff;
    *filter_id = *temp;

    buff = NULL;
    temp = NULL;

    PR_DEBUG("ali, flash read, filter_id:%d", *filter_id);
    return OPRT_OK;
}

/**
* @brief: tuya_check_whether_the_filter_is_new
* @desc: check whether the filter is new
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_check_whether_the_filter_is_new(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T filter_ID_store = 0;

    op_ret = tuya_read_filter_id(&filter_ID_store);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_read_filter_id err:%d", op_ret);
        return op_ret;
    }

    /* If filter is new */
    if (filter_ID_store != g_filter_card_msg.ID) {
        PR_DEBUG("Detect a new filter");
        tuya_filter_reset(TRUE);
        tuya_write_filter_id(g_filter_card_msg.ID);
    }

    return op_ret;
}

/**
* @brief: tuya_analyze_CardData_to_get_filter_msg
* @desc: analyze CardData to get filter msg
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_analyze_CardData_to_get_filter_msg(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    UCHAR_T i = 0;

    /* check filter_buf */
    if (NULL == filter_buf) {
        PR_ERR("filter_buf is error");
        return OPRT_INVALID_PARM;
    }

    /* printf filter buf */
    PR_DEBUG_RAW("filter buf data: ");
    for (i = 0; i < CARD_READ_BYTE_LEN; i++) {
        PR_DEBUG_RAW("%02X ", filter_buf[i]);
    }
    PR_DEBUG_RAW("\n");

    /* check if filter is piratic */
    if (CARD_READ_HEAD_TAG != filter_buf[0]) {
        PR_INFO("Do not have tuya tag, filter is piratic");
        return OPRT_COM_ERROR;
    }

    /* get filter type */
    switch (filter_buf[1]) {
    case FILTER_STANDARD:
        g_filter_card_msg.type = FILTER_STANDARD;
        PR_DEBUG_RAW("filter type: standard\n");
        break;
    case FILTER_ANTIBACTERIAL:
        g_filter_card_msg.type = FILTER_ANTIBACTERIAL;
        PR_DEBUG_RAW("filter type: antibacterial\n");
        break;
    case FILTER_ALDEHYDE_REMOVAL:
        g_filter_card_msg.type = FILTER_ALDEHYDE_REMOVAL;
        PR_DEBUG_RAW("filter type: aldehyde removal\n");
        break;
    case FILTER_PROFESSIONAL:
        g_filter_card_msg.type = FILTER_PROFESSIONAL;
        PR_DEBUG_RAW("filter type: professional\n");
        break;
    default:
        PR_INFO("filter type buf data is erro, maybe filter is piratic");
        return OPRT_COM_ERROR;
        break;
    }
    
    /* get filter ID */
    g_filter_card_msg.ID = filter_buf[2] * 256 + filter_buf[3];
    PR_DEBUG_RAW("filter ID: %d\n", g_filter_card_msg.ID);

    /* get filter product date */
    g_filter_card_msg.product_date.year  = filter_buf[4] + 2000;
    g_filter_card_msg.product_date.month = filter_buf[5];
    g_filter_card_msg.product_date.day   = filter_buf[6];
    PR_DEBUG_RAW("filter product date: %d-%d-%d\n", filter_buf[4]+2000, filter_buf[5], filter_buf[6]);

    /* DP report */
    tuya_update_single_dp(FILTER_TYPE_DPID, PROP_ENUM, g_filter_card_msg.type);
    /* check whether the filter is new */
    tuya_check_whether_the_filter_is_new();

    return op_ret;
}

/**
* @brief: tuya_nfc_detect_filter
* @desc: nfc detect to read filter message
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_nfc_detect_filter(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    UCHAR_T i = 0;
    UCHAR_T page = 0;
    UCHAR_T read_buf[16] = {0};
    UCHAR_T TagType[2];
    UCHAR_T SelectedSnr[4];

    /* mfrc522 reset, and open antenna */
    PcdReset();
	PcdAntennaOff(); 
	PcdAntennaOn(); 

    /* look for card */
    op_ret = PcdRequest(0x52, TagType);
    if (OPRT_OK != op_ret) {
        PR_INFO("PcdRequest err:%d", op_ret);
        PR_INFO("Do not find NFC Card, filter do not install");
        return op_ret;
    }

    /* printf the type of card */
    PR_DEBUG_RAW("TagType: 0x");
    for (i = 0; i < 2; i++) {
        PR_DEBUG_RAW("%02X", TagType[i]);
    }
    PR_DEBUG_RAW("\n");

    /* card collision protection */
    op_ret = PcdAnticoll(SelectedSnr);
    if (OPRT_OK != op_ret) {
        PR_INFO("PcdAnticoll err:%d", op_ret);
        PR_INFO("Do not find NFC Card, filter do not install");
        return op_ret;
    }

    /* select card */
    op_ret = PcdSelect(SelectedSnr);
    if (OPRT_OK != op_ret) {
        PR_INFO("PcdSelect err:%d", op_ret);
        PR_INFO("Do not find NFC Card, filter do not install");
        return op_ret;
    }

    /* read card */
    PR_DEBUG("Find NFC Card, the filter is install ok");
    PR_DEBUG_RAW("Read NFC Card data:\n");
    for (page = 0; page < 45; page +=4) {
        //read
        op_ret = PcdRead(page, read_buf);
        if (OPRT_OK != op_ret) {
            PR_ERR("PcdRead err:%d, page:%d~%d", op_ret, page, (page + 3));
        }
        //get filter message
        if (CARD_READ_PAGE_ADDR == page) {
            memset(filter_buf, 0x00, CARD_READ_BYTE_LEN);
            for (i = 0; i < CARD_READ_BYTE_LEN; i++) {
                filter_buf[i] = *(read_buf + i);
            }
        }
        //printf read buf
        PR_DEBUG_RAW("  ");
        for (i = 0; i < 16; i++) {
            PR_DEBUG_RAW("%02X ", read_buf[i]);
        }
        PR_DEBUG_RAW("----- page: %d~%d\n", page, (page + 3));     
    }
    PR_DEBUG_RAW("\n");

    op_ret = tuya_analyze_CardData_to_get_filter_msg();
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    return op_ret;
}

/**
* @brief: tuya_check_door_and_filter_once
* @desc: check door and filter once
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_check_door_and_filter_once(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* check whether the door is open */
    if (TRUE == DOOR_OPEN_FLAG) {
        PR_INFO("The door is not close, air cleaner can not run");
        tuya_update_single_dp(FAULT_DPID, PROP_BITMAP, FUALT_DOOR_OPEN);
        return OPRT_COM_ERROR;
    }

    /* check filter nfc card */
    op_ret = tuya_nfc_detect_filter();
    if (OPRT_OK != op_ret) {
        PR_INFO("air cleaner can not run");
        tuya_update_single_dp(FAULT_DPID, PROP_BITMAP, FUALT_FILTER_WRONG);
        return OPRT_COM_ERROR;
    }

    /* air cleaner motor resume normal work */
    PR_DEBUG("check_door_and_filter, it is ok");
    tuya_fault_to_stop_motor(FALSE);
    tuya_update_single_dp(FAULT_DPID, PROP_BITMAP, 0x00);

    return op_ret;
}

/**
* @brief: tuya_check_timer_cb
* @desc: check timer callback function
* 
* @return none
*/
STATIC VOID_T tuya_check_timer_cb()
{
    OPERATE_RET op_ret = OPRT_OK;

    if (POWER_OFF == tuya_get_power_status()) {
        sys_stop_timer(timer_check_door_filter);
        return;
    }

    op_ret = tuya_check_door_and_filter_once();
    if (OPRT_OK == op_ret) {
        tuya_recovery_from_fault();
        sys_stop_timer(timer_check_door_filter);
    } 

    return;
}

/** 
* @brief: tuya_check_timer_start
* @desc: start timer, check door filter, cycle count 5s
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
STATIC OPERATE_RET tuya_check_timer_start(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = sys_add_timer(tuya_check_timer_cb, NULL, &timer_check_door_filter);
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_add_timer error:%d", op_ret);
        return op_ret;
    } 

    op_ret = sys_start_timer(timer_check_door_filter, CHECK_DOOR_CYCLE_TIME, TIMER_CYCLE); //Start the timer, cycle count 5s
    if (op_ret != OPRT_OK) {
        PR_ERR("sys_start_timer error:%d", op_ret);
        return op_ret;
    }  

    return op_ret;
}

/**
* @brief: tuya_check_door_and_filter
* @desc: check door and filter
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_check_door_and_filter(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = tuya_check_door_and_filter_once();
    if (OPRT_OK != op_ret) {
        tuya_fault_to_stop_motor(TRUE);

        tuya_hal_system_sleep(BUZ_ALARM_DELAY_TIME);
        tuya_buz_start(SOUND_ALARM);
        
        tuya_check_timer_start();
        return OPRT_OK;
    } else {
        return op_ret;
    }

    return op_ret;
}

/**
* @brief: tuya_filter_type_dp_updata
* @desc: updata filter_type dp value
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_filter_type_dp_updata(VOID_T)
{
    UINT_T filter_ID_store = 0;

    tuya_read_filter_id(&filter_ID_store);
    tuya_update_single_dp(FILTER_TYPE_DPID, PROP_ENUM, filter_ID_store);

    return OPRT_OK;
}

/**
* @brief: tuya_door_sen_pin_irq_cb
* @desc: door sen pin callback function
* 
* @return none
*/
VOID_T tuya_door_sen_pin_irq_cb(VOID_T *args)
{
    tuya_os_adapt_semaphore_post(p_check_semaphore_handle);
    return;
}

/**
* @brief: tuya_check_door_and_filter_task_cb
* @desc: check door and filter task thread start cb
* 
* @return none
*/
STATIC VOID_T tuya_check_door_and_filter_task_cb(VOID_T)
{
    while(1)
    {
        tuya_os_adapt_semaphore_wait(p_check_semaphore_handle);

        tuya_hal_system_sleep(10); 

        if (POWER_ON == tuya_get_power_status()) {
            if (TRUE == DOOR_OPEN_FLAG) {
                tuya_fault_to_stop_motor(TRUE);
                tuya_buz_start(SOUND_ALARM);
                tuya_check_timer_start();
                tuya_update_single_dp(FAULT_DPID, PROP_BITMAP, FUALT_DOOR_OPEN);
                PR_INFO("The door is not close, air cleaner can not run");
            }
        }

        tuya_hal_system_sleep(3000); 
    }
}

/**
* @brief: tuya_check_door_and_filter_task_init
* @desc: check_door_and_filter task thread start
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_check_door_and_filter_task_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    
    THRD_HANDLE ty_user_handle = NULL;
    THRD_PARAM_S thrd_param;
    thrd_param.priority = TRD_PRIO_5; 
    thrd_param.stackDepth = 515*4;
    thrd_param.thrdname = "ty_check_door_and_filter_task";
    op_ret = CreateAndStart(&ty_user_handle, NULL, NULL, tuya_check_door_and_filter_task_cb, NULL, &thrd_param);      
    if(op_ret != OPRT_OK) {
        PR_ERR("start tuya_check_door_and_filter_task_cb ty_task_cb err");
        return op_ret;
    }

    op_ret = tuya_os_adapt_semaphore_create_init(&p_check_semaphore_handle, 0, 1);  
    if(op_ret != OPRT_OK) {
        PR_ERR("tuya_os_adapt_semaphore_create_init err");
        return op_ret;
    }
    
    PR_NOTICE("tuya_check_door_and_filter_task_init success !!!");
    return op_ret;
}
