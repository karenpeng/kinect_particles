//
//  Particle.h
//  KinectBasic
//
//  Created by Karen Peng on 8/6/14.
//
//


#include "cinder/gl/gl.h"
#include "cinder/Vector.h"
#include "cinder/Rand.h"
#include "cinder/Perlin.h"
#include "cinder/Color.h"


using namespace ci;
using namespace std;

class Particle {
public:
  
  Particle();
  ~Particle();
  
  enum Shape {
    TRIANGLE,
    SQUARE,
    HEXAGON,
    CIRCLE
  };
  //static const Color Colors[];
  static const Color myColors[5][4];
  
  static const Shape Shapes[];
  
  void setup(const Vec2f &position, const Vec2f &velocity);
  void update(float delta, Perlin &perlin);
  void addForce(Vec2f aForce);
  Vec2f seek(vector<Vec2f> aPoints);
  Vec2f separate(Vec2f movingPoint);
  void draw();
  
  Vec3f mPosition;
  Vec3f mVelocity;
  Vec3f mAcceleration;
  float maxSpeed;
  int seekIndex;
  
  float mRadius;
  float mVecolityDecay;
  list<Color> mColors;
  
  int mLifespan;
  int mAge;
  int mFlapLifespan;
  bool mIsDead;
  
  Color mColorA;
  Color mColorB;
  
  void drawArc(const Vec2f &center, const float radius, const float start, const float stop, const int numSegments);
  
  Shape mShape;
  
private:
  static Color colorFromInt(const uint32_t rgb);
};

