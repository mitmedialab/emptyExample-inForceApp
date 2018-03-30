#include "ProjectionApp.h"
#include <iostream>
#include <fstream>

ProjectionApp::ProjectionApp() {
	cout << "###### Application created: " << ProjectionApp::getName() << endl;

	setupProjectionMesh();
}

void ProjectionApp::setupProjectionMesh() {
	for (int y = 0; y < INFORCE_ROWS + 1; y++) {
		for (int x = 0; x< INFORCE_COLS + 1; x++) {
			vertexPinHigh.push_back(ofVec2f(x * 20 + y * 5, y * 20));    // static - mesh index = x + y*width
			vertexPinLow.push_back(ofVec2f(x * 20 + y * 5, y * 20 + 100));    // static - mesh index = x + y*width
		}
	}


	highMesh.clear();
	highMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	lowMesh.clear();
	lowMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	for (int y = 0; y < INFORCE_ROWS + 1; y++) {
		for (int x = 0; x< INFORCE_COLS + 1; x++) {
			int index = x + y*(INFORCE_COLS + 1);
			highMesh.addVertex(vertexPinHigh[index]);
			lowMesh.addVertex(vertexPinLow[index]);
		}
	}

	for (int y = 0; y<INFORCE_ROWS; y++) {
		for (int x = 0; x<INFORCE_COLS; x++) {
			highMesh.addIndex(x + y*(INFORCE_COLS + 1));                // 0
			highMesh.addIndex((x + 1) + y*(INFORCE_COLS + 1));            // 1
			highMesh.addIndex(x + (y + 1)*(INFORCE_COLS + 1));            // 10

			highMesh.addIndex((x + 1) + y*(INFORCE_COLS + 1));            // 1
			highMesh.addIndex((x + 1) + (y + 1)*(INFORCE_COLS + 1));        // 11
			highMesh.addIndex(x + (y + 1)*(INFORCE_COLS + 1));


			lowMesh.addIndex(x + y*(INFORCE_COLS + 1));                // 0
			lowMesh.addIndex((x + 1) + y*(INFORCE_COLS + 1));            // 1
			lowMesh.addIndex(x + (y + 1)*(INFORCE_COLS + 1));

			lowMesh.addIndex((x + 1) + y*(INFORCE_COLS + 1));            // 1
			lowMesh.addIndex((x + 1) + (y + 1)*(INFORCE_COLS + 1));        // 11
			lowMesh.addIndex(x + (y + 1)*(INFORCE_COLS + 1));            // 10
		}
	}



	// define pin-height this would be dynamic
	

	mapImageTop.load("projectionTest/test-02.png");

	for (int x = 0; x< INFORCE_ROWS; x++) {
		mapImageFront[x].load("projectionTest/test-02.png");
	}
	for (int y = 0; y < INFORCE_COLS; y++) {
		mapImageSide[y].load("projectionTest/test-02.png");
	}


	updateMeshRendering();

}

void ProjectionApp::update(float dt) {
	if (dx > 0)
		x += dt*dx;
	else
		x += dt*dx;
	y += dt*dy;
	if (x > INFORCE_COLS) {
		x = INFORCE_COLS;
		dx = -abs(dx);
	}
	if (x < 0) {
		x = 0;
		dx = abs(dx);
	}
	if (y > INFORCE_ROWS) {
		y = INFORCE_ROWS;
		dy = -abs(dy);
	}
	if (y < 0) {
		y = 0;
		dy = abs(dy);
	}
	//updateHeights2();
	//updateHeights5(2, 1);
	//cout << "asdf: " << (heightsFromShapeDisplay[1][1] + 17342) << " " << forceFromShapeDisplay[1][1] << endl;
	//cout << "temp: " << forceFromShapeDisplay[1][4] << endl;

	//std::bitset<32> x(heightsFromShapeDisplay[1][1]);
	//std::bitset<32> y(-3);
	//std::cout << "asdf2: " << x << " " << y << endl;

	for (int y = 0; y < INFORCE_ROWS; y++) {
		for (int x = 0; x< INFORCE_COLS; x++) {
			float d = ofDist(0, 0, x, y);
			//heightsForShapeDisplay[x][y] = ofMap(sin((float(int(ofGetElapsedTimeMillis()) % 5000)) / 5000 + d / 10), -1, 1, pinHeight_MIN, pinHeight_MAX);
			heightsForShapeDisplay[x][y] = pinHeight_MAX;
		}
	}

	if (editMode == 1) {
		for (int y = 0; y < INFORCE_ROWS; y++) {
			for (int x = 0; x< INFORCE_COLS; x++) {
				heightsForShapeDisplay[x][y] = pinHeight_MAX;
			}
		}
	} else if (editMode == 2) {
		for (int y = 0; y < INFORCE_ROWS; y++) {
			for (int x = 0; x< INFORCE_COLS; x++) {
				heightsForShapeDisplay[x][y] = pinHeight_MIN;
			}
		}
	}


	

	updateMeshRendering();

}

void ProjectionApp::updateHeights() {
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			float distanceSquared = (i - x)*(i - x) + (j - y)*(j - y);
			float height = exp(-distanceSquared / variance) * (maxHeight - minHeight) + minHeight;

			heightsForShapeDisplay[i][j] = height;
		}
	}
}

void ProjectionApp::updateHeights2() {
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = float(x) / INFORCE_COLS*maxHeight;
		}
	}
}



void ProjectionApp::updateHeights3() {
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = setHeight;
		}
	}

}

void ProjectionApp::updateHeights4() {

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = float(x) / INFORCE_COLS*maxHeight;
		}
	}
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			if (i != 0 || j != 0) {
				float distanceSquared = (i - x)*(i - x) + (j - y)*(j - y);
				float height = exp(-distanceSquared / variance) * (maxHeight - minHeight) + minHeight;
				heightsForShapeDisplay[i][j] = height;
			}
		}
	}
}

void ProjectionApp::updateHeights5(int i, int j) {
	heightsForShapeDisplay[i][j] = float(x) / INFORCE_COLS*maxHeight;
}

void ProjectionApp::updateMeshRendering() {
	// define pin-height this would be dynamic
	int imgWTop = mapImageTop.getWidth();
	int imgHTop = mapImageTop.getHeight();

	//add one vertex to the mesh for each pixel


	for (int y = 0; y < INFORCE_ROWS + 1; y++) {
		for (int x = 0; x< INFORCE_COLS + 1; x++) {
			imageCoordinateTop[x][y].set((imgWTop / INFORCE_COLS) * x, (imgHTop / INFORCE_ROWS) * y); // static (image can be dynamic)
		}
	}

	for (int y = 0; y < INFORCE_ROWS; y++) {
		for (int x = 0; x< INFORCE_COLS; x++) {
			pin_TopMesh[x][y].clear();
			pin_TopMesh[x][y].setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

			for (int i = x; i < x + 2; i++) {
				for (int j = y; j < y + 2; j++) {
					int index = (i)+(j)*(INFORCE_COLS + 1);
					float h_x = ofMap(heightsFromShapeDisplay[INFORCE_COLS-1-x][INFORCE_ROWS-1-y], pinHeight_MIN, pinHeight_MAX, vertexPinLow[index].x, vertexPinHigh[index].x);
					float h_y = ofMap(heightsFromShapeDisplay[INFORCE_COLS-1-x][INFORCE_ROWS-1-y], pinHeight_MIN, pinHeight_MAX, vertexPinLow[index].y, vertexPinHigh[index].y);
					pin_TopMesh[x][y].addVertex(ofVec2f(h_x, h_y));
					pin_TopMesh[x][y].addTexCoord(imageCoordinateTop[i][j]);
				}
			}

		}
	}


	for (int y = 0; y< INFORCE_ROWS; y++) {
		for (int x = 0; x< INFORCE_COLS; x++) {
			pin_TopMesh[x][y].addIndex(0);                // 0
			pin_TopMesh[x][y].addIndex(1);            // 1
			pin_TopMesh[x][y].addIndex(2);            // 10

			pin_TopMesh[x][y].addIndex(1);            // 1
			pin_TopMesh[x][y].addIndex(3);        // 11
			pin_TopMesh[x][y].addIndex(2);            // 10
		}
	}


	///SIDE ////

	float imgWSide = mapImageSide[0].getWidth();
	float imgHSide = mapImageSide[0].getHeight();

	//add one vertex to the mesh for each pixel
	for (int s = 0; s < INFORCE_COLS; s++) {
		for (int x = 0; x< INFORCE_ROWS + 1; x++) {
			for (int y = 0; y < 2; y++) {
				float h = ofMap(heightsFromShapeDisplay[INFORCE_COLS-1-s][INFORCE_ROWS-1-x], pinHeight_MIN, pinHeight_MAX, 0, 1);
				imageCoordinateSide[s][x][y][0].set((imgWSide / INFORCE_ROWS) * x, ofMap(y, 0, 1, imgHSide *(1 - h), imgHSide)); // static (image can be dynamic)
				imageCoordinateSide[s][x][y][1].set((imgWSide / INFORCE_ROWS) * (x+1), ofMap(y, 0, 1, imgHSide *(1 - h), imgHSide)); // static (image can be dynamic)

			}
		}
	}



	for (int s = 0; s < INFORCE_COLS; s++) {
		for (int x = 0; x< INFORCE_ROWS; x++) {
			pin_SideMesh[s][x].clear();
			pin_SideMesh[s][x].setMode(OF_PRIMITIVE_TRIANGLE_STRIP);


			float h_x = ofMap(heightsFromShapeDisplay[INFORCE_COLS-1-s][INFORCE_ROWS-1-x], pinHeight_MIN, pinHeight_MAX, vertexPinLow[s + x *(INFORCE_COLS + 1)].x, vertexPinHigh[s + x *(INFORCE_COLS + 1)].x);
			float h_y = ofMap(heightsFromShapeDisplay[INFORCE_COLS-1-s][INFORCE_ROWS-1-x], pinHeight_MIN, pinHeight_MAX, vertexPinLow[s + x *(INFORCE_COLS + 1)].y, vertexPinHigh[s + x *(INFORCE_COLS + 1)].y);

			float h_x2 = ofMap(heightsFromShapeDisplay[INFORCE_COLS-1-s][INFORCE_ROWS-1-x], pinHeight_MIN, pinHeight_MAX, vertexPinLow[s + (x + 1) *(INFORCE_COLS + 1)].x, vertexPinHigh[s + (x + 1) *(INFORCE_COLS + 1)].x);
			float h_y2 = ofMap(heightsFromShapeDisplay[INFORCE_COLS-1-s][INFORCE_ROWS-1-x], pinHeight_MIN, pinHeight_MAX, vertexPinLow[s + (x + 1) *(INFORCE_COLS + 1)].y, vertexPinHigh[s + (x + 1) *(INFORCE_COLS + 1)].y);



			pin_SideMesh[s][x].addVertex(ofVec2f(h_x, h_y));
			pin_SideMesh[s][x].addVertex(vertexPinLow[s + x *(INFORCE_COLS + 1)]);
			pin_SideMesh[s][x].addVertex(ofVec2f(h_x2, h_y2));
			pin_SideMesh[s][x].addVertex(vertexPinLow[s + (x + 1)  *(INFORCE_COLS + 1)]);

			pin_SideMesh[s][x].addTexCoord(imageCoordinateSide[s][x][0][0]);
			pin_SideMesh[s][x].addTexCoord(imageCoordinateSide[s][x][1][0]);
			pin_SideMesh[s][x].addTexCoord(imageCoordinateSide[s][x][0][1]);
			pin_SideMesh[s][x].addTexCoord(imageCoordinateSide[s][x][1][1]);
		}
	}


	for (int s = 0; s < INFORCE_COLS; s++) {
		for (int x = 0; x< INFORCE_ROWS; x++) {

			pin_SideMesh[s][x].addIndex(0);                // 0
			pin_SideMesh[s][x].addIndex(1);            // 1
			pin_SideMesh[s][x].addIndex(2);            // 10

			pin_SideMesh[s][x].addIndex(1);            // 1
			pin_SideMesh[s][x].addIndex(3);            // 11
			pin_SideMesh[s][x].addIndex(2);            // 10
		}
	}


	/////FRONT////

	int imgWFront = mapImageFront[0].getWidth();
	int imgHFront = mapImageFront[0].getHeight();

	//add one vertex to the mesh for each pixel


	for (int s = 0; s < INFORCE_ROWS; s++) {
		for (int x = 0; x< INFORCE_COLS + 1; x++) {
			for (int y = 0; y < 2; y++) {
				float h = ofMap(heightsFromShapeDisplay[INFORCE_COLS-1-x][INFORCE_ROWS-1-s], pinHeight_MIN, pinHeight_MAX, 0, 1);
				imageCoordinateFront[s][x][y][0].set((imgWFront / INFORCE_COLS) * x, ofMap(y, 0, 1, imgHFront *(1 - h), imgHFront)); // static (image can be dynamic)
				imageCoordinateFront[s][x][y][1].set((imgWFront / INFORCE_COLS) * (x+1), ofMap(y, 0, 1, imgHFront *(1 - h), imgHFront)); // static (image can be dynamic)

			}
		}
	}



	for (int s = 0; s < INFORCE_ROWS; s++) {
		for (int x = 0; x< INFORCE_COLS; x++) {
			pin_FrontMesh[s][x].clear();
			pin_FrontMesh[s][x].setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

			float h_x = ofMap(heightsFromShapeDisplay[INFORCE_COLS-1-x][INFORCE_ROWS-1-s], pinHeight_MIN, pinHeight_MAX, vertexPinLow[x + (s + 1) *(INFORCE_COLS + 1)].x, vertexPinHigh[x + (s + 1) *(INFORCE_COLS + 1)].x);
			float h_y = ofMap(heightsFromShapeDisplay[INFORCE_COLS-1-x][INFORCE_ROWS-1-s], pinHeight_MIN, pinHeight_MAX, vertexPinLow[x + (s + 1) *(INFORCE_COLS + 1)].y, vertexPinHigh[x + (s + 1) *(INFORCE_COLS + 1)].y);

			float h_x2 = ofMap(heightsFromShapeDisplay[INFORCE_COLS-1-x][INFORCE_ROWS-1-s], pinHeight_MIN, pinHeight_MAX, vertexPinLow[(x + 1) + (s + 1) *(INFORCE_COLS + 1)].x, vertexPinHigh[(x + 1) + (s + 1) *(INFORCE_COLS + 1)].x);
			float h_y2 = ofMap(heightsFromShapeDisplay[INFORCE_COLS-1-x][INFORCE_ROWS-1-s], pinHeight_MIN, pinHeight_MAX, vertexPinLow[(x + 1) + (s + 1) *(INFORCE_COLS + 1)].y, vertexPinHigh[(x + 1) + (s + 1) *(INFORCE_COLS + 1)].y);


			pin_FrontMesh[s][x].addVertex(ofVec2f(h_x, h_y));
			pin_FrontMesh[s][x].addVertex(vertexPinLow[x + (s + 1)*(INFORCE_COLS + 1)]);
			pin_FrontMesh[s][x].addVertex(ofVec2f(h_x2, h_y2));
			pin_FrontMesh[s][x].addVertex(vertexPinLow[(x + 1) + (s + 1)  *(INFORCE_COLS + 1)]);


			pin_FrontMesh[s][x].addTexCoord(imageCoordinateFront[s][x][0][0]);
			pin_FrontMesh[s][x].addTexCoord(imageCoordinateFront[s][x][1][0]);
			pin_FrontMesh[s][x].addTexCoord(imageCoordinateFront[s][x][0][1]);
			pin_FrontMesh[s][x].addTexCoord(imageCoordinateFront[s][x][1][1]);

		}
	}

	for (int s = 0; s < INFORCE_ROWS; s++) {
		for (int x = 0; x< INFORCE_COLS; x++) {
			pin_FrontMesh[s][x].addIndex(0);                // 0
			pin_FrontMesh[s][x].addIndex(1);            // 1
			pin_FrontMesh[s][x].addIndex(2);            // 10

			pin_FrontMesh[s][x].addIndex(1);            // 1
			pin_FrontMesh[s][x].addIndex(3);        // 11
			pin_FrontMesh[s][x].addIndex(2);            // 10
		}
	}
}

void ProjectionApp::loadCalibrationData() {
	if (XML.loadFile("projectionTest/calibration_projectionMapping_inForce.xml")) {
		message = "calibration_projectionMapping_inForce.xml loaded!";
	}
	else {
		message = "unable to load calibration_projectionMapping_inForce.xml check data/ folder";
	}


	XML.pushTag("CALIBRATION", 0);
	XML.pushTag("HIGH", 0);
	for (int y = 0; y < INFORCE_ROWS + 1; y++) {
		for (int x = 0; x < INFORCE_COLS + 1; x++) {
			int index = x + y*(INFORCE_COLS + 1);
			vertexPinHigh[index].set(XML.getValue("PT:X", 0, index), XML.getValue("PT:Y", 0, index));
			highMesh.setVertex(index, vertexPinHigh[index]);

		}
	}

	XML.popTag();

	XML.pushTag("LOW", 0);
	for (int y = 0; y < INFORCE_ROWS + 1; y++) {
		for (int x = 0; x < INFORCE_COLS + 1; x++) {
			int index = x + y*(INFORCE_COLS + 1);
			vertexPinLow[index].set(XML.getValue("PT:X", 0, index), XML.getValue("PT:Y", 0, index));
			lowMesh.setVertex(index, vertexPinLow[index]);
		}
	}
	XML.popTag();
	XML.popTag();


}

void ProjectionApp::saveCalibrationData() {
	XML.clear();
	int lastTagNumber = XML.addTag("CALIBRATION");

	if (XML.pushTag("CALIBRATION", lastTagNumber)) {
		lastTagNumber = XML.addTag("HIGH");
		if (XML.pushTag("HIGH", lastTagNumber)) {
			for (int y = 0; y < INFORCE_ROWS + 1; y++) {
				for (int x = 0; x < INFORCE_COLS + 1; x++) {

					int tagNum = XML.addTag("PT");
					int index = x + y*(INFORCE_COLS + 1);

					XML.setValue("PT:X", vertexPinHigh[index].x, tagNum);
					XML.setValue("PT:Y", vertexPinHigh[index].y, tagNum);

				}
			}

			XML.popTag();
		}

		lastTagNumber = XML.addTag("LOW");
		if (XML.pushTag("LOW", lastTagNumber)) {
			for (int y = 0; y < INFORCE_ROWS + 1; y++) {
				for (int x = 0; x < INFORCE_COLS + 1; x++) {

					//now we will add a pt tag - with two
					//children - X and Y
					int tagNum = XML.addTag("PT");
					int index = x + y*(INFORCE_COLS + 1);

					XML.setValue("PT:X", vertexPinLow[index].x, tagNum);
					XML.setValue("PT:Y", vertexPinLow[index].y, tagNum);

				}
			}

			XML.popTag();
		}

		XML.popTag();
	}


	XML.saveFile("projectionTest/calibration_projectionMapping_inForce.xml");
	cout << "filesaved" << endl;
}

void ProjectionApp::autoCalibrationFromCornerPoints() {

	ofVec2f topLeft = vertexPinHigh[0 + 0 * (INFORCE_COLS + 1)];
	ofVec2f topRight = vertexPinHigh[INFORCE_COLS + 0 * (INFORCE_COLS + 1)];
	ofVec2f bottomLeft = vertexPinHigh[0 + INFORCE_ROWS * (INFORCE_COLS + 1)];
	ofVec2f bottomRight = vertexPinHigh[INFORCE_COLS + INFORCE_ROWS * (INFORCE_COLS + 1)];

	for (int y = 1; y < INFORCE_ROWS; y++) {
		int x = 0;
		int index = x + y*(INFORCE_COLS + 1);
		vertexPinHigh[index].set(topLeft.x + y * (bottomLeft.x - topLeft.x) / INFORCE_ROWS, topLeft.y + y * (bottomLeft.y - topLeft.y) / INFORCE_ROWS);

		x = INFORCE_COLS;
		index = x + y*(INFORCE_COLS + 1);
		vertexPinHigh[index].set(topRight.x + y * (bottomRight.x - topRight.x) / INFORCE_ROWS, topRight.y + y * (bottomRight.y - topRight.y) / INFORCE_ROWS);

	}

	for (int y = 0; y < INFORCE_ROWS + 1; y++) {
		for (int x = 1; x < INFORCE_COLS; x++) {

			ofVec2f left = vertexPinHigh[0 + y*(INFORCE_COLS + 1)];
			ofVec2f right = vertexPinHigh[INFORCE_COLS + y*(INFORCE_COLS + 1)];

			int index = x + y*(INFORCE_COLS + 1);
			vertexPinHigh[index].set(left.x + x * (right.x - left.x) / INFORCE_COLS, left.y + x * (right.y - left.y) / INFORCE_COLS);

		}
	}

	topLeft = vertexPinLow[0 + 0 * (INFORCE_COLS + 1)];
	topRight = vertexPinLow[INFORCE_COLS + 0 * (INFORCE_COLS + 1)];
	bottomLeft = vertexPinLow[0 + INFORCE_ROWS * (INFORCE_COLS + 1)];
	bottomRight = vertexPinLow[INFORCE_COLS + INFORCE_ROWS * (INFORCE_COLS + 1)];

	for (int y = 1; y < INFORCE_ROWS; y++) {
		int x = 0;
		int index = x + y*(INFORCE_COLS + 1);
		vertexPinLow[index].set(topLeft.x + y * (bottomLeft.x - topLeft.x) / INFORCE_ROWS, topLeft.y + y * (bottomLeft.y - topLeft.y) / INFORCE_ROWS);

		x = INFORCE_COLS;
		index = x + y*(INFORCE_COLS + 1);
		vertexPinLow[index].set(topRight.x + y * (bottomRight.x - topRight.x) / INFORCE_ROWS, topRight.y + y * (bottomRight.y - topRight.y) / INFORCE_ROWS);

	}

	for (int y = 0; y < INFORCE_ROWS + 1; y++) {
		for (int x = 1; x < INFORCE_COLS; x++) {

			ofVec2f left = vertexPinLow[0 + y*(INFORCE_COLS + 1)];
			ofVec2f right = vertexPinLow[INFORCE_COLS + y*(INFORCE_COLS + 1)];

			int index = x + y*(INFORCE_COLS + 1);
			vertexPinLow[index].set(left.x + x * (right.x - left.x) / INFORCE_COLS, left.y + x * (right.y - left.y) / INFORCE_COLS);

		}
	}



	updateHighLowMesh();

}

void ProjectionApp::updateHighLowMesh() {
	for (int y = 0; y < INFORCE_ROWS + 1; y++) {
		for (int x = 0; x < INFORCE_COLS + 1; x++) {
			int index = x + y*(INFORCE_COLS + 1);
			highMesh.setVertex(index, vertexPinHigh[index]);
			lowMesh.setVertex(index, vertexPinLow[index]);
		}
	}

}


string ProjectionApp::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}



void ProjectionApp::keyPressed(int key) {
	cout << "keyPressed " << key << endl;

	/*if (key == 'w') {
	//	variance *= 1.1;
	}
	else if (key == 's') {
	//	variance /= 1.1;
	}
	else if (key == 'a') {
	//	dx /= 1.1;
	//	dy /= 1.1;
	}
	else if (key == 'd') {
	//	dx *= 1.1;
	//	dy *= 1.1;
	}
	else if (key == 'q') {
	//	dx *= -1;
	}
	else if (key == 'k') {
		ofstream outputFile;
		outputFile.open("data 1-30-2018.txt");

		int ii = 0;
		int jj = 2;

		outputFile << "forceFromShapeDisplayHistory\n";
		for (int i = 0; i < 400; i++)
			outputFile << pins[ii][jj]->forceFromShapeDisplayHistory[i] << "\n";

		outputFile << "heightForShapeDisplayHistory\n";
		for (int i = 0; i < 400; i++)
			outputFile << pins[ii][jj]->heightForShapeDisplayHistory[i] << "\n";

		outputFile << "heightFromShapeDisplayHistory\n";
		for (int i = 0; i < 400; i++)
			outputFile << pins[ii][jj]->heightFromShapeDisplayHistory[i] << "\n";

		outputFile.close();
	}
	else */if (key == 'e') {
		editMode++;
		if (editMode == 3) {
			editMode = 0;
		}
	}
	else if (key == 's') {
		saveCalibrationData();
	}
	else if (key == 'l') {
		loadCalibrationData();
	}
	else if (key == 'a') {
		autoCalibrationFromCornerPoints();
	}


}

void ProjectionApp::drawApp() {
	ofBackground(0);
	ofDisableDepthTest();
	//ofBackground(100, 100, 100);
	ofTranslate(transX, transY);

	//ofTranslate(0,200);

	if (editMode == 0) {

		ofSetColor(255);

		for (int y = 0; y < INFORCE_ROWS; y++) {
			for (int x = INFORCE_COLS - 1; x >= 0; x--) {
				mapImageSide[x].bind();
				pin_SideMesh[x][y].draw();
				mapImageSide[x].unbind();

				mapImageTop.bind();
				pin_TopMesh[x][y].draw();
				mapImageTop.unbind();

				mapImageFront[y].bind();
				pin_FrontMesh[y][x].draw();
				// pin_FrontMesh[currentY][currentX].draw();
				mapImageFront[y].unbind();


			}
		}

		ofSetColor(200);
		for (int y = 0; y < INFORCE_ROWS; y++) {
			for (int x = 0; x< INFORCE_COLS; x++) {
				// pin_TopMesh[x][y].drawWireframe();
				//   pin_FrontMesh[x][y].drawWireframe();
				//  pin_SideMesh[x][y].drawWireframe();
			}
		}
	}
	else if (editMode == 1) {

		ofSetColor(255);
		ofSetLineWidth(1);
		highMesh.drawWireframe();

		ofNoFill();
		for (int y = 0; y < INFORCE_ROWS + 1; y++) {
			for (int x = 0; x < INFORCE_COLS + 1; x++) {
				int index = x + y*(INFORCE_COLS + 1);

				if ((x == 0 || x == INFORCE_COLS) && (y == 0 || y == INFORCE_ROWS)) {
					ofSetColor(0, 0, 255);
				}
				else {
					ofSetColor(255, 0, 0);
				}
				ofDrawEllipse(vertexPinHigh[index].x, vertexPinHigh[index].y, radius_grabPoint, radius_grabPoint);
			}
		}

	}
	else if (editMode == 2) {

		ofSetColor(255);
		ofSetLineWidth(1);
		lowMesh.drawWireframe();

		ofNoFill();

		for (int y = 0; y < INFORCE_ROWS + 1; y++) {
			for (int x = 0; x < INFORCE_COLS + 1; x++) {
				int index = x + y*(INFORCE_COLS + 1);
				if ((x == 0 || x == INFORCE_COLS) && (y == 0 || y == INFORCE_ROWS)) {
					ofSetColor(0, 0, 255);
				}
				else {
					ofSetColor(255, 0, 0);
				}
				ofDrawEllipse(vertexPinLow[index].x, vertexPinLow[index].y, radius_grabPoint, radius_grabPoint);
			}
		}

	}

}

void ProjectionApp::mouseDragged(int x, int y, int button) {
	if (mouseGrabbing) {
		int index = grabbedPoint.x + grabbedPoint.y*(INFORCE_COLS + 1);
		if (editMode == 1) {
			vertexPinHigh[index].set(x - transX, y - transY);
			highMesh.setVertex(index, vertexPinHigh[index]);
		}
		else {
			vertexPinLow[index].set(x - transX, y - transY);
			lowMesh.setVertex(index, vertexPinLow[index]);

		}
	}
}

void ProjectionApp::mousePressed(int x, int y, int button) {
	if (editMode == 1) {
		for (int j = 0; j < INFORCE_ROWS + 1; j++) {
			for (int i = 0; i < INFORCE_COLS + 1; i++) {
				int index = i + j*(INFORCE_COLS + 1);
				float d = ofDist(x - transX, y - transY, vertexPinHigh[index].x, vertexPinHigh[index].y);
				if (d < radius_grabPoint) {
					vertexPinHigh[index].set(x - transX, y - transY);
					highMesh.setVertex(index, vertexPinHigh[index]);

					grabbedPoint.set(i, j);
					mouseGrabbing = true;
				}
			}
		}

	}
	else if (editMode == 2) {
		for (int j = 0; j < INFORCE_ROWS + 1; j++) {
			for (int i = 0; i < INFORCE_COLS + 1; i++) {
				int index = i + j*(INFORCE_COLS + 1);
				float d = ofDist(x - transX, y - transY, vertexPinLow[index].x, vertexPinLow[index].y);
				if (d < radius_grabPoint) {
					vertexPinLow[index].set(x - transX, y - transY);
					lowMesh.setVertex(index, vertexPinLow[index]);

					grabbedPoint.set(i, j);
					mouseGrabbing = true;
				}
			}
		}
	}
}


void ProjectionApp::mouseReleased(int x, int y, int button) {
	mouseGrabbing = false;
}


