#include "ofxPuffersphere.h"


ofxPuffersphere::ofxPuffersphere(){
	offaxisLocation = "offaxis";
	spherizeLocation = "spherize";
	hasBegun = false;
	shaderLoaded = false;
	renderForPuffersphere = false;
}

ofxPuffersphere::~ofxPuffersphere(){
	for(int i = 0; i < objects.size(); i++){
		delete objects[i];
	}
	objects.clear();
}

void ofxPuffersphere::setup(float s){
	size = s;
	canvas.allocate(size*2, size, GL_RGBA);

	//TODO: collapse shaders into one
	offaxis.load(offaxisLocation);	 
 	spherize.load(spherizeLocation);
}


void ofxPuffersphere::addGuiParams(){
	ofxGuiSetDefaultWidth(320);
	ofxGuiSetDefaultHeight(30);

	gui.setHeaderBackgroundColor(ofColor::darkRed);
	gui.setup("Custom Pufferspher Settings","customPufferspherSetting.xml");

	gui.add(renderForPuffersphere.set("Render For Puffersphere",false));
	gui.add(sphereShaderSettings.fov.set("fov",1.0,1.0,359.0));
	gui.add(sphereShaderSettings.Z.set("Z",0.0,0.0,1000.0));
	gui.add(sphereShaderSettings.shaderLensCorrection.set("Lens Correction",ofVec3f(0.8,0.0012,0.15),ofVec3f(0.8,0.0012,0.15),ofVec3f(1.5,0.1212,0.44)));

	gui.loadFromFile("customPufferspherSetting.xml");
}

ofxPuffersphereObject* ofxPuffersphere::createObject(){
	ofxPuffersphereObject* object = new ofxPuffersphereObject();
	object->offaxis = getShaderReference();
	object->setup(size);
	objects.push_back( object );
	return object;
}

ofxPuffersphereObject* ofxPuffersphere::createObject(string imagePath){
	ofxPuffersphereObject* object = createObject();
	object->loadImage(imagePath);
	return object;
}

ofxPuffersphereObject* ofxPuffersphere::createObjectFromVideoPath(string videoPath){
	ofxPuffersphereObject* object = createObject();
	object->loadVideo(videoPath);
	return object;
}

//return ref. to the offaxis location
ofShader* ofxPuffersphere::getShaderReference(){
	return &offaxis;
}

void ofxPuffersphere::render(){
	begin();
	ofClear(0, 0, 0, 1.0);
	renderAllObjects();
	end();
}

void ofxPuffersphere::begin(){
	if(!hasBegun){
		hasBegun = true;
		canvas.begin();
		ofClear(255, 255, 255, 255);
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_DEPTH_TEST);
        ofSetColor(255,255,255);
	}
}

void ofxPuffersphere::end(){
	if(hasBegun){
		glPopAttrib();
		canvas.end();
		hasBegun = false;
	}
}

void ofxPuffersphere::renderAllObjects(){
	for(int i = 0; i < objects.size(); i++){
		objects[i]->draw();
	}

}

void ofxPuffersphere::draw(){
	ofPushStyle();
	ofSetColor(255);
	//draws for puffersphere as target
	if(renderForPuffersphere){
		spherize.begin();
		spherize.setUniform2f("imageSize", float(canvas.getWidth()), float(canvas.getHeight()));
		spherize.setUniform1f("fov", sphereShaderSettings.fov);
		spherize.setUniform1f("Z", sphereShaderSettings.Z);
		spherize.setUniform3f("lensCorr", sphereShaderSettings.shaderLensCorrection.get().x,sphereShaderSettings.shaderLensCorrection.get().y,sphereShaderSettings.shaderLensCorrection.get().z);
		
		//centered
		canvas.draw(ofGetWidth()/2.-canvas.getWidth()/2., ofGetHeight()/2.-canvas.getHeight()/2.);
		spherize.end();
	}
	//draw flat for preview on screen
	else {
		ofPushMatrix();
		//TODO: scale viewport
		glScalef(.5, .5, .5);
		canvas.draw(0, 0);
		ofPopMatrix();
	}
	ofPopStyle();
}

float ofxPuffersphere::getSize(){
	return size;
}

void ofxPuffersphere::drawSphere(ofVec3f position, float scale){

    float radius = scale;
	int segments = 100;

	// tig: this was lifted from Cinder.
	// https://github.com/cinder/Cinder/blob/master/src/cinder/gl/gl.cpp

	// http://local.wasp.uwa.edu.au/~pbourke/texture_colour/spheremap/  Paul Bourke's sphere code
	// We should weigh an alternative that reduces the batch count by using GL_TRIANGLES instead
	if( segments < 0 )
		return;

    canvas.getTextureReference(0).bind();

	float * verts = new float[(segments+1)*2*3];
	float * normals = new float[(segments+1)*2*3];
	float * texCoords = new float[(segments+1)*2*2];

	// TG. not pushing/popping this bit leads to weird things in the fbo drawn after
	glPushAttrib(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    ofDisableAlphaBlending();

	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, 0, verts );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
	glEnableClientState( GL_NORMAL_ARRAY );
	glNormalPointer( GL_FLOAT, 0, normals );

	for( int j = 0; j < segments / 2; j++ ) {
		float theta1 = j * 2 * 3.14159f / segments - ( 3.14159f / 2.0f );
		float theta2 = (j + 1) * 2 * 3.14159f / segments - ( 3.14159f / 2.0f );

		for( int i = 0; i <= segments; i++ ) {
			ofVec3f e, p;
			float theta3 = i * 2 * 3.14159f / segments;

			e.x = cos( theta1 ) * cos( theta3 );
			e.y = sin( theta1 );
			e.z = cos( theta1 ) * sin( theta3 );
			p = e * radius + position;
			normals[i*3*2+0] = e.x; normals[i*3*2+1] = e.y; normals[i*3*2+2] = e.z;
			texCoords[i*2*2+0] = (1.0 - (i / (float)segments))*canvas.getWidth();
            texCoords[i*2*2+1] = (0.999f - 2 * j / (float)segments)*canvas.getHeight();
			verts[i*3*2+0] = p.x; verts[i*3*2+1] = p.y; verts[i*3*2+2] = p.z;

			e.x = cos( theta2 ) * cos( theta3 );
			e.y = sin( theta2 );
			e.z = cos( theta2 ) * sin( theta3 );
			p = e * radius + position;
			normals[i*3*2+3] = e.x; normals[i*3*2+4] = e.y; normals[i*3*2+5] = e.z;
			texCoords[i*2*2+2] = (1.0 - (i / (float)segments)) * canvas.getWidth();
            texCoords[i*2*2+3] = (0.999f - 2 * ( j + 1 ) / (float)segments) * canvas.getHeight();
			verts[i*3*2+3] = p.x; verts[i*3*2+4] = p.y; verts[i*3*2+5] = p.z;
		}
		glDrawArrays( GL_TRIANGLE_STRIP, 0, (segments + 1)*2 );
	}

    canvas.getTextureReference(0).unbind();

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	ofEnableAlphaBlending();

	glPopAttrib();


}
