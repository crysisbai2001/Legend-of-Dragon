#pragma once
//  Kevin M. Smith - CS 134 SJSU

#include "ofMain.h"
#include "Particle.h"


//  Pure Virtual Function Class - must be subclassed to create new forces.
//
class ParticleForce {
protected:
public:
	bool applyOnce = false;
	bool applied = false;
	virtual void updateForce(Particle *) = 0;
};

class ParticleSystem {
public:
	void add(const Particle &);
	void addForce(ParticleForce *);
	void remove(int);
	void update();
	void setLifespan(float);
	void reset();
	int removeNear(const ofVec3f & point, float dist);
	void draw();
	vector<Particle> particles;
	vector<ParticleForce *> forces;
};



// Some convenient built-in forces
//
class GravityForce: public ParticleForce {
	ofVec3f gravity;
public:
	GravityForce(const ofVec3f & gravity);
	void updateForce(Particle *);
	void set(const ofVec3f& gravity);
};

class TurbulenceForce : public ParticleForce {
	ofVec3f tmin, tmax;
public:
	TurbulenceForce(const ofVec3f & min, const ofVec3f &max);
	void updateForce(Particle *);
	void set(const ofVec3f& min, const ofVec3f& max);
};

class ImpulseRadialForce : public ParticleForce {
	float magnitude;
	ofVec3f dir;
public:
	ImpulseRadialForce(float magnitude); 
	void updateForce(Particle *);
	void set(float magnitude);
};

class RadialForce : public ParticleForce {
	float magnitude;
	ofVec3f dir;
public:
	RadialForce(float magnitude);
	void updateForce(Particle*);
	void set(float magnitude);
};

class CyclicForce : public ParticleForce {
	float magnitude;
public:
	CyclicForce(float magnitude);
	void updateForce(Particle*);
	void set(float magnitude);
};
