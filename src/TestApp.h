#pragma once

#include "Application.h"


class TestApp : public Application {
public:
	TestApp();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);
    void updateOscSend();

	OscDataSender dataSender;

	string getName() { return "Testing Application"; };

private:
	void updateHeights();
	void updateHeights2();
	void updateHeights3();
	void updateHeights4();
	void updateHeights5(int i, int j);

	float x = 0;
	float y = 0;

	float dx = 2;
	float dy = 3;

	int32_t minHeight = 20000;
	int32_t maxHeight = 60000;
	float variance = 16;

	float setHeight = 50000;
};