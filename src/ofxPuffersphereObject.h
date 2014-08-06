#pragma once
#include "ofMain.h"


class ofxPuffersphereObject{

	  public:
		ofxPuffersphereObject();
		void makeTransparent();
	
		void setup(float dimenions = 1050);
		void draw();
	
		void setTexture(ofBaseHasTexture* newTexture);
	
		void loadImage(string filename);
		void loadVideo(string filename);

		ofVec2f targetSize;
		ofQuaternion rotation;
		ofBaseHasTexture* image; //can be an ofFbo or ofImage, or any other drawable
		ofShader* offaxis; //please share me between all other sphere objects
    
		ofFloatColor tint; // can be used to fade object out
		ofVec2f scale;
	
	  private:
		bool isTransparent();
		bool textureIsOurs; //flagged true if we created the texture, so we know to destroy it
		vector<ofVec2f> canvasCoords;
		//	string isA;	// remember the original type of the object.

};
