#pragma once

#include "Application.h"


class JugglingApp : public Application {
public:
	JugglingApp();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	string getName() { return "Juggling Application"; };



private:
	float time = 0.0;

	float centerI = 2;
	float centerJ = 2;
	float ballRadius = 2.5f;
	float ballHeight = 0.4f;
	float scale = 20000;

	float height = 30000;

	float throwTime = 0;


	int32_t minHeight = 25000;
	int32_t maxHeight = 110000;


};