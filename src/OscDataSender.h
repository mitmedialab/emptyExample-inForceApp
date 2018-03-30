#pragma once
#include "Config.h"
#include "ofxOsc.h"
#include <map>


class OscDataSender {
public:
	OscDataSender();

	

	void sendHeightData(int32_t heightForShapeDisplay[INFORCE_COLS][INFORCE_ROWS]);
	void sendPinCalibrationParameter();
	void sendOperationParameter(int32_t data[], int numberOfData);
	void switchOperationMode(int32_t i);
	void sendBackAllPinHeight();


	void sendAllSameOperationParameter(int32_t data[], int numberOfData);
	void sendOperationParameterToSingleArduino(int32_t data[], int numberOfData, int arduinoAddress);


	void sendHeightDependentForceParameters(int pinI, int pinJ, int maxHeight, int minHeight, int heightMapping[], int forceMapping[], int numMappings);


private:
	ofxOscSender sender;


};