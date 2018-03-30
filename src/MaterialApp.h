#pragma once

#include "Application.h"


class MaterialApp : public Application {
public:
	MaterialApp();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	string getName() { return "Material Application"; };

	float variance = 4; // 0.0000000000001;

	OscDataSender dataSender;

private:
	float time = 0.0;

	// Notes: 
	// - lower kp feels stiffer
	// - speedGoingUp also matters

	// pin motion control
	float kp = 300; // 500
	float kd = 300;
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


};