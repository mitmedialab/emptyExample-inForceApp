#pragma once

#include "Application.h"
#include "ProjectionMapper.h"


class LayerApp2 : public Application {
public:
	LayerApp2();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	string getName() { return "Layer Application"; };

	OscDataSender dataSender;
	void updateOscSend();

	void randomLayer();

private:
	float time = 0.0;

	float layerToHeightOfLayer(int layer);
	float layerToStiffnessOfLayer(int layer);

	int NUM_LAYERS = 10;
	float currentLayer = 0;
	float layerStiffness[100];
	float layerHeight[10];

	float maxHeight = 80000;
	float minHeight = 10000;

	float pauseTime = 1.0;
		
};