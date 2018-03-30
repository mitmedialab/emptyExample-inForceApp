#pragma once

#include "Application.h"


class PianoAppArduino : public Application {
public:
	PianoAppArduino();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	string getName() { return "Material Application in Arduino"; };

	OscDataSender dataSender;
	void updateOscSend();

private:
	float time = 0.0;


};