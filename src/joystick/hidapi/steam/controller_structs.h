/*
  Simple DirectMedia Layer
  Copyright (C) 2020 Valve Corporation

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#ifndef _CONTROLLER_STRUCTS_
#define _CONTROLLER_STRUCTS_

#pragma pack(1)

#define HID_FEATURE_REPORT_BYTES 64

// Header for all host <==> target messages
typedef struct
{
	unsigned char type;
	unsigned char length;
} FeatureReportHeader;

// Generic controller attribute structure
typedef struct
{
	unsigned char attributeTag;
	uint32_t attributeValue;
} ControllerAttribute;

// Generic controller settings structure
typedef struct
{
	ControllerAttribute attributes[ ( HID_FEATURE_REPORT_BYTES - sizeof( FeatureReportHeader ) ) / sizeof( ControllerAttribute ) ];
} MsgGetAttributes;

// 16bit Steam Deck register with address
typedef struct
{
	uint8_t addr;
	uint16_t val;
} WriteDeckRegister;

// Generic Steam Deck write register message
typedef struct
{
	WriteDeckRegister reg[ (HID_FEATURE_REPORT_BYTES - sizeof ( FeatureReportHeader ) ) / sizeof (WriteDeckRegister ) ];
} MsgWriteDeckRegister;


// This is the only message struct that application code should use to interact with feature request messages. Any new
// messages should be added to the union. The structures defined here should correspond to the ones defined in
// ValveDeviceCore.cpp.
//
typedef struct
{
	FeatureReportHeader header;
	union
	{
		MsgGetAttributes				getAttributes;
		MsgWriteDeckRegister			wrDeckRegister;
	} payload;

} FeatureReportMsg;

// Roll this version forward anytime that you are breaking compatibility of existing
// message types within ValveInReport_t or the header itself.  Hopefully this should
// be super rare and instead you should just add new message payloads to the union,
// or just add fields to the end of existing payload structs which is expected to be 
// safe in all code consuming these as they should just consume/copy upto the prior size 
// they were aware of when processing.
#define k_ValveInReportMsgVersion 0x01

typedef enum
{
	ID_CONTROLLER_STATE = 1,
	ID_CONTROLLER_DEBUG = 2,
	ID_CONTROLLER_WIRELESS = 3,
	ID_CONTROLLER_STATUS = 4,
	ID_CONTROLLER_DEBUG2 = 5,
	ID_CONTROLLER_SECONDARY_STATE = 6,
	ID_CONTROLLER_BLE_STATE = 7,
	ID_CONTROLLER_DECK_STATE = 9,
	ID_CONTROLLER_MSG_COUNT
} ValveInReportMessageIDs; 

typedef struct 
{
	unsigned short unReportVersion;
	
	unsigned char ucType;
	unsigned char ucLength;
	
} ValveInReportHeader_t;

// State payload
typedef struct 
{
	// If packet num matches that on your prior call, then the controller state hasn't been changed since 
	// your last call and there is no need to process it
	Uint32 unPacketNum;
	
	// Button bitmask and trigger data.
	union
	{
		Uint64 ulButtons;
		struct
		{
			unsigned char _pad0[3];
			unsigned char nLeft;
			unsigned char nRight;
			unsigned char _pad1[3];
		} Triggers;
	} ButtonTriggerData;
	
	// Left pad coordinates
	short sLeftPadX;
	short sLeftPadY;
	
	// Right pad coordinates
	short sRightPadX;
	short sRightPadY;
	
	// This is redundant, packed above, but still sent over wired
	unsigned short sTriggerL;
	unsigned short sTriggerR;

	// FIXME figure out a way to grab this stuff over wireless
	short sAccelX;
	short sAccelY;
	short sAccelZ;
	
	short sGyroX;
	short sGyroY;
	short sGyroZ;
	
	short sGyroQuatW;
	short sGyroQuatX;
	short sGyroQuatY;
	short sGyroQuatZ;

} ValveControllerStatePacket_t;

// BLE State payload this has to be re-formatted from the normal state because BLE controller shows up as 
//a HID device and we don't want to send all the optional parts of the message. Keep in sync with struct above.
typedef struct
{
	// If packet num matches that on your prior call, then the controller state hasn't been changed since 
	// your last call and there is no need to process it
	Uint32 unPacketNum;

	// Button bitmask and trigger data.
	union
	{
		Uint64 ulButtons;
		struct
		{
			unsigned char _pad0[3];
			unsigned char nLeft;
			unsigned char nRight;
			unsigned char _pad1[3];
		} Triggers;
	} ButtonTriggerData;

	// Left pad coordinates
	short sLeftPadX;
	short sLeftPadY;

	// Right pad coordinates
	short sRightPadX;
	short sRightPadY;

	//This mimcs how the dongle reconstitutes HID packets, there will be 0-4 shorts depending on gyro mode
	unsigned char ucGyroDataType; //TODO could maybe find some unused bits in the button field for this info (is only 2bits)
	short sGyro[4];

} ValveControllerBLEStatePacket_t;

// Define a payload for reporting debug information
typedef struct
{
	// Left pad coordinates
	short sLeftPadX;
	short sLeftPadY;

	// Right pad coordinates
	short sRightPadX;
	short sRightPadY;

	// Left mouse deltas
	short sLeftPadMouseDX;
	short sLeftPadMouseDY;

	// Right mouse deltas
	short sRightPadMouseDX;
	short sRightPadMouseDY;
	
	// Left mouse filtered deltas
	short sLeftPadMouseFilteredDX;
	short sLeftPadMouseFilteredDY;

	// Right mouse filtered deltas
	short sRightPadMouseFilteredDX;
	short sRightPadMouseFilteredDY;
	
	// Pad Z values
	unsigned char ucLeftZ;
	unsigned char ucRightZ;
	
	// FingerPresent
	unsigned char ucLeftFingerPresent;
	unsigned char ucRightFingerPresent;
	
	// Timestamps
	unsigned char ucLeftTimestamp;
	unsigned char ucRightTimestamp;
	
	// Double tap state
	unsigned char ucLeftTapState;
	unsigned char ucRightTapState;
	
	unsigned int unDigitalIOStates0;
	unsigned int unDigitalIOStates1;
	
} ValveControllerDebugPacket_t;

typedef struct
{
	unsigned char ucPadNum;
	unsigned char ucPad[3]; // need Data to be word aligned
	short Data[20];
	unsigned short unNoise;
} ValveControllerTrackpadImage_t;

typedef struct
{
	unsigned char ucPadNum;
	unsigned char ucOffset;
	unsigned char ucPad[2]; // need Data to be word aligned
	short rgData[28];
} ValveControllerRawTrackpadImage_t;

// Payload for wireless metadata
typedef struct 
{
	unsigned char ucEventType;
} SteamControllerWirelessEvent_t;

typedef struct 
{
	// Current packet number.
    unsigned int unPacketNum;
	
	// Event codes and state information.
    unsigned short sEventCode;
    unsigned short unStateFlags;

    // Current battery voltage (mV).
    unsigned short sBatteryVoltage;
	
	// Current battery level (0-100).
	unsigned char ucBatteryLevel;
} SteamControllerStatusEvent_t;

// Deck State payload
typedef struct
{
	// If packet num matches that on your prior call, then the controller
	// state hasn't been changed since your last call and there is no need to
	// process it
	Uint32 unPacketNum;

	// Button bitmask and trigger data.
	union
	{
		Uint64 ulButtons;
		struct
		{
			Uint32 ulButtonsL;
			Uint32 ulButtonsH;
		};
	};

	// Left pad coordinates
	short sLeftPadX;
	short sLeftPadY;

	// Right pad coordinates
	short sRightPadX;
	short sRightPadY;

	// Accelerometer values
	short sAccelX;
	short sAccelY;
	short sAccelZ;

	// Gyroscope values
	short sGyroX;
	short sGyroY;
	short sGyroZ;

	// Gyro quaternions
	short sGyroQuatW;
	short sGyroQuatX;
	short sGyroQuatY;
	short sGyroQuatZ;

	// Uncalibrated trigger values
	short sLeftTrigger;
	short sRightTrigger;

	// Left stick values
	short sLeftStickX;
	short sLeftStickY;

	// Right stick values
	short sRightStickX;
	short sRightStickY;

	// Touchpad pressures
	short sLeftPadPressure;
	short sRightPadPressure;
} SteamDeckStatePacket_t;

typedef struct
{
	ValveInReportHeader_t header;
	
	union
	{
		ValveControllerStatePacket_t controllerState;
		ValveControllerBLEStatePacket_t controllerBLEState;
		ValveControllerDebugPacket_t debugState;
		ValveControllerTrackpadImage_t padImage;
		ValveControllerRawTrackpadImage_t rawPadImage;
		SteamControllerWirelessEvent_t wirelessEvent;
		SteamControllerStatusEvent_t statusEvent;
		SteamDeckStatePacket_t deckState;
	} payload;
	
} ValveInReport_t;


// Enumeration for BLE packet protocol
enum EBLEPacketReportNums
{
	// Skipping past 2-3 because they are escape characters in Uart protocol
	k_EBLEReportState = 4,
	k_EBLEReportStatus = 5,
};


// Enumeration of data chunks in BLE state packets
enum EBLEOptionDataChunksBitmask
{
	// First byte upper nibble
	k_EBLEButtonChunk1 = 0x10,
	k_EBLEButtonChunk2 = 0x20,
	k_EBLEButtonChunk3 = 0x40,
	k_EBLELeftJoystickChunk = 0x80,

	// Second full byte
	k_EBLELeftTrackpadChunk = 0x100,
	k_EBLERightTrackpadChunk = 0x200,
	k_EBLEIMUAccelChunk = 0x400,
	k_EBLEIMUGyroChunk = 0x800,
	k_EBLEIMUQuatChunk = 0x1000,
};

#pragma pack()

#endif // _CONTROLLER_STRUCTS
