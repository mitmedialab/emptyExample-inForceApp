#include "PinModel.h"

PinModel::PinModel(int ii, int jj) {
	pinI = ii;
	pinJ = jj;
	for (int i = 0; i < historyLength; i++) {
		heightForShapeDisplayHistory[i] = 0;
		heightFromShapeDisplayHistory[i] = 0;
		forceFromShapeDisplayHistory[i] = 0;

		idealMovementForceHistory[i] = 0;
		movementForceHistory[i] = 0;
		unsmoothedForceHistory[i] = 0;
		forceHistory[i] = 0;
	}
	//cout << "Pin initialized: " << ii << " " << jj << endl;
}

void PinModel::setForceCurve(int upOrDown, int height, float force) {
	if (upOrDown == 0)
		heightToForceRest[height] = force;
	else if (upOrDown == 1)
		heightToForceGoingUp[height] = force;
	else if (upOrDown == -1)
		heightToForceGoingDown[height] = force;
}
void PinModel::calibrate(float dt, float rawForce) {
	const float alpha = pow(0.5, dt / halfLife); // 0.9885

	if (rawForce >= minRestForce && rawForce <= maxRestForce) {
		restMean = alpha*restMean + (1 - alpha)*rawForce;
		restVar = max(float(9.0), alpha*restVar + (1 - alpha)*(rawForce - restMean)*(rawForce - restMean));
	}
}
void PinModel::update(float dt, float heightForShapeDisplay, float heightFromShapeDisplay, float forceFromShapeDisplay) {
	frame += 1;

	for (int i = historyLength - 1; i > 0; i--)
		heightForShapeDisplayHistory[i] = heightForShapeDisplayHistory[i - 1];
	heightForShapeDisplayHistory[0] = heightForShapeDisplay;

	for (int i = historyLength - 1; i > 0; i--)
		heightFromShapeDisplayHistory[i] = heightFromShapeDisplayHistory[i - 1];
	heightFromShapeDisplayHistory[0] = heightFromShapeDisplay;

	for (int i = historyLength - 1; i > 0; i--)
		forceFromShapeDisplayHistory[i] = forceFromShapeDisplayHistory[i - 1];
	forceFromShapeDisplayHistory[0] = forceFromShapeDisplay;


	// Detect if pin is in the process of moving up or down
	if (heightForShapeDisplayHistory[communicationDelay] - heightForShapeDisplayHistory[communicationDelay+5] > 5 * upOrDownThreshold)
		upOrDown = 1;
	else if (heightForShapeDisplayHistory[communicationDelay] - heightForShapeDisplayHistory[communicationDelay+5] < -5 * upOrDownThreshold)
		upOrDown = -1;
	else
		upOrDown = 0;

	// Update rest mean force parameters
	//const float alpha = pow(0.5, dt / halfLife); // 0.9885
	//if (forceFromShapeDisplay >= minRestForce && forceFromShapeDisplay <= maxRestForce) {
	//	restMean = alpha*restMean + (1 - alpha)*forceFromShapeDisplay;
	//}

	for (int i = historyLength - 1; i > 0; i--)
		smoothCurrentHistory[i] = smoothCurrentHistory[i - 1];
	smoothCurrentHistory[0] = _smoothCurrent();

	for (int i = historyLength - 1; i > 0; i--)
		idealMovementForceHistory[i] = idealMovementForceHistory[i - 1];
	idealMovementForceHistory[0] = _idealMovementForce();

	for (int i = historyLength - 1; i > 0; i--)
		movementForceHistory[i] = movementForceHistory[i - 1];
	movementForceHistory[0] = _movementForce();

	for (int i = historyLength - 1; i > 0; i--)
		unsmoothedForceHistory[i] = unsmoothedForceHistory[i - 1];
	unsmoothedForceHistory[0] = _unsmoothedForce();

	for (int i = historyLength - 1; i > 0; i--)
		forceHistory[i] = forceHistory[i - 1];
	forceHistory[0] = getForce();
}

float PinModel::_idealMovementForce() {
	/*
	int delay = 7;

	int height = heightFromShapeDisplayHistory[0];
	int heightFloored = (height / 1000) * 1000;
	float heightFractional = (height - heightFloored) / 1000.0;
	
	float forceGoingDown = (1.0 - heightFractional)*heightToForceGoingDown[heightFloored] + heightFractional*heightToForceGoingDown[heightFloored + 1000];
	float forceGoingUp = (1.0 - heightFractional)*heightToForceGoingUp[heightFloored] + heightFractional*heightToForceGoingUp[heightFloored + 1000];

	
	float upOrDownFractional = (heightForShapeDisplayHistory[delay+4] - heightFromShapeDisplayHistory[delay]) / threshold;
	if (abs(upOrDownFractional) > 1) upOrDownFractional /= abs(upOrDownFractional);
	upOrDownFractional = (upOrDownFractional + 1) / 2;

	if (pinI == 2 && pinJ == 2 && frame % 1 == 0) cout << "force: " << threshold << " " << (heightForShapeDisplayHistory[delay] - heightFromShapeDisplayHistory[delay]) << " " << upOrDownFractional << endl;

	return (1.0 - upOrDownFractional)*forceGoingDown + upOrDownFractional*forceGoingUp;
	*/

	int height = heightFromShapeDisplayHistory[0];
	int heightFloored = (height / 1000) * 1000;
	float heightFractional = (height - heightFloored) / 1000.0;

	float idealMvmtForce;
	if (upOrDown == 0)
		//idealMvmtForce = restMean;
		idealMvmtForce = (1.0 - heightFractional)*heightToForceRest[heightFloored] + heightFractional*heightToForceRest[heightFloored + 1000]; // linear interpolate
	else if (upOrDown == 1)
		idealMvmtForce = (1.0 - heightFractional)*heightToForceGoingUp[heightFloored] + heightFractional*heightToForceGoingUp[heightFloored + 1000];
	else if (upOrDown == -1)
		idealMvmtForce = (1.0 - heightFractional)*heightToForceGoingDown[heightFloored] + heightFractional*heightToForceGoingDown[heightFloored + 1000];
	
	if (idealMvmtForce == 0)
		return restMean;
	
	return idealMvmtForce;
}
float PinModel::_movementForce() {
	//int window = 4;
	//int delay = 3;
	/*
	int speed = abs(heightForShapeDisplayHistory[0] - heightForShapeDisplayHistory[1]);

	if (speed > 300) {
		window = 4;
		delay = 5;
	} 
	else if (speed > 200) {
		window = 5;
		delay = 4;
	}
	else if (speed > 100) {
		window = 6;
		delay = 5;
	}
	else if (speed > 70) {
		window = 7;
		delay = 6;
	}
	else if (speed > 50) {
		window = 8;
		delay = 6;
	}
	else {
		window = 8;
		delay = 7;
	}

	float sum = 0.0;
	for (int i = 0; i < window; i++)
		sum += idealMovementForceHistory[delay+i];

	return 0.5 * (sum / window) + 0.25 * movementForceHistory[0] + 0.25 * movementForceHistory[1];
	*/

	return (3 * idealMovementForceHistory[3] +
		4 * idealMovementForceHistory[4] +
		4 * idealMovementForceHistory[5] +
		3 * idealMovementForceHistory[6] +
		2 * idealMovementForceHistory[7] +
		1 * idealMovementForceHistory[8]) / 17;

		
	return (3 * idealMovementForceHistory[2] +
		4 * idealMovementForceHistory[3] +
		3 * idealMovementForceHistory[4]) / 10;
}

float covariance(float arr1[], float arr2[], int start1, int end1, int start2, int end2) {
	assert(end1 - start1 == end2 - start2);
	int size = end1 - start1;

	float cov = 0.0f;
	float arr1mean = 0.0f;
	float arr2mean = 0.0f;

	for (int i = 0; i < size; i++) {
		cov += arr1[start1+i] * arr2[start2+i];
		arr1mean += arr1[start1+i];
		arr2mean += arr2[start2+i];
	}

	arr1mean /= size;
	arr2mean /= size;

	cov -= size*arr1mean*arr2mean;
	cov /= (size - 1);

	return cov;
}
float correlation(float arr1[], float arr2[], int start1, int end1, int start2, int end2) {
	float corr = covariance(arr1, arr2, start1, end1, start2, end2);
	corr /= sqrt(covariance(arr1, arr1, start1, end1, start1, end1));
	corr /= sqrt(covariance(arr2, arr2, start2, end2, start2, end2));
	return corr;
}
float maxShiftedCorr(float arr1[], float arr2[], int start1, int end1, int start2, int end2, int minLags, int maxLags, bool shortenWindow=true, bool verbose=false) {
	assert((end1 - start1 == end2 - start2) || !shortenWindow);
	float bestCorr = -1.0;

	float corrs[100];
	for (int i = minLags; i < maxLags; i++) {
		float corr;
		if (shortenWindow)
			corr = correlation(arr1, arr2, start1 + i, end1, start2, end2 - i);
		else
			corr = correlation(arr1, arr2, start1 + i, end1 + i, start2, end2);
		if (corr > bestCorr)
			bestCorr = corr;
		corrs[i - minLags] = corr;
		if (verbose) cout << "corr " << i << ": " << corr << endl;
	}

	// Find smallest period with significant autocorrelation
	int period = -1;
	for (int i = maxLags - 1; i >= minLags; i--) {
		if (corrs[i - minLags] > max(0.5*bestCorr, 0.1))
			period = i;
	}

	if (period == -1)
		return -1;

	if (verbose) cout << "period: " << period << endl;

	// Search around optimal period
	int bestPeriod = -1;
	int min_i = max(0, period - 2);
	int max_i = min(maxLags, period + 2);
	for (int i = min_i; i <= max_i; i++) {
		if (corrs[i - minLags] >= corrs[period - minLags])
			bestPeriod = i;
		if (verbose) cout << "# corrs " << i << ", bestPeriod " << bestPeriod << ": " << corrs[i - minLags] << ", " << bestPeriod << endl;
	}

	return bestPeriod;
}
float PinModel::_smoothCurrent() {
	const int length = 60; // how many data points we should use
	const int minPeriod = 6;
	const int maxPeriod = 40;
	float oscillationMagnitude = 50; // don't adjust by too much; to distinguish from when the force is ACTUALLY oscillating
	
	if (frame > length + 60) {

		// TODO:
		// - period detection for small wiggilations
		
		// Determine period of oscillations

		float forceDifference[length - 1];
		for (int i = 0; i < length - 1; i++)
			forceDifference[i] = forceFromShapeDisplayHistory[i + 1] - forceFromShapeDisplayHistory[i]; // TODO: clip this

		int period = maxShiftedCorr(forceDifference, forceDifference, 0, length - 1, 0, length - 1, minPeriod, maxPeriod); // TODO: clip at corr>0.10 or period not in [5, 15]
		
		//if (pinI == 0 && pinJ == 2 && frame % 1 == 0) cout << "period: " << period << endl;
		//if (pinI == 0 && pinJ == 2 && frame % 50 == 0) int temp = maxShiftedCorr(forceDifference, forceDifference, 0, length - 1, 0, length - 1, minPeriod, maxPeriod, true, true);

		// Adjust by averaging current[now] with current[now-(half a period)]]

		float adjustedCurrent;
		if (period != -1) {
			float current = forceFromShapeDisplayHistory[0];
			float current_shifted = forceFromShapeDisplayHistory[period / 2];

			if (current_shifted - current > oscillationMagnitude) current_shifted = current + 50;
			else if (current_shifted - current < -oscillationMagnitude) current_shifted = current - 50;

			adjustedCurrent = (current + current_shifted) / 2;
		}
		else // there is no period
			adjustedCurrent = forceFromShapeDisplayHistory[0];
		
		return (5 * adjustedCurrent + 3 * smoothCurrentHistory[0] + 1 * smoothCurrentHistory[1]) / 9;
	}
	else {
		return forceFromShapeDisplayHistory[0];
	}
}

float PinModel::_unsmoothedForce() {
	float force = smoothCurrentHistory[0];

	float touchThreshold = 50;
	const float alpha = pow(0.5, (1.0/60.0) / halfLife); // 0.9885

	
	if (force < movementForceHistory[0] + touchThreshold) { // not touching
		//restMeanAdjusted = alpha*restMeanAdjusted + (1 - alpha) * movementForceHistory[0];
		restMeanAdjusted = movementForceHistory[0];
		return 0;
	}
	else {
		restMeanAdjusted = alpha*restMeanAdjusted + (1 - alpha) * restMean;
		return force - restMeanAdjusted;
	}
	//force -= 300;
	//force -=  movementForceHistory[0];
	//return force;
}



float PinModel::getForce() {
	int deadzone = 80;

	return max(0, -deadzone + (5*unsmoothedForceHistory[0] + 3*unsmoothedForceHistory[1] + 1*unsmoothedForceHistory[2]) / 9);
	
}