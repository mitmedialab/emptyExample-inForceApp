#pragma once
#include "Config.h"
#include <map>


class PinModel {
public:
	PinModel(int ii, int jj);

	void setForceCurve(int upOrDown, int height, float force);
	void calibrate(float dt, float rawForce);
	void update(float dt, float heightForShapeDisplay, float heightFromShapeDisplay, float forceFromShapeDisplay);


	float _idealMovementForce();
	float _movementForce();

	float _smoothCurrent();

	float _unsmoothedForce();
	float getForce();

	// Pin[i][j]
	int pinI, pinJ;
	int frame = 0;

	// We model PinForce ~ N(rest_mean, rest_var) at rest
	float restMean = 250;
	float restVar = 10;

	float restMeanAdjusted = 250;

	const static int historyLength = 1000;
	const static int communicationDelay = 4;
	int heightForShapeDisplayHistory[historyLength];
	int heightFromShapeDisplayHistory[historyLength];
	int forceFromShapeDisplayHistory[historyLength];


	int smoothCurrentHistory[historyLength];
	int idealMovementForceHistory[historyLength];
	int movementForceHistory[historyLength];
	int unsmoothedForceHistory[historyLength];
	int forceHistory[historyLength];

	int upOrDown = 0;

	int window = 4;
	int delay = 3;

private:

	int upOrDownThreshold = 15;
	map<int, float> heightToForceRest;
	map<int, float> heightToForceGoingUp;
	map<int, float> heightToForceGoingDown;


	const float halfLife = 0.5;
	const float minRestForce = 0;
	const float maxRestForce = 600;
	float significanceThreshold = 0; 

};