#include "ofApp.h"

// spawnSprite - we override this function in the Emitter class to spawn our 
// "custom" Agent sprite.
// Written by Kevin Smith and Yingchen Bai, SJSU CS134 2023

void AgentEmitter::spawnSprite() {

	Agent sprite;
	if (haveChildImage) sprite.setImage(childImage);
	sprite.velocity = velocity;
	sprite.lifespan = lifespan;
	sprite.pos = glm::vec3(ofRandom(ofGetWindowWidth()), ofRandom(ofGetWindowHeight()), 0.0);
	sprite.rot = rot;
	sprite.birthtime = ofGetElapsedTimeMillis();
	sprite.emit = new ParticleEmitter();
	sys->add(sprite);
}

//  moveSprite - we override this function in the Emitter class to implment
//  "following" motion towards the target
//
void AgentEmitter::moveSprite(Sprite* sprite) {
	
	// Get a pointer to the app so we can access application
	// data such as the target sprite.
	//
	ofApp* theApp = (ofApp*)ofGetAppPtr();

	// rotate sprite to point towards target
	//  - find vector "v" from sprite to target
	//  - set rotation of sprite to align with v
	//
	glm::vec3 v = glm::normalize(theApp->target->pos - sprite->pos);
	glm::vec3 u = glm::vec3(0, -1, 0);
	float a = glm::orientedAngle(u, v, glm::vec3(0, 0, 1));
	//sprite->rot = glm::degrees(a);
	//cout << "rot: "<< sprite->rot << '\n';
	//cout << "a: " << glm::degrees(a) << '\n';
	if (abs(sprite->rot - glm::degrees(a)) > 10) {
		if (sprite->rot < glm::degrees(a)) {
			sprite->angularForce = 3;
		}
		else if (sprite->rot > glm::degrees(a)) {
			sprite->angularForce = -3;
		}
	}
	// Calculate new velocity vector
	// with same speed (magnitude) as the old one but in direction of "v"
	// 
	//sprite->velocity = glm::length(sprite->velocity) * v;
	sprite->force = sprite->heading() * 500 * 2;
	sprite->integrate();

	// Now move the sprite in the normal way (along velocity vector)
	//
	//Emitter::moveSprite(sprite);

}
// Example of how to move 10 units along the heading
//



//--------------------------------------------------------------
void ofApp::setup() {


	ofSetVerticalSync(true);
	hitSound.load("sound/metal pipe.mp3");
	hitSound.setMultiPlay(true);
	death.load("sound/death.mp3");
	explo.load("sound/explo.mp3");
	explo.setMultiPlay(true);
	mainMenu.load("sound/SA Main Menu.mp3");
	mainMenu.play();	
	mainMenu.setVolume(0.9f);
	mainMenu.setLoop(true);
	easyMusic.load("sound/easyMusic.mp3");
	easyMusic.setLoop(true);
	hardMusic.load("sound/hardMusic.mp3");
	hardMusic.setLoop(true);
	lunaticMusic.load("sound/lunaticMusic.mp3");
	lunaticMusic.setLoop(true);
	failed.load("sound/failed.mp3");
	failed.setLoop(true);
	select.load("sound/select.wav");
	select.setMultiPlay(true);
	selected.load("sound/selected.wav");
	esc.load("sound/esc.wav");
	engineSound.load("sound/thrusters-loop.wav");
	engineSound.setMultiPlay(true);
	engineSound.setLoop(true);


	sans.load("font/Sans.ttf", 20);
	main.load("font/Main Menu.ttf", 50);
	// create an image for sprites being spawned by emitter
	//
	
	ofSetBackgroundColor(ofColor::black);

	mainMenuBG.load("images/main menu.jpg");
	gameBG.load("images/touhou.jpg");
	endBG.load("images/end screen.png");

	
	radialForce = new ImpulseRadialForce(100000);
	// create an emitter
	//

	emitter = new AgentEmitter();  // C++ polymorphism

	emitter->pos = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0);
	emitter->drawable = false;
	emitter->setShowImage(showImage);	
	emitter->start();

	// create a target sprite to chase
	//
	target = new Sprite();
	target->pos = glm::vec3(400, 100, 0);
	target->nEnergy = 5;
	target->bShowImage = showImage;
	target->emit = new ParticleEmitter();
	target->emit->setEmitterType(DirectionalEmitter);
	
	explosionEff = new ParticleEmitter();
	explosionEff->setEmitterType(RadialEmitter);
	explosionEff->setLifespan(5);
	explosionEff->sys->addForce(radialForce);
	explosionEff->setRate(100);
	explosionEff->setGroupSize(10);

	ofImage targetImage;
	if (!targetImage.load("images/long.jpg")) {
		cout << "can't load image" << endl;
		ofExit(0);

	}
	target->setImage(targetImage);

	if (defaultImage.load("images/zimin.jpg")) {
		imageLoaded = true;
	}
	else {
		cout << "Can't open image file" << endl;
		//			ofExit();
	}

	if (imageLoaded) emitter->setChildImage(defaultImage);

	gui.setup();
	gui.add(rate.setup("rate", 1, 1, 10));
	gui.add(life.setup("life", 5, .1, 10));
	gui.add(velocity.setup("velocity", ofVec3f(0, -200, 0), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));
	gui.add(scale.setup("Scale", .5, .1, 1.0));

	gui.add(targetVelocity.setup("Player Velocity", 1, 0, 5));
	gui.add(targetScale.setup("Player Scale", 1, 0, 5));

	bHide = false;
	diffSetup = true;
	displayTime = true;
}

//--------------------------------------------------------------
void ofApp::update() {
	
	if (gameState == "start") {
		
	}
	else if (gameState == "game") {
		if (diffSetup) {
			if (difficulty == "Easy") {
				rate = 1;
				target->emit->rate = 5;
			}
			else if (difficulty == "Hard") {
				rate = 3;
				target->nEnergy = 3;
				target->emit->rate = 3;
			}
			else if (difficulty == "Lunatic") {
				rate = 5;
				target->nEnergy = 1;
				target->emit->rate = 1;
			}
		}

		diffSetup = false;
		emitter->setRate(rate);
		emitter->setLifespan(life * 1000);    // convert to milliseconds 
		emitter->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
		emitter->update();
		target->emit->update();
		explosionEff->sys->update();
		emitter->setShowImage(showImage);
		target->bShowImage = showImage;
		collided = false;


		//for (int k = 0; k < 10; k++) {
			//explosionEff->spawn(0);
		//}

		for (int i = 0; i < emitter->sys->sprites.size(); i++) {

			// Get values from sliders and update sprites dynamically
			//
			float sc = scale;

			emitter->sys->sprites[i].scale = glm::vec3(sc, sc, sc);
			if (emitter->sys->sprites[i].inside(target->pos) || target->inside(emitter->sys->sprites[i].pos)) {
				cout << "hit" << endl;
				if (target->nEnergy > 1) {
					hitSound.play();
				}
				collided = true;
				emitter->sys->sprites.erase(emitter->sys->sprites.begin() + i);
				target->nEnergy--;
			}

			if (!collided) {
				for (int j = 0; j < target->emit->sys->particles.size(); j++) {
					if (emitter->sys->sprites[i].inside(target->emit->sys->particles[j].position)) {
						cout << "enemy hit" << endl;
						explosionEff = new ParticleEmitter();
						explosionEff->setEmitterType(RadialEmitter);
						explosionEff->setLifespan(5);
						explosionEff->setRate(100);
						explosionEff->setGroupSize(10);
						for (int k = 0; k < 10; k++) {
							explosionEff->spawn(ofGetElapsedTimeMillis());
						}
						for (int k = 0; k < explosionEff->sys->particles.size(); k++) {
							explosionEff->sys->particles[k].position.set(target->emit->sys->particles[j].position);

							//cout << explosionEff->sys->particles[k].position << endl;
							//cout << emitter->sys->sprites[i].pos << endl;
						}
						radialForce = new ImpulseRadialForce(100000);
						explosionEff->sys->addForce(radialForce);
						explosionEff->start();
						explo.play();
						target->nEnergy++;
						emitter->sys->sprites.erase(emitter->sys->sprites.begin() + i);
						target->emit->sys->remove(j);
					}
				}
			}
				
		}
		if (ofGetElapsedTimef() - explosionEff->lastSpawned * 1000 > 5) {
			explosionEff->stop();
			explosionEff->sys->reset();
		}
		//for (int i = 0; i < explosionEff->sys->particles.size(); i++) {			
			//cout << explosionEff->sys->particles[i].position << endl;
		//}

		float ts = targetScale;

		target->scale = glm::vec3(ts, ts, ts);

		if (target->nEnergy <= 0) {
			death.play();
			death.setVolume(0.8f);
			gameState = "end";
			easyMusic.stop();
			hardMusic.stop();
			lunaticMusic.stop();
			failed.play();
			//ofExit();
		}

		if (target->pos.x > ofGetWindowWidth()) {
			target->pos.x = ofGetWindowWidth();
		}

		if (target->pos.x < 0) {
			target->pos.x = 0;
		}

		if (target->pos.y > ofGetWindowHeight()) {
			target->pos.y = ofGetWindowHeight();
		}

		if (target->pos.y < 0) {
			target->pos.y = 0;
		}
		if (left)   // turn left
			target->angularForce = -5;
		if (right)  // turn right
			target->angularForce = 5;
		if (up) {    // go forward
			target->force = target->heading() * 500 * 5;
			bThrust = true;
		}
		if (down) {  // go backward
			target->force = -target->heading() * 500 * 5;
			bThrust = true;
		}
		if (!up && !down)
			bThrust = false;

		target->integrate();
		if (fire) {
			for (int i = 0; i < target->emit->sys->particles.size(); i++) {
				if (target->emit->sys->particles[i].isFired == false) {
					target->emit->sys->particles[i].position = target->pos;
					target->emit->sys->particles[i].isFired = true;
					target->emit->sys->particles[i].damping = 1;
					target->emit->sys->particles[i].velocity = target->heading() * 500 * 5;

				}

			}

		}	

		if (bThrust && !engineSound.isPlaying()) {
			engineSound.play();
		}
		else if (!bThrust && engineSound.isPlaying()) {
			engineSound.stop();
		}
		
	}
	else if (gameState == "end") {

	}
	
}




//--------------------------------------------------------------
void ofApp::draw(){
	if (gameState == "start") {
		mainMenuBG.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
		sprintf(easy, "Easy");		
		sprintf(hard, "Hard");
		sprintf(lunatic, "Lunatic");
		sprintf(help, "Help");
		if (!helpPage) {
			if (difficulty == "Easy") {
				ofSetHexColor(0xff0000);
				main.drawString(easy, 100, ofGetWindowHeight() / 2 + 150);
				ofSetHexColor(0xffffff);
				main.drawString(hard, 100, ofGetWindowHeight() / 2 + 250);
				main.drawString(lunatic, 100, ofGetWindowHeight() / 2 + 350);
				main.drawString(help, 100, ofGetWindowHeight() / 2 + 450);
			}
			else if (difficulty == "Hard") {
				ofSetHexColor(0xff0000);
				main.drawString(hard, 100, ofGetWindowHeight() / 2 + 250);
				ofSetHexColor(0xffffff);
				main.drawString(easy, 100, ofGetWindowHeight() / 2 + 150);
				main.drawString(lunatic, 100, ofGetWindowHeight() / 2 + 350);
				main.drawString(help, 100, ofGetWindowHeight() / 2 + 450);
			}
			else if (difficulty == "Lunatic") {
				ofSetHexColor(0xff0000);
				main.drawString(lunatic, 100, ofGetWindowHeight() / 2 + 350);
				ofSetHexColor(0xffffff);
				main.drawString(easy, 100, ofGetWindowHeight() / 2 + 150);
				main.drawString(hard, 100, ofGetWindowHeight() / 2 + 250);
				main.drawString(help, 100, ofGetWindowHeight() / 2 + 450);
			}
			else if (difficulty == "Help") {
				ofSetHexColor(0xff0000);
				main.drawString(help, 100, ofGetWindowHeight() / 2 + 450);
				ofSetHexColor(0xffffff);
				main.drawString(easy, 100, ofGetWindowHeight() / 2 + 150);
				main.drawString(hard, 100, ofGetWindowHeight() / 2 + 250);
				main.drawString(lunatic, 100, ofGetWindowHeight() / 2 + 350);
			}

		}
		else {
			sprintf(helpMessage, "Space: Confirm");
			main.drawString(helpMessage, 100, ofGetWindowHeight() / 2 + 150);
			sprintf(helpMessage, "Shift: Main Menu");
			main.drawString(helpMessage, 100, ofGetWindowHeight() / 2 + 250);
			sprintf(helpMessage, "Arrows: Move");
			main.drawString(helpMessage, 100, ofGetWindowHeight() / 2 + 350);
			sprintf(helpMessage, "T: Toggle Sprites");
			main.drawString(helpMessage, 100, ofGetWindowHeight() / 2 + 450);
		}
		

	}
	else if (gameState == "game") {
		ofSetColor(ofColor::white);
		gameBG.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
		
		

		emitter->draw();
		target->draw();
		for (int i = 0; i < target->emit->sys->particles.size(); i++) {
			ofSetColor(ofColor::red);
			ofDrawCircle(target->emit->sys->particles[i].position, 8);
		}
		for (int i = 0; i < explosionEff->sys->particles.size(); i++) {
			ofSetColor(ofColor::yellow);
			ofDrawCircle(explosionEff->sys->particles[i].position, 4);
			//cout << explosionEff->sys->particles[i].position << endl;
		}
		


		ofSetColor(ofColor::white);	
		sprintf(fps, "%d FPS", (int)ofGetFrameRate());
		sans.drawString(fps, 30, ofGetWindowHeight() - 50);
		sprintf(time, "%ds", (int)ofGetElapsedTimeMillis() / 1000 - timer);
		sans.drawString(time, 30, ofGetWindowHeight() - 80);
		sprintf(energy, "Energy: %d", target->nEnergy);
		sans.drawString(energy, 30, ofGetWindowHeight() - 110);

		if (!bHide) {
			gui.draw();
		}
	}
	else if (gameState == "end") {
		if (displayTime) {
			elapsedTime = ofGetElapsedTimeMillis() / 1000;
			displayTime = false;
		}
		ofSetColor(ofColor::white);
		endBG.draw(ofGetWindowWidth() / 2 - endBG.getWidth() / 2, ofGetWindowHeight() / 2 - endBG.getHeight() / 2 - 100);
		const char* buffer = difficulty.c_str();
		sprintf(endMessage, "You lasted %ds on %s", elapsedTime - timer, buffer);
		sprintf(endGuide, "Space/Shift to restart");
		main.load("font/Main Menu.ttf", 30);
		main.drawStringCenteredHorizontally(endMessage, ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 + 80);
		if (ofGetElapsedTimeMillis() / 1000 % 2 == 0) {
			main.drawStringCenteredHorizontally(endGuide, ofGetWindowWidth() / 2, ofGetWindowHeight() * 3 / 4);
		}

	}
}


//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
//	cout << "mouse( " << x << "," << y << ")" << endl;


}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if (bDrag) {
		glm::vec3 p = glm::vec3(x, y, 0);
		glm::vec3 delta = p - lastMousePos;
		target->pos += delta;
		lastMousePos = p;
	}

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	glm::vec3 pos = glm::vec3(x, y, 0);
	if (target->inside(pos)) {
		bDrag = true;
		lastMousePos = pos;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	bDrag = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

void ofApp::keyPressed(int key) {
	switch (key) {
	case 'C':
	case 'c':
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		bHide = !bHide;
		break;
	case 't':
	case 'T':
		showImage = !showImage;
		break;
	case 'r':
		break;
	case 's':
		fire = true;
		target->emit->start();
		//target->emit->spawn(0);
		//for (int i = 0; i < target->emit->sys->particles.size(); i++) {
			//if (target->emit->sys->particles[i].isFired == false) {
				//target->emit->sys->particles[i].position = target->pos;
				//target->emit->sys->particles[i].isFired = true;
				//target->emit->sys->particles[i].forces += target->heading() * 500;
				//cout << target->emit->sys->particles[i].position << endl;
				//cout << target->emit->sys->particles[i].velocity << endl;
			//}
		//}
		break;
	case 'u':
		break;
	case OF_KEY_UP:
		if (gameState == "start") {
			if (difficulty == "Easy") {
				difficulty = "Help";
			}
			else if (difficulty == "Hard") {
				difficulty = "Easy";
			}
			else if (difficulty == "Lunatic") {
				difficulty = "Hard";
			}
			else if (difficulty == "Help") {
				difficulty = "Lunatic";
			}
			select.play();
		}
		else if (gameState == "game") {
			up = true;
		}
		break;
	case OF_KEY_DOWN:
		if (gameState == "start") {
			if (difficulty == "Easy") {
				difficulty = "Hard";
			}
			else if (difficulty == "Hard") {
				difficulty = "Lunatic";
			}
			else if (difficulty == "Lunatic") {
				difficulty = "Help";
			}
			else if (difficulty == "Help") {
				difficulty = "Easy";
			}
			select.play();
		}
		else if (gameState == "game") {
			down = true;
		}
		break;
	case OF_KEY_LEFT:
		if (gameState == "game") {
			left = true;
		}
		break;
	case OF_KEY_RIGHT:
		if (gameState == "game") {
			right = true;
		}
		break;
	case ' ':
		cout << gameState << endl;
		if (gameState == "start") {
			if (difficulty != "Help") {
				mainMenu.stop();
				selected.play();
				gameState = "game";
				if (difficulty == "Easy") {
					easyMusic.play();
					easyMusic.setVolume(0.5f);
				}
				else if (difficulty == "Hard") {
					hardMusic.play();
					hardMusic.setVolume(0.5f);
				}
				else if (difficulty == "Lunatic") {
					lunaticMusic.play();
					lunaticMusic.setVolume(0.5f);
				}
				timer = ofGetElapsedTimeMillis() / 1000;
			}
			else {
				helpPage = true;
				selected.play();
			}
		}
		else if (gameState == "end") {
			failed.stop();
			esc.play();
			gameState = "start";
			setup();
		}
		break;
	case OF_KEY_SHIFT:
		if (gameState == "game" || gameState == "end") {
			esc.play();
			failed.stop();
			easyMusic.stop();
			hardMusic.stop();
			lunaticMusic.stop();
			gameState = "start";
			setup();
		}
		if (helpPage) {
			helpPage = false;
		}
		break;
	case OF_KEY_ALT:
		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_DEL:
		break;
	}
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
	case OF_KEY_LEFT:
		left = false;
		break;
	case OF_KEY_RIGHT:
		right = false;
		break;
	case OF_KEY_UP:
		up = false;
		break;
	case OF_KEY_DOWN:
		down = false;
		break;
	case ' ':
	case 't':
	case 'T':
	case 's':
		fire = false;
		target->emit->stop();
		break;
	case OF_KEY_ESC:
		break;
	case OF_KEY_ALT:
		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_SHIFT:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

