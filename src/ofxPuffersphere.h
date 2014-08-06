#pragma once
#include "ofxPuffersphereObject.h"
#include "ofxGui.h"


typedef struct {
    ofParameter<float> fov;
    ofParameter<float> Z;
    ofParameter<ofVec3f> shaderLensCorrection;
} SpherizeShaderParams;


class ofxPuffersphere{

	  public:
		ofxPuffersphere();
		~ofxPuffersphere();

		void setup(float size = 1050); //some are 1050, some are 1080

		void addGuiParams();

		//updates the current render canvas
		void render();

		//if you have elements you want to manually render do begin, then end
		void begin();
		void renderAllObjects(); //just renders the elements, don't call this outside of BEGIN/END
		void end();

		//use this for drawing to the puffersphere itself
		void draw();

		//let's you draw the sphere in 3d space to preview what it will look like on the sphere
		void drawSphere(ofVec3f position, float scale = 100);

		string spherizeLocation;
		string offaxisLocation;
		ofShader* getShaderReference(); //return ref. to the offaxis location

		void addObject(ofxPuffersphereObject* object);
		ofxPuffersphereObject* createObject();
		ofxPuffersphereObject* createObject(string imagePath);
		// creates video object. class does not take care of update/play/pause/loop etc.
		ofxPuffersphereObject* createObjectFromVideoPath(string videoPath);

		ofFbo * getCanvasRef(){ return &canvas; }

		float getSize();

	public:
		ofxPanel gui;

	  private:
		ofFbo canvas;
		bool shaderLoaded;
		bool hasBegun;
		ofParameter<bool> renderForPuffersphere;

		float size;
		ofShader offaxis;
		ofShader spherize;

		SpherizeShaderParams sphereShaderSettings;

		vector<ofxPuffersphereObject*> objects;


};
