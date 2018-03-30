#pragma once

#include "Application.h"
#include "PinScanner.h"


class ScannerDisplayApp : public Application {
public:
	ScannerDisplayApp();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	string getName() { return "Scanner Display Application"; };

	PinScanner* scannerPins[INFORCE_COLS][INFORCE_ROWS];

	float variance = 1; // 0.0000000000001;

	//void drawApp();

private:
	float time = 0.0;

	// pin motion control
	float kp = 100; // 500
	float kd = 100;
	float lastNetForce[INFORCE_COLS][INFORCE_ROWS];
	float speedGoingUp = 2.0;

	//float heightsForShapeDisplayBuffered[INFORCE_COLS + 2][INFORCE_ROWS + 2]; //heightForShapeDisplayBuffered[i+1][j+1]=heightForShapeDisplay[i][j]
	//float heightsFromShapeDisplayBuffered[INFORCE_COLS + 2][INFORCE_ROWS + 2]; //heightForShapeDisplayBuffered[i+1][j+1]=heightForShapeDisplay[i][j]


	float getTargetForce(int height);
	float getTargetForce2(int height);
	float getTargetForce3(int height);
	float getTargetForce4(int height);
	float getTargetForce5(int height, int x, int y);

	float getTargetFrequency(int height);

	int32_t minHeight = 10000;
	int32_t maxHeight = 80000;

	//visualization
	ofEasyCam cam;
};