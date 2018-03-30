#pragma once

#include "Application.h"


class MirrorApp : public Application {
public:
	MirrorApp();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	string getName() { return "Mirror Application"; };

private:
	float time = 0.0;

	const static int INFORCE_COLS_HALF = 4;

	// pin motion control
	float kp = 600;
	float kd = 100;
	float lastNetForce[INFORCE_COLS][INFORCE_ROWS];

	float leftStrength = 1.0;


	int32_t minHeight = 10000;
	int32_t maxHeight = 80000;
	int32_t midHeight = 45000;


};