#pragma once

#include "Application.h"
#include "PinScanner.h"


class ScannerDisplayAppArduino : public Application {
public:
	ScannerDisplayAppArduino();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	string getName() { return "Scanner Display on Arduino Application"; };

	void sendScanInformation(string filename);

	OscDataSender dataSender;

private:
	float time = 0.0;


	//visualization
	ofEasyCam cam;
};