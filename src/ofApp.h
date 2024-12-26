#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Emitter.h"
#include "Shape.h"
#include "Sprite.h"
#include "ParticleEmitter.h"
#include "Particle.h"
#include "../addons/ofxCenteredTrueTypeFont.h"



class Agent : public Sprite {
public:
	Agent() {
		Sprite::Sprite();
//		cout << "in Agent Constuctor" << endl;
	}
};

class AgentEmitter : public Emitter {
public:
	void spawnSprite();
	void moveSprite(Sprite*);
};

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	bool left, right, up, down;

	Emitter* emitter = NULL;
	Sprite* target = NULL;
	ParticleEmitter* explosionEff = NULL;
	ImpulseRadialForce* radialForce = NULL;

	ofImage defaultImage;
	ofImage mainMenuBG;
	ofImage gameBG;
	ofImage endBG;
	ofVec3f mouse_last;
	bool imageLoaded;
	bool bDrag = false;
	glm::vec3 lastMousePos;
	bool showImage = false;
	bool helpPage = false;
	bool diffSetup = false;
	bool fire = false;
	bool collided = false;
	bool displayTime = false;
	int elapsedTime = 0;
	int timer = 0;
	bool bThrust = false;
	// Some basic UI
	//
	bool bHide;
	ofxFloatSlider rate;
	ofxFloatSlider life;
	ofxVec3Slider velocity;
	ofxLabel screenSize;	
	ofxFloatSlider scale;
	ofxFloatSlider targetVelocity;
	ofxFloatSlider targetScale;

	ofTrueTypeFont sans;
	ofxCenteredTrueTypeFont main;
	char fps[255];
	char energy[255];
	char time[255];
	char easy[255];
	char hard[255];
	char lunatic[255];
	char help[255];
	char helpMessage[255];
	char endMessage[255];
	char endGuide[255];

	ofxPanel gui;
	ofSoundPlayer hitSound;
	ofSoundPlayer mainMenu;
	ofSoundPlayer select;
	ofSoundPlayer selected;
	ofSoundPlayer death;
	ofSoundPlayer explo;
	ofSoundPlayer esc;
	ofSoundPlayer failed;
	ofSoundPlayer easyMusic;
	ofSoundPlayer hardMusic;
	ofSoundPlayer lunaticMusic;
	ofSoundPlayer engineSound;

	string gameState = "start";
	string difficulty = "Easy";

};
