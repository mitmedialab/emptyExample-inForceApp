#pragma once

#include "Application.h"


class GradientApp : public Application {
public:
	GradientApp();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	string getName() { return "Gradient Application"; };

	float variance = 0.00001; // 0.0000000000001;


	OscDataSender dataSender;
	void updateOscSend();

	void sendOscParameter();

private:
	float time = 0.0;

	// pin motion control
	float kp = 500;
	float kd = 500;
	float speedGoingUp = 3.0;
	float kd_height = 0.7;

	float lastNetForce[INFORCE_COLS][INFORCE_ROWS];

	bool frequencyGradient = false;

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