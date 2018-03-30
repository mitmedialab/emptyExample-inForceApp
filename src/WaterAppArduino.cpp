#include "WaterAppArduino.h"
#include <iostream>
#include <fstream>

WaterAppArduino::WaterAppArduino() {
	dataSender.switchOperationMode(5);
	dataSender.sendPinCalibrationParameter();
}

void WaterAppArduino::update(float dt) {
	updateOscSend();
}

void WaterAppArduino::updateOscSend() {
	dataSender.sendBackAllPinHeight();
}

string WaterAppArduino::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void WaterAppArduino::keyPressed(int key) {
	cout << "keyPressed" << endl;



}