#include "ScannerDisplayAppArduino.h"
#include <iostream>
#include <fstream>

ScannerDisplayAppArduino::ScannerDisplayAppArduino() {
	dataSender.switchOperationMode(2);
	dataSender.sendPinCalibrationParameter();
	sendScanInformation("foamScan.txt");
}


void ScannerDisplayAppArduino::sendScanInformation(string filename) {


//	sendOperationParameterToSingleArduino(int32_t data[], int numberOfData, int arduinoAddress);

	//pinJ=4, pinI = 1 3 5 7

	int heightMapping[2];
	int forceMapping[2];
	heightMapping[0] = 30000;
	forceMapping[0] = 65;
	heightMapping[1] = 20000;
	forceMapping[1] = 1000;

	dataSender.sendHeightDependentForceParameters(1, 4, 30000, 20000, heightMapping, forceMapping, 2);
}

void ScannerDisplayAppArduino::update(float dt) {

}

string ScannerDisplayAppArduino::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void ScannerDisplayAppArduino::keyPressed(int key) {
	cout << "keyPressed" << endl;

}

