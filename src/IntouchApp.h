#pragma once

#include "Application.h"


class IntouchApp : public Application {
public:
	IntouchApp();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	string getName() { return "inTouch Application"; };

	float variance = 3; // 0.0000000000001;

private:
	float time = 0.0;

	// pin motion control
	float kp = 500;
	float kd = 100;
	float lastNetForce[INFORCE_COLS][INFORCE_ROWS];
	float speedGoingUp = 1.2;

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


};