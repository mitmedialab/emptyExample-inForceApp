#pragma once

#include "Application.h"


class DataCollectionApp : public Application {
public:
	DataCollectionApp();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	string getName() { return "Testing Application"; };

private:

	int minHeight = 0;
	int maxHeight = 85000;
	int height = 0;
	int velocity = 0;
	int setVelocity = 200;
	int phase = 1;
	int up = 1;

	float pause = 3;

	// Data collection
	float time = 0;
	ofstream outputFile;


};