//
//  Particle Controller.h
//  KinectBasic
//
//  Created by Karen Peng on 8/6/14.
//
//

#include "cinder/Perlin.h"
#include "Particle.h"

using namespace ci;
using namespace std;

class ParticleController {
public:
  void setup();
  void update(float delta, Perlin &perlin, Vec2f movingPoint);
  void draw();
  void addParticles(int amt, const Vec2f &position, const Vec2f &velocity);
  void removeParticles(int amt);
  
  list<Particle*> mParticles;
};