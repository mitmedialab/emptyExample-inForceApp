#pragma once

#include "Application.h"


class MaterialApp2 : public Application {
public:
	MaterialApp2();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	string getName() { return "Testing Application"; };

	float variance = 4;

private:
	float time = 0.0;

	// pin motion control
	float kp = 700;
	float kd = 100;
	float lastNetForce[INFORCE_COLS][INFORCE_ROWS];

	float heightsForShapeDisplayBuffered[INFORCE_COLS + 2][INFORCE_ROWS + 2]; //heightForShapeDisplayBuffered[i+1][j+1]=heightForShapeDisplay[i][j]
	float heightsFromShapeDisplayBuffered[INFORCE_COLS + 2][INFORCE_ROWS + 2]; //heightForShapeDisplayBuffered[i+1][j+1]=heightForShapeDisplay[i][j]



	float getTargetForce(int height);
	float getTargetForce2(int height);
	float getTargetForce3(int height);
	float getTargetForce4(int height);

	float getTargetFrequency(int height);

	int32_t minHeight = 10000;
	int32_t maxHeight = 80000;


};