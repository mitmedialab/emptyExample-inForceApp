#pragma once

#include "Application.h"
#include "PinScanner.h"
#include "Config.h"




class MaterialScannerApp : public Application {
public:
	MaterialScannerApp();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	void drawApp();


	string getName() { return "Material Scanner Application"; };

	PinScanner* scannerPin;
	int scannerI = 5;
	int scannerJ = 2;


	bool scanning = false;

	void resetScan();

	void scan();

	void writeToFile(string filename);

	OscDataSender dataSender;

private:
	float time = 0.0;

	int32_t minHeight = 10000;
	int32_t maxHeight = 80000;



};