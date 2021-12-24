/**
* @file tuya_mfrc522_app.c
* @author ali.zhang@tuya.com
* @brief tuya_mfrc522_app module is used to dirver mfrc522
* @version 1.0.0
* @date 2021-10-28
*
* @copyright Copyright (c) tuya.inc 2021
*
*/
 
/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_mfrc522_app.h"
#include "uni_log.h"
 
/*********************************************************************
******************************micro define****************************
*********************************************************************/
#define MAXRLEN 18
 
/*********************************************************************
****************************typedef define****************************
*********************************************************************/
 
 
/*********************************************************************
****************************variable define***************************
*********************************************************************/
STATIC VOID_T _delay_10ms(IN CONST UINT_T _10ms);
 
/*********************************************************************
****************************function define***************************
*********************************************************************/
/**
* @brief: tuya_mfrc522_app_init
* @desc: mfrc522 app initialize
* 
* @return OPERATE_RET  OPRT_OK is sucess, other is fail
*/
OPERATE_RET tuya_mfrc522_app_init(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = tuya_mfrc522_pin_init();
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_mfrc522_pin_init err:%d", op_ret);
        return op_ret;
    }

    return op_ret;
}

/**
* @brief: PcdRequest
* @desc: Sends an Request Command
* @param[in] req_code: Request mode
* @ref            0x52 = Find all cards in the zone conforming to 14443A standard
* @ref            0x26 = Find the card that has not entered hibernation 
* @param[out] pTagType: The card type
* @ref            0x4400 = Mifare_UltraLight
* @ref            0x0400 = Mifare_One(S50)
* @ref            0x0200 = Mifare_One(S70)
* @ref            0x0800 = Mifare_Pro(X)
* @ref            0x4403 = Mifare_DESFire     
* @return OPERATE_RET
*/
OPERATE_RET PcdRequest(IN UCHAR_T req_code, OUT UCHAR_T *pTagType)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T  unLen;
    UCHAR_T ucComMF522Buf[MAXRLEN]; 

    ClearBitMask(Status2Reg, 0x08);
    WriteRawRC(BitFramingReg, 0x07);
    SetBitMask(TxControlReg, 0x03);
 
    ucComMF522Buf[0] = req_code;

    op_ret = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &unLen);
    if ((op_ret == OPRT_OK) && (unLen == 0x10)) {    
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
    } else {
        op_ret = OPRT_COM_ERROR;
    }
   
    return op_ret;
}

/**
* @brief: PcdAnticoll
* @desc: Card collision protection
* @param[out] pSnr: Card serial number, 4 bytes
* @return OPERATE_RET
*/
OPERATE_RET PcdAnticoll(OUT UCHAR_T *pSnr)
{
    OPERATE_RET op_ret = OPRT_OK;
    UCHAR_T i, snr_check = 0;
    UINT_T  unLen;
    UCHAR_T ucComMF522Buf[MAXRLEN]; 
    
    ClearBitMask(Status2Reg, 0x08);
    WriteRawRC(BitFramingReg, 0x00);
    ClearBitMask(CollReg, 0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    op_ret = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, &unLen);
    if (op_ret == OPRT_OK) {
    	for (i = 0; i < 4; i++) {
            *(pSnr+i)  = ucComMF522Buf[i];
            snr_check ^= ucComMF522Buf[i];
        }
        if (snr_check != ucComMF522Buf[i]) {
            op_ret = OPRT_COM_ERROR;
        }
    }
    
    SetBitMask(CollReg, 0x80);
    return op_ret;
}

/**
* @brief: PcdSelect
* @desc: send commands to select a single Card
* @param[in] pSnr: Card serial number, 4 bytes
* @return OPERATE_RET
*/
OPERATE_RET PcdSelect(IN UCHAR_T *pSnr)
{
    OPERATE_RET op_ret = OPRT_OK;
    UCHAR_T i;
    UINT_T  unLen;
    UCHAR_T ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    
    for (i = 0; i < 4; i++) {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    
    CalulateCRC(ucComMF522Buf, 7, &ucComMF522Buf[7]);
    ClearBitMask(Status2Reg, 0x08);

    op_ret = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, &unLen);
    if ((op_ret == OPRT_OK) && (unLen == 0x18)) {
        op_ret = OPRT_OK;
    } else {
        op_ret = OPRT_COM_ERROR;
    }

    return op_ret;
}

/**
* @brief: PcdAuthState
* @desc: Authentication to enable a secure communication to card
* @param[in] auth_mode:
* @ref            0x60 = authenticate A key
* @ref            0x61 = authenticate B key
* @param[in] addr: block address
* @param[in] pKey: key
* @param[in] pSnr: card serial number, 4 bytes
* @return OPERATE_RET
*/            
OPERATE_RET PcdAuthState(IN UCHAR_T auth_mode, IN UCHAR_T addr, IN UCHAR_T *pKey, IN UCHAR_T *pSnr)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T  unLen;
    UCHAR_T i, ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    
    for (i = 0; i < 6; i++) {
        ucComMF522Buf[i + 2] = *(pKey + i);
    }
    for (i = 0; i < 6; i++) {
        ucComMF522Buf[i+8] = *(pSnr+i);
    }

    op_ret = PcdComMF522(PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, &unLen);
    if ((op_ret != OPRT_OK) || (!(ReadRawRC(Status2Reg) & 0x08))) {
        op_ret = OPRT_COM_ERROR;
    }
    
    return op_ret;
}

/**
* @brief: PcdRead
* @desc: Read 16 bytes from the card
* @param[in] addr: block address
* @param[out] pData: read data
* @return OPERATE_RET
*/           
OPERATE_RET PcdRead(IN UCHAR_T addr, OUT UCHAR_T *pData)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T  unLen;
    UCHAR_T i, ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    
    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);
   
    op_ret = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((op_ret == OPRT_OK) && (unLen == 0x90)) {
        for (i = 0; i < 16; i++) {
            *(pData+i) = ucComMF522Buf[i];
        }
    } else {
        op_ret = OPRT_COM_ERROR;
    }
    
    return op_ret;
}
 
/**
* @brief: PcdWrite
* @desc: write 16 bytes data in the card
* @param[in] addr: block address
* @param[in] pData: write data
* @return OPERATE_RET
*/                         
OPERATE_RET PcdWrite(IN UCHAR_T addr, IN UCHAR_T *pData)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T  unLen;
    UCHAR_T i, ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);
 
    op_ret = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);
    if ((op_ret != OPRT_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) {   
        op_ret = OPRT_COM_ERROR;
        PR_ERR("PcdComMF522 err");
    }
        
    if (op_ret == OPRT_OK) {
        for (i = 0; i < 16; i++) {
            ucComMF522Buf[i] = *(pData+i);
        }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        op_ret = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, &unLen);
        if ((op_ret != OPRT_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) {
            op_ret = OPRT_COM_ERROR;
            PR_ERR("PcdComMF522 err");
        }
    }
    
    return op_ret;
}

/**
* @brief: PcdHalt
* @desc: Command card to into sleep
* @return OPERATE_RET
*/  
OPERATE_RET PcdHalt(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T  unLen;
    UCHAR_T ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);
 
    op_ret = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

    return OPRT_OK;
}

/**
* @brief: CalulateCRC
* @desc: Calculate CRC16 function with MF522
* @param[in] pIndata:
* @param[in] len:
* @param[out] pOutData:
* @return none
*/  
VOID_T CalulateCRC(IN UCHAR_T *pIndata, IN UCHAR_T len, OUT UCHAR_T *pOutData)
{
    UCHAR_T i, n;
    
    ClearBitMask(DivIrqReg, 0x04);
    WriteRawRC(CommandReg, PCD_IDLE);
    SetBitMask(FIFOLevelReg, 0x80);
    
    for (i=0; i<len; i++) {
        WriteRawRC(FIFODataReg, *(pIndata+i));
    }

    WriteRawRC(CommandReg, PCD_CALCCRC);

    i = 0xFF;
    do {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i != 0) && !(n & 0x04));

    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}

/**
* @brief: PcdReset
* @desc: Reset mfrc522
* @return OPERATE_RET
*/  
OPERATE_RET PcdReset(VOID_T)
{
#if USE_MFRC522_RST_PIN
    MF522_RST=1;
    _nop_();
    MF522_RST=0;
    _nop_();
    MF522_RST=1;
    _nop_();
#endif

    WriteRawRC(CommandReg, PCD_RESETPHASE);
    tuya_hal_system_sleep(10);
    
    WriteRawRC(ModeReg, 0x3D);  //Communicates with the Mifare card, CRC initial value 0x6363
    WriteRawRC(TReloadRegL, 30);           
    WriteRawRC(TReloadRegH, 0);
    WriteRawRC(TModeReg, 0x8D);
    WriteRawRC(TPrescalerReg, 0x3E);
    WriteRawRC(TxAutoReg, 0x40);

    return OPRT_OK;
}

/**
* @brief: M500PcdConfigISOType
* @desc: Set the working mode
* @param[in] type:
* @return OPERATE_RET
*/  
OPERATE_RET M500PcdConfigISOType(IN UCHAR_T type)
{
   if (type == 'A')                     //ISO14443_A
   { 
       ClearBitMask(Status2Reg,0x08);

 /*     WriteRawRC(CommandReg,0x20);    //as default   
       WriteRawRC(ComIEnReg,0x80);      //as default
       WriteRawRC(DivlEnReg,0x0);       //as default
	   WriteRawRC(ComIrqReg,0x04);      //as default
	   WriteRawRC(DivIrqReg,0x0);       //as default
	   WriteRawRC(Status2Reg,0x0);//80  //trun off temperature sensor
	   WriteRawRC(WaterLevelReg,0x08);  //as default
       WriteRawRC(ControlReg,0x20);     //as default
	   WriteRawRC(CollReg,0x80);        //as default
*/
       WriteRawRC(ModeReg,0x3D);//3F
/*	   WriteRawRC(TxModeReg,0x0);       //as default???
	   WriteRawRC(RxModeReg,0x0);       //as default???
	   WriteRawRC(TxControlReg,0x80);   //as default???

	   WriteRawRC(TxSelReg,0x10);       //as default???
   */
       WriteRawRC(RxSelReg,0x86);//84
 //      WriteRawRC(RxThresholdReg,0x84);//as default
 //      WriteRawRC(DemodReg,0x4D);      //as default

 //      WriteRawRC(ModWidthReg,0x13);   //26
       WriteRawRC(RFCfgReg,0x7F);        //4F
	/*   WriteRawRC(GsNReg,0x88);        //as default???
	   WriteRawRC(CWGsCfgReg,0x20);      //as default???
       WriteRawRC(ModGsCfgReg,0x20);     //as default???
*/
   	   WriteRawRC(TReloadRegL,30);//tmoLength);  //TReloadVal = 'h6a =tmoLength(dec) 
	   WriteRawRC(TReloadRegH,0);
       WriteRawRC(TModeReg,0x8D);
	   WriteRawRC(TPrescalerReg,0x3E);
	   
  //     PcdSetTmo(106);
	    _delay_10ms(1);
        PcdAntennaOn();
    } else {
        return OPRT_COM_ERROR;
    }
   
   return OPRT_OK;
}

/**
* @brief: SetBitMask
* @desc: Set mfrc522 register bit
* @param[in] reg: register address
* @param[in] mask: set value
* @return none
*/  
VOID_T SetBitMask(IN UCHAR_T reg, IN UCHAR_T mask)
{
    CHAR_T tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp | mask);  //set bit mask
}

/**
* @brief: ClearBitMask
* @desc: Clear mfrc522 register bit
* @param[in] reg: register address
* @param[in] mask: clear value
* @return none
*/
VOID_T ClearBitMask(IN UCHAR_T reg, IN UCHAR_T mask)
{
    CHAR_T tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  //clear bit mask
} 

/**
* @brief: PcdComMF522
* @desc: Through mfrc522, communication with Card
* @param[in] Command: mfrc522 command
* @param[in] pInData: send data
* @param[in] InLenByte: send data len of byte
* @param[out] pOutData: receive data
* @param[out] pOutLenBit: receive data len of bit
* @return OPERATE_RET
*/  
OPERATE_RET PcdComMF522(UCHAR_T Command, UCHAR_T *pInData, UCHAR_T InLenByte, UCHAR_T *pOutData, UINT_T *pOutLenBit)
{
    OPERATE_RET op_ret = OPRT_COM_ERROR;
    UCHAR_T irqEn = 0x00;
    UCHAR_T waitFor = 0x00;
    UCHAR_T lastBits;
    UCHAR_T n;
    UINT_T i;

    switch (Command) {
    case PCD_AUTHENT:
        irqEn   = 0x12;
        waitFor = 0x10;
        break;
    case PCD_TRANSCEIVE:
        irqEn   = 0x77;
        waitFor = 0x30;
        break;
    default:
        break;
    }
   
    WriteRawRC(ComIEnReg, irqEn | 0x80);
    ClearBitMask(ComIrqReg, 0x80);
    WriteRawRC(CommandReg, PCD_IDLE);
    SetBitMask(FIFOLevelReg, 0x80);
    
    for (i = 0; i < InLenByte; i++) {
        WriteRawRC(FIFODataReg, pInData[i]);
    }
    WriteRawRC(CommandReg, Command);
   
    if (Command == PCD_TRANSCEIVE) {
        SetBitMask(BitFramingReg, 0x80);
    }
    
    //i = 600; //According to the clock frequency adjustment, the maximum waiting time of operating M1 card is 25ms
    i = 2000;
    do {
        n = ReadRawRC(ComIrqReg);
        i--;
    }
    while ((i != 0) && !(n & 0x01) && !(n & waitFor));
    ClearBitMask(BitFramingReg,0x80);
	      
    if (i != 0) {    
        if (!(ReadRawRC(ErrorReg) & 0x1B)) {
            op_ret = OPRT_OK;
            if (n & irqEn & 0x01) {
                //PR_DEBUG("ComIrqReg, n:0x%02X", n);
                op_ret = OPRT_COM_ERROR;   
            }
            if (Command == PCD_TRANSCEIVE) {
               	n = ReadRawRC(FIFOLevelReg);
              	lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits) {
                    *pOutLenBit = (n-1)*8 + lastBits;
                } else {
                    *pOutLenBit = n*8;
                }
                if (n == 0) {
                    n = 1;
                }
                if (n > MAXRLEN) {
                    n = MAXRLEN;
                }
                for (i = 0; i < n; i++) {
                    pOutData[i] = ReadRawRC(FIFODataReg);
                }
            }
        } else {
            op_ret = OPRT_COM_ERROR;
        }  
    }
   
   SetBitMask(ControlReg, 0x80);  //stop timer now
   WriteRawRC(CommandReg, PCD_IDLE); 

   return op_ret;
}

/**
* @brief: PcdAntennaOn
* @desc: Open the antenna, there should be at least a 1ms interval between each open or close
* @return none
*/  
VOID_T PcdAntennaOn()
{
    UCHAR_T i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03)) {
        SetBitMask(TxControlReg, 0x03);
    }
}

/**
* @brief: PcdAntennaOff
* @desc: Close the antenna, there should be at least a 1ms interval between each open or close
* @return none
*/  
VOID_T PcdAntennaOff()
{
    ClearBitMask(TxControlReg, 0x03);
}

/**
* @brief: WaitCardOff
* @desc: Waiting for the card to leave
* @return none
*/  
VOID_T WaitCardOff(VOID_T)
{
	OPERATE_RET op_ret = OPRT_OK;
    UCHAR_T	TagType[2];

	while(1)
    {
		op_ret = PcdRequest(REQ_ALL, TagType);
		if (op_ret) {
			op_ret = PcdRequest(REQ_ALL, TagType);
			if (op_ret) {
				op_ret = PcdRequest(REQ_ALL, TagType);
				if (op_ret) {
					return;
				}
			}
		}
		_delay_10ms(100);
	}
}

/**
* @brief: _delay_10ms
* @desc: delay 10ms
* @return none
*/ 
STATIC VOID_T _delay_10ms(IN CONST UINT_T _10ms)
{
	UINT_T i, j;

	for (i = 0; i < _10ms; i++) {
		for(j = 0; j < 60000; j++);
	}
}