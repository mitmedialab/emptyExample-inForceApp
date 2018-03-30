#include "PianoAppArduino.h"
#include <iostream>
#include <fstream>

PianoAppArduino::PianoAppArduino() {
	dataSender.switchOperationMode(6);
	//dataSender.sendPinCalibrationParameter();
}

void PianoAppArduino::update(float dt) {
	updateOscSend();
}

void PianoAppArduino::updateOscSend() {
	dataSender.sendBackAllPinHeight();
}

string PianoAppArduino::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void PianoAppArduino::keyPressed(int key) {
	cout << "keyPressed" << endl;

}