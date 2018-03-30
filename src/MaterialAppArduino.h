#pragma once

#include "Application.h"


class MaterialAppArduino : public Application {
public:
	MaterialAppArduino();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	void drawApp();

	string getName() { return "Material Application in Arduino"; };

	OscDataSender dataSender;
	void updateOscSend();

	void sendHeightDependentForceParameters(int maxHeight, int minHeight, int heightMapping[], int forceMapping[], int numMappings);
	void sendControlParameters(float variance, float kp, float kd, int speedGoingUp, float kd2_soft, float kd2_hard);

	void readAndSendHeightDependentForceParametersFromFile(string filename);

	float variance = 2.0;

	ofFbo graphMaterialFbo;

private:
	float time = 0.0;


};