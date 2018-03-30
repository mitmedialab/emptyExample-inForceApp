#pragma once

#include "Application.h"


class MaterialApp2 : public Application {
public:
	MaterialApp2();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	string getName() { return "Testing Application"; };


	bool isTouched[INFORCE_COLS][INFORCE_ROWS];
	float forceFromTouchedPins[INFORCE_COLS][INFORCE_ROWS];
	float heightsFromTouchedPins[INFORCE_COLS][INFORCE_ROWS];
	float variance = 16;


	//ofColor val_color[INFORCE_COLS][INFORCE_ROWS];

private:
	float time = 0.0;

	float kp = 800; 
	float kd = 50;
	float lastDeltaForce[INFORCE_COLS][INFORCE_ROWS];

	float staticFrictionForce = 50;

	float getForce(int height, int max);
	float getForce2(int height);
	float getForce3(int height);

	int32_t minHeight = 10000;
	int32_t maxHeight = 80000;

	float minForce = 50;
	float midForce = 800;
	float maxForce = 1300;


	float speedConstant = 500;
};