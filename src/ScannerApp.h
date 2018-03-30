#pragma once

#include "Application.h"
#include "PinScanner.h"
#include "Config.h"




class ScannerApp : public Application {
public:
	ScannerApp();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);
	void drawApp();

	string getName() { return "Scanner Application"; };

	PinScanner* scannerPins[INFORCE_COLS][INFORCE_ROWS];

	bool scanning = false;
	bool showScan = false;

	int numScanned();
	bool doneScanned();
	void resetScan();

	void scan();

	void writeToFile(string filename);

private:
	float time = 0.0;


	int32_t minHeight = 2000;
	int32_t maxHeight = 85000;


	//visualization
	ofEasyCam cam;


};