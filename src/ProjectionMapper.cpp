#include "ProjectionMapper.h"

ProjectionMapper::ProjectionMapper(int _minHeight, int _maxHeight) {

	pinHeight_MIN = _minHeight;
	pinHeight_MAX = _maxHeight;

	
	setupProjectionMesh();
}



void ProjectionMapper::update(float dt, int32_t heightsFromShapeDisplay[INFORCE_COLS][INFORCE_ROWS]) {

	updateMeshRendering(heightsFromShapeDisplay);

}



void ProjectionMapper::setupProjectionMesh() {
	
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

	loadCalibrationData(); // vertexPinHigh and Low will be updated
	//updateMeshRendering();

}


void ProjectionMapper::updateMeshRendering(int32_t heightsFromShapeDisplay[INFORCE_COLS][INFORCE_ROWS]) {
	int imgWTop = mapImageTop.getWidth();
	int imgHTop = mapImageTop.getHeight();


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
					float h_x = ofMap(heightsFromShapeDisplay[INFORCE_COLS - 1 - x][INFORCE_ROWS - 1 - y], pinHeight_MIN, pinHeight_MAX, vertexPinLow[index].x, vertexPinHigh[index].x);
					float h_y = ofMap(heightsFromShapeDisplay[INFORCE_COLS - 1 - x][INFORCE_ROWS - 1 - y], pinHeight_MIN, pinHeight_MAX, vertexPinLow[index].y, vertexPinHigh[index].y);
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
				float h = ofMap(heightsFromShapeDisplay[INFORCE_COLS - 1 - s][INFORCE_ROWS - 1 - x], pinHeight_MIN, pinHeight_MAX, 0, 1);
				imageCoordinateSide[s][x][y][0].set((imgWSide / INFORCE_ROWS) * x, ofMap(y, 0, 1, imgHSide *(1 - h), imgHSide)); // static (image can be dynamic)
				imageCoordinateSide[s][x][y][1].set((imgWSide / INFORCE_ROWS) * (x + 1), ofMap(y, 0, 1, imgHSide *(1 - h), imgHSide)); // static (image can be dynamic)

			}
		}
	}



	for (int s = 0; s < INFORCE_COLS; s++) {
		for (int x = 0; x< INFORCE_ROWS; x++) {
			pin_SideMesh[s][x].clear();
			pin_SideMesh[s][x].setMode(OF_PRIMITIVE_TRIANGLE_STRIP);


			float h_x = ofMap(heightsFromShapeDisplay[INFORCE_COLS - 1 - s][INFORCE_ROWS - 1 - x], pinHeight_MIN, pinHeight_MAX, vertexPinLow[s + x *(INFORCE_COLS + 1)].x, vertexPinHigh[s + x *(INFORCE_COLS + 1)].x);
			float h_y = ofMap(heightsFromShapeDisplay[INFORCE_COLS - 1 - s][INFORCE_ROWS - 1 - x], pinHeight_MIN, pinHeight_MAX, vertexPinLow[s + x *(INFORCE_COLS + 1)].y, vertexPinHigh[s + x *(INFORCE_COLS + 1)].y);

			float h_x2 = ofMap(heightsFromShapeDisplay[INFORCE_COLS - 1 - s][INFORCE_ROWS - 1 - x], pinHeight_MIN, pinHeight_MAX, vertexPinLow[s + (x + 1) *(INFORCE_COLS + 1)].x, vertexPinHigh[s + (x + 1) *(INFORCE_COLS + 1)].x);
			float h_y2 = ofMap(heightsFromShapeDisplay[INFORCE_COLS - 1 - s][INFORCE_ROWS - 1 - x], pinHeight_MIN, pinHeight_MAX, vertexPinLow[s + (x + 1) *(INFORCE_COLS + 1)].y, vertexPinHigh[s + (x + 1) *(INFORCE_COLS + 1)].y);



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
				float h = ofMap(heightsFromShapeDisplay[INFORCE_COLS - 1 - x][INFORCE_ROWS - 1 - s], pinHeight_MIN, pinHeight_MAX, 0, 1);
				imageCoordinateFront[s][x][y][0].set((imgWFront / INFORCE_COLS) * x, ofMap(y, 0, 1, imgHFront *(1 - h), imgHFront)); // static (image can be dynamic)
				imageCoordinateFront[s][x][y][1].set((imgWFront / INFORCE_COLS) * (x + 1), ofMap(y, 0, 1, imgHFront *(1 - h), imgHFront)); // static (image can be dynamic)

			}
		}
	}



	for (int s = 0; s < INFORCE_ROWS; s++) {
		for (int x = 0; x< INFORCE_COLS; x++) {
			pin_FrontMesh[s][x].clear();
			pin_FrontMesh[s][x].setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

			float h_x = ofMap(heightsFromShapeDisplay[INFORCE_COLS - 1 - x][INFORCE_ROWS - 1 - s], pinHeight_MIN, pinHeight_MAX, vertexPinLow[x + (s + 1) *(INFORCE_COLS + 1)].x, vertexPinHigh[x + (s + 1) *(INFORCE_COLS + 1)].x);
			float h_y = ofMap(heightsFromShapeDisplay[INFORCE_COLS - 1 - x][INFORCE_ROWS - 1 - s], pinHeight_MIN, pinHeight_MAX, vertexPinLow[x + (s + 1) *(INFORCE_COLS + 1)].y, vertexPinHigh[x + (s + 1) *(INFORCE_COLS + 1)].y);

			float h_x2 = ofMap(heightsFromShapeDisplay[INFORCE_COLS - 1 - x][INFORCE_ROWS - 1 - s], pinHeight_MIN, pinHeight_MAX, vertexPinLow[(x + 1) + (s + 1) *(INFORCE_COLS + 1)].x, vertexPinHigh[(x + 1) + (s + 1) *(INFORCE_COLS + 1)].x);
			float h_y2 = ofMap(heightsFromShapeDisplay[INFORCE_COLS - 1 - x][INFORCE_ROWS - 1 - s], pinHeight_MIN, pinHeight_MAX, vertexPinLow[(x + 1) + (s + 1) *(INFORCE_COLS + 1)].y, vertexPinHigh[(x + 1) + (s + 1) *(INFORCE_COLS + 1)].y);


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

void ProjectionMapper::loadCalibrationData() {
	if (XML.loadFile("projectionTest/calibration_projectionMapping_inForce.xml")) {
		message = "calibration_projectionMapping_inForce.xml loaded!";
	}
	else {
		message = "unable to load calibration_projectionMapping_inForce.xml check data/ folder";
	}

	cout << message << endl;


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




void ProjectionMapper::updateHighLowMesh() {
	for (int y = 0; y < INFORCE_ROWS + 1; y++) {
		for (int x = 0; x < INFORCE_COLS + 1; x++) {
			int index = x + y*(INFORCE_COLS + 1);
			highMesh.setVertex(index, vertexPinHigh[index]);
			lowMesh.setVertex(index, vertexPinLow[index]);
		}
	}

}




void ProjectionMapper::drawProjectionMapper() {
	ofPushMatrix();
	//ofBackground(0);
	ofDisableDepthTest();
	//ofBackground(100, 100, 100);
	ofTranslate(transX, transY);

	//ofTranslate(0,200);

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
		ofPopMatrix();

}

void ProjectionMapper::setPinTopImage(ofImage img) {
	mapImageTop.clear();
	mapImageTop = img;
}

void ProjectionMapper::setPinSideImage(ofImage img[INFORCE_COLS]) {
	
	for (int y = 0; y < INFORCE_COLS; y++) {
		mapImageSide[y].clear();
		mapImageSide[y] = img[y];
	}

}
void ProjectionMapper::setPinFrontImage(ofImage img[INFORCE_ROWS]) {
	for (int x = 0; x< INFORCE_ROWS; x++) {
		mapImageFront[x].clear();
		mapImageFront[x] = img[x];
	}
}