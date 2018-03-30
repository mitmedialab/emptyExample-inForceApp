#pragma once

#include "Application.h"
#include "PinModel.h"

class TouchTestApp : public Application {
public:
	TouchTestApp();

	void update(float dt);
	float getForce(int height);
	float getForce2(int height);
	float getForce3(int height);

	string appInstructionsText();
	void keyPressed(int key);

	string getName() { return "Touch Testing Application"; };

private:

	float calibrationTime = 12.0;

	int32_t minHeight = 1000;
	int32_t maxHeight = 60000;

	int32_t defaultHeight = 80000;

	float minForce = 50;
	float maxForce = 1500;

	float speedConstant = 500.0;

	int32_t forceFromShapeDisplayEMA[INFORCE_COLS][INFORCE_ROWS];

	PinModel* pins[INFORCE_COLS][INFORCE_ROWS];

	float halfLife = 0.1;
};