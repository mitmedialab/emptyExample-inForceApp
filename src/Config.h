//
//  Config.h
//  inForce_openFrameworks
//
//  Created by Daniel Fitzgerald on 8/25/16.
//
//

#ifndef Config_h
#define Config_h

#include "ofMain.h"
//test
#define SERIAL_BAUDRATE  1000000//115200

#define FRAMES_PER_SECOND 200

// Display dimensions - physical
#define INFORCE_COLS 10
#define INFORCE_ROWS 5
#define INFORCE_NUM_PINS (INFORCE_COLS * INFORCE_ROWS)

// Distribution of embedded control - see InforceInterface.cpp for logic on mapping pins <-> teenys
#define PINS_PER_TEENSY (INFORCE_COLS/2)
#define NUM_TEENSYS (INFORCE_ROWS*2)

#define PIN_POSITION_MAX_UM 120000  //uint32 - Motors actually have 120mm of travel, but we'll limit themn to 100mm for safety and to leave room for calibration
#define PIN_FORCE_MAX_MN 4448       //uint16 - derived approximately from empirical motor testing

// Physical properties of pins
#define PIN_WIDTH_MM 18
#define PIN_HEIGHT_MM 120
#define PIN_SPACING_MM 20
#define DISPLAY_HEIGHT_MM 800

// Maximum Phyxel forcemap depth
#define PHYXEL_DEPTH 256

// Working pin state datatypes
typedef int32_t HeightUM;
typedef int32_t ForceMN;
typedef char TouchState;
typedef struct {
    HeightUM height;
    ForceMN force;
    TouchState touch;
} PinState;
const size_t BYTES_PER_PIN_STATE_MESSAGE =  sizeof(HeightUM) + sizeof(ForceMN) + sizeof(TouchState); //(sizeof(Position) + sizeof(Force) + sizeof(bool)))
const size_t BYTES_PER_PIN_STATE_MESSAGE_FRAME = PINS_PER_TEENSY * BYTES_PER_PIN_STATE_MESSAGE;

typedef struct {
    PinState pins[INFORCE_COLS][INFORCE_ROWS];
} DisplayState;

// Commands
enum ArduinoUpdateMessageType{DEBUG_MESSAGE = 'D', PIN_STATE_UPDATE = 'U', PIN_CALIBRATION_UPDATE = 'C', PIN_SCAN_UPDATE = 'S'};    // Any message received by any arduino should begin with a byte with one of these values to indicate the type of the following message

enum OperationMode { HeightDependentForce = 1 }; 

enum ForceDisplayCommandType {FORCE_CONTROL = 'F', HEIGHT_CONTROL = 'P', CALIBRATION = 'C', SCANNING = 'S', SLEEP = 'Z', SHUTDOWN = 'D'}; // NOTE: This should be identical to the Mode enum in Config.h for the embedded code!

// OSC ports
#define OUT_PORT 12346
#define IN_PORT 12345

#define NUM_MSG_STRINGS 150

// thread frequencies
#define COMMAND_FREQUENCY 60

#define PARAM_FOR_OPERATION_PIN_CALIBRATION_DATA 26214442
#define MAX_OFXOSCMESSAGE_LENGTH 400 //200

extern string globalDebugString;

#endif /* Config_h */
