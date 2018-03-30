#pragma once

#include "Application.h"
#include <map>

class CalibrationApp : public Application {
public:
	CalibrationApp();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	string getName() { return "Calibration Application"; };

	OscDataSender dataSender;
	void updateOscSend();

private:
	void updateHeights(int height);

	int32_t minHeight = 0;
	int32_t maxHeight = 100000;

	
	// phase 0   = measuring current when pin is moving for different heights
	// phase 1-9 = measuring current when pin is at rest for different heights
	// phase 10  = process and write data
	// phase 11  = done
	int32_t phase = 0;
	float pause = 3.0f;

	int32_t heightsOfPins = 0;
	int32_t speed = 1000;
	int32_t goingUp = true;


	map<int, int> heightToForceRest[INFORCE_COLS][INFORCE_ROWS];
	map<int, int> heightToForceGoingUp[INFORCE_COLS][INFORCE_ROWS];
	map<int, int> heightToForceGoingDown[INFORCE_COLS][INFORCE_ROWS];

	map<int, int> heightToForceRestNum[INFORCE_COLS][INFORCE_ROWS];
	map<int, int> heightToForceGoingUpNum[INFORCE_COLS][INFORCE_ROWS];
	map<int, int> heightToForceGoingDownNum[INFORCE_COLS][INFORCE_ROWS];

	ofstream outputFile;

};