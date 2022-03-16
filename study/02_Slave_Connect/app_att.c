/********************************************************************************************************
 * @file     app_att.c 
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     Sep. 18, 2015
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "application/print/u_printf.h"

typedef struct
{
  /** Minimum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMin;
  /** Maximum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMax;
  /** Number of LL latency connection events (0x0000 - 0x03e8) */
  u16 latency;
  /** Connection Timeout (0x000A - 0x0C80 * 10 ms) */
  u16 timeout;
} gap_periConnectParams_t;

static const u16 clientCharacterCfgUUID = GATT_UUID_CLIENT_CHAR_CFG;

static const u16 extReportRefUUID = GATT_UUID_EXT_REPORT_REF;

static const u16 reportRefUUID = GATT_UUID_REPORT_REF;

static const u16 characterPresentFormatUUID = GATT_UUID_CHAR_PRESENT_FORMAT;

static const u16 userdesc_UUID	= GATT_UUID_CHAR_USER_DESC;

static const u16 serviceChangeUUID = GATT_UUID_SERVICE_CHANGE;

static const u16 my_primaryServiceUUID = GATT_UUID_PRIMARY_SERVICE;

static const u16 my_characterUUID = GATT_UUID_CHARACTER;

static const u16 my_devServiceUUID = SERVICE_UUID_DEVICE_INFORMATION;

static const u16 my_PnPUUID = CHARACTERISTIC_UUID_PNP_ID;

static const u16 my_devNameUUID = GATT_UUID_DEVICE_NAME;

static const u16 my_gapServiceUUID = SERVICE_UUID_GENERIC_ACCESS;

static const u16 my_appearanceUIID = GATT_UUID_APPEARANCE;

static const u16 my_periConnParamUUID = GATT_UUID_PERI_CONN_PARAM;

static const u16 my_appearance = GAP_APPEARE_UNKNOWN;

static const gap_periConnectParams_t my_periConnParameters = {20, 40, 0, 1000};

static const u16 my_gattServiceUUID = SERVICE_UUID_GENERIC_ATTRIBUTE;

static u16 serviceChangeVal[2] = {0};

static u8 serviceChangeCCC[2] = {0,0};

#define MY_DEV_NAME                        "Ai-Thinker"
extern  u8 ble_devName[];

static const u8 my_PnPtrs [] = {0x02, 0x8a, 0x24, 0x66, 0x82, 0x01, 0x00};

/* dungnt98 */
#define USER_UUID_LEN 16
#define USER_SPP_SERUUID     {0x55, 0xe4,0x05,0xd2,0xaf,0x9f,0xa9,0x8f,0xe5,0x4a,0x7d,0xfe,0x43,0x53,0x53,0X55}
#define USER_SPP_SERUUID_RX  0x16,0x96,0x24,0x47,0xc6,0x23, 0x61,0xba,0xd9,0x4b,0x4d,0x1e,0x43,0x53,0x53,0x49
#define USER_SPP_SERUUID_TX  0xb3,0x9b,0x72,0x34,0xbe,0xec, 0xd4,0xa8,0xf4,0x43,0x41,0x88,0x43,0x53,0x53,0x49

////////////////////// SPP ////////////////////////////////////
u8 TelinkSppServiceUUID[16]	      	    = USER_SPP_SERUUID;
u8 TelinkSppDataServer2ClientUUID_RX[16]      = {USER_SPP_SERUUID_RX};
u8 TelinkSppDataServer2ClientUUID_TX[16]      = {USER_SPP_SERUUID_TX};

static const u8 UserSppRxData_1[19] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(SPP_SERVER_TO_CLIENT_RX_DP_H), 
	U16_HI(SPP_SERVER_TO_CLIENT_RX_DP_H), 
	USER_SPP_SERUUID_RX
};
	
static const u8 UserSppTxData_1[19] = {
	 CHAR_PROP_WRITE_WITHOUT_RSP,
	U16_LO(SPP_SERVER_TO_CLIENT_TX_DP_H), 
	U16_HI(SPP_SERVER_TO_CLIENT_TX_DP_H), 
	USER_SPP_SERUUID_TX
};
/* end dungnt98 */

//////////////////////// OTA  ////////////////////////////////////////////////////
static const  u8 my_OtaUUID[16]					    = TELINK_SPP_DATA_OTA;
static const  u8 my_OtaServiceUUID[16]				= TELINK_OTA_UUID_SERVICE;
static u8 my_OtaData 						        = 0x00;
static const  u8 my_OtaName[] 						= {'O', 'T', 'A'};

////////////////////// SPP ////////////////////////////////////
// static const u8 TelinkSppServiceUUID[16]	      	    = TELINK_SPP_UUID_SERVICE;
// static const u8 TelinkSppDataServer2ClientUUID[16]      = TELINK_SPP_DATA_SERVER2CLIENT;
// static const u8 TelinkSppDataClient2ServerUUID[16]      = TELINK_SPP_DATA_CLIENT2SERVER;


// Spp data from Server to Client characteristic variables
static u8 SppDataServer2ClientDataCCC[2]  				= {0};

//this array will not used for sending data(directly calling HandleValueNotify API), so cut array length from 20 to 1, saving some SRAM

static u8 SppData_1[1] 					= {0};  //SppDataServer2ClientData[20]
// Spp data from Client to Server characteristic variables
//this array will not used for receiving data(data processed by Attribute Write CallBack function), so cut array length from 20 to 1, saving some SRAM
static u8 SppData_2[1] 					= {0};  //SppDataClient2ServerData[20]


//SPP data descriptor
static const u8 Telink_Descriptor_1[] 		 		= "Ai-Thinker SPP: Module<->Phone";

//// GAP attribute values
static const u8 my_devNameCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(GenericAccess_DeviceName_DP_H), U16_HI(GenericAccess_DeviceName_DP_H),
	U16_LO(GATT_UUID_DEVICE_NAME), U16_HI(GATT_UUID_DEVICE_NAME)
};
static const u8 my_appearanceCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(GenericAccess_Appearance_DP_H), U16_HI(GenericAccess_Appearance_DP_H),
	U16_LO(GATT_UUID_APPEARANCE), U16_HI(GATT_UUID_APPEARANCE)
};
static const u8 my_periConnParamCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(CONN_PARAM_DP_H), U16_HI(CONN_PARAM_DP_H),
	U16_LO(GATT_UUID_PERI_CONN_PARAM), U16_HI(GATT_UUID_PERI_CONN_PARAM)
};


//// GATT attribute values
static const u8 my_serviceChangeCharVal[5] = {
	CHAR_PROP_INDICATE,
	U16_LO(GenericAttribute_ServiceChanged_DP_H), U16_HI(GenericAttribute_ServiceChanged_DP_H),
	U16_LO(GATT_UUID_SERVICE_CHANGE), U16_HI(GATT_UUID_SERVICE_CHANGE)
};

int module_onReceiveData(void *p)
{
	rf_packet_att_data_t *pData = (rf_packet_att_data_t *)p;
	u8 len = pData->l2cap - 3;//有效数据长度

	char data[20];
	memset(data, 0, 20);
	memcpy(data, pData->dat, len);

	printf("%s\n",data);//透传模式，直接串口输出蓝牙数据
	at_print("On Read\n");//透传模式，直接串口输出蓝牙数据

	return 0;
}

int module_onWriteData(void *p)
{
	rf_packet_att_data_t *pData = (rf_packet_att_data_t *)p;
	u8 len = pData->l2cap - 3;//有效数据长度

	at_print("On Write\n");//透传模式，直接串口输出蓝牙数据
	return 0;
}

/* typedef struct attribute
{
  u16  attNum;
  u8   perm;
  u8   uuidLen;
  u32  attrLen;    //4 bytes aligned
  u8* uuid;
  u8* pAttrValue;
  att_readwrite_callback_t w;
  att_readwrite_callback_t r;
} attribute_t; */
static const attribute_t my_Attributes[] = {

	{ATT_END_H -1, 0,0,0,0,0},	// total num of attribute

	// 0001 - 0007  gap
	{3,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_devNameCharVal),(u8*)(&my_characterUUID), (u8*)(my_devNameCharVal), 0},
	{0,ATT_PERMISSIONS_READ,2,MAX_DEV_NAME_LEN, (u8*)(&my_devNameUUID), (u8*)(ble_devName), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_appearanceCharVal),(u8*)(&my_characterUUID), (u8*)(my_appearanceCharVal), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (u8*)(&my_appearanceUIID), 	(u8*)(&my_appearance), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_periConnParamCharVal),(u8*)(&my_characterUUID), (u8*)(my_periConnParamCharVal), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), 	(u8*)(&my_periConnParameters), 0},

	// 0008 - 000b gatt
	{4,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gattServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_serviceChangeCharVal),(u8*)(&my_characterUUID), (u8*)(my_serviceChangeCharVal), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (serviceChangeVal), (u8*)(&serviceChangeUUID), 	(u8*)(&serviceChangeVal), 0},
	{0,ATT_PERMISSIONS_RDWR,2,sizeof (serviceChangeCCC),(u8*)(&clientCharacterCfgUUID), (u8*)(serviceChangeCCC), 0},
	/* dungnt98 */
	
	// dungnt98 SPP
	{7,ATT_PERMISSIONS_READ,2,USER_UUID_LEN,(u8*)(&my_primaryServiceUUID), 	(u8*)(&TelinkSppServiceUUID), 0},
	// {0,ATT_PERMISSIONS_READ,2,sizeof(UserSppRxData_1),(u8*)(&my_characterUUID), 		(u8*)(UserSppRxData_1), 0},				//prop
	{0,ATT_PERMISSIONS_RDWR,USER_UUID_LEN,sizeof(SppData_1),(u8*)(&TelinkSppDataServer2ClientUUID_RX), (u8*)(SppData_1), 0},	//value
	// {0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&SppDataServer2ClientDataCCC)},
	{0,ATT_PERMISSIONS_READ,2,sizeof(UserSppTxData_1),(u8*)(&my_characterUUID), 		(u8*)(UserSppTxData_1), 0},				//prop
	{0,ATT_PERMISSIONS_RDWR,USER_UUID_LEN,sizeof(SppData_1),(u8*)(&TelinkSppDataServer2ClientUUID_TX), (u8*)(SppData_1), 
		(att_readwrite_callback_t)&module_onReceiveData, (att_readwrite_callback_t)&module_onWriteData},	//value
	// {0,ATT_PERMISSIONS_READ,2,sizeof(Telink_Descriptor_1),(u8*)&userdesc_UUID,(u8*)(&Telink_Descriptor_1)},
};

void my_att_init (void)
{
	bls_att_setAttributeTable ((u8 *)my_Attributes);

	u8 device_name[] = MY_DEV_NAME;
	bls_att_setDeviceName(device_name, sizeof(MY_DEV_NAME));
}