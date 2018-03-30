#pragma once

#include "Application.h"


class WaterAppArduino : public Application {
public:
	WaterAppArduino();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	string getName() { return "Water Application in Arduino"; };

	OscDataSender dataSender;
	void updateOscSend();

private:
	float time = 0.0;


};