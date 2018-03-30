#pragma once

#include "Application.h"


class MaterialFuncApp : public Application {
public:
	MaterialFuncApp();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	void drawApp();

	string getName() { return "Testing Application"; };

private:
	float time = 0.0;

	// pin motion control
	float kp = 700;
	float kd = 50;
	float lastDeltaForce[INFORCE_COLS][INFORCE_ROWS];
	bool pinAcceleratingUp[INFORCE_COLS][INFORCE_ROWS];

	float getTargetForce(int height);
	float getTargetForce2(int height);
	float getTargetForce3(int height);
	float getTargetForce4(int height);

	float getTargetFrequency(int height);

	int32_t minHeight = 10000;
	int32_t maxHeight = 80000;

	float minForce = 200;
	float midForce = 500;
	float maxForce = 900;

	float maxFrequency = 30;
	float minFrequency = 0;

	float speedConstant = 500;

	float lastNetForce[INFORCE_COLS][INFORCE_ROWS];


	void setPinParameter(int x, int y, int maxH, int minH, int force, float frequency, bool vibrationWithPushing);
	void processPinPositionUsingParameter(float dt);

	void resetPinCalculationParameter(int x, int y);

	float variance = 0.0000000000001;





};