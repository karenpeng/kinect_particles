//
//  Particle Controller.cpp
//  KinectBasic
//
//  Created by Karen Peng on 8/6/14.
//
//

#include "Particle Controller.h"

#include "cinder/CinderMath.h"
#include "cinder/Rand.h"

#define SPRAY 128.0f
#define MAX_VELOCITY 16.0f
#define LEFTDRIFT 1.0f

void ParticleController::setup() {
}

void ParticleController::addParticles(int amt, const Vec2f &position, const Vec2f &velocity) {
  Vec2f clampedVelocity = Vec2f(cinder::math<float>::clamp(velocity.x, -MAX_VELOCITY, MAX_VELOCITY),
                                cinder::math<float>::clamp(velocity.y, -MAX_VELOCITY, MAX_VELOCITY));
  int max = amt;
  
  for (int i = 0; i < max; i++) {
    
    Vec2f offset = SPRAY * powf(Rand::randFloat(), 2.0f) * Rand::randVec2f();
    Vec2f vel = clampedVelocity * 0.5 + randVec2f();
   // vel.x -= LEFTDRIFT;
    
    Particle *p = new Particle();
    //p->setup(position + offset, vel);
    p->setup(position, velocity);
    
    mParticles.push_back(p);
  }
}

void ParticleController::update(float delta, Perlin &perlin, Vec2f movingPoint ) {
  for (std::list<Particle*>::iterator i = mParticles.begin(); i != mParticles.end();) {
    Particle *p = (*i);
    if (p->mIsDead || p->mPosition.x>1280.0 || p->mPosition.x<640.0 || p->mPosition.y<0.0 || p->mPosition.y>480.0) {
      i = mParticles.erase(i);
      delete p;
    } else {
      p->update(delta, perlin);
      //Vec2f seekForce= p->seek((aPoints));
      //p->addForce(seekForce);
      Vec2f separateForce = p->separate(movingPoint);
      p->addForce(separateForce);
      ++i;
    }
  }
}

void ParticleController::draw() {
  for (std::list<Particle*>::iterator p = mParticles.begin(); p != mParticles.end(); p++) {
    (*p)->draw();
  }
}
