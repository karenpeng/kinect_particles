//
//  Particle.cpp
//  KinectBasic
//
//  Created by Karen Peng on 8/6/14.
//
//

#include "Particle.h"
#include "cinder/Color.h"
#include "cinder/app/App.h"
#include "cinder/Vector.h"
#include "cinder/CinderMath.h"

#define RADIUS_MIN 12.0f
#define RADIUS_MAX 32.0f
#define RADIUS_POW 2.0f
#define FLAP_LIFESPAN 70

#define   windowWidth 1280
#define   windowHeight 640

#define SEPARATE_DIST_THRESHOLD 200

const Particle::Shape Particle::Shapes[] = {
  Particle::Shape::TRIANGLE,
  Particle::Shape::SQUARE,
  Particle::Shape::HEXAGON,
  Particle::Shape::CIRCLE
};

using namespace ci;
using namespace ci::app;

const Color Particle::myColors[5][4] = {
  {
    Particle::colorFromInt(0x5133A5),
    Particle::colorFromInt(0x673FB4),
    Particle::colorFromInt(0x6BFEDA),
    Particle::colorFromInt(0x96FEDA)
  },
  
  {
    Particle::colorFromInt(0x7A27A0),
    Particle::colorFromInt(0x9A2EAE),
    Particle::colorFromInt(0xFC5457),
    Particle::colorFromInt(0xFF5353)
  },
  {
    Particle::colorFromInt(0x1688CE),
    Particle::colorFromInt(0x1DAAF1),
    Particle::colorFromInt(0x34FFFE),
    Particle::colorFromInt(0x19FFFF)
  },
  {
    Particle::colorFromInt(0x1797A6),
    Particle::colorFromInt(0x1FBCD2),
    Particle::colorFromInt(0xEEFD54),
    Particle::colorFromInt(0xEEFF41)
  },
  {
    Particle::colorFromInt(0xF37C22),
    Particle::colorFromInt(0xFD9727),
    Particle::colorFromInt(0xFC4482),
    Particle::colorFromInt(0xFF4181)
  }
};
/*
 const Color Particle::Colors[] = {
 Particle::colorFromInt(0xdb4437),
 Particle::colorFromInt(0xe91e63),
 Particle::colorFromInt(0x9c27b0),
 Particle::colorFromInt(0x673ab7),
 Particle::colorFromInt(0x3f51b5),
 Particle::colorFromInt(0x4285f4),
 Particle::colorFromInt(0x4285f4),
 Particle::colorFromInt(0x00bcd4),
 Particle::colorFromInt(0x009688),
 Particle::colorFromInt(0x0f9d58),
 Particle::colorFromInt(0x8bc34a),
 Particle::colorFromInt(0xcddc39),
 Particle::colorFromInt(0xffeb3b),
 Particle::colorFromInt(0xf4b400),
 Particle::colorFromInt(0xff9800),
 Particle::colorFromInt(0xff5722)
 };
 */
Particle::Particle() {
  
}

Particle::~Particle() {
  
}

void Particle::setup(const Vec2f &position, const Vec2f &velocity) {
  mPosition = Vec3f(position, 0.0f);
  mVelocity = Vec3f(velocity, 0.0f);
  mAcceleration = Vec3f(0.0f, 0.0f, 0.0f);
  
  mAge = 0;
  mFlapLifespan = randInt(FLAP_LIFESPAN - 5, FLAP_LIFESPAN + 5);
  mLifespan = mFlapLifespan * randInt(14, 19);
  mVecolityDecay = 0.99f;
  //  mRadius = RADIUS * (RADIUS_MIN_QUOTA + (1.0f - RADIUS_MIN_QUOTA) * pow(randFloat(), RADIUS_POW));
  mRadius = lerp(RADIUS_MIN, RADIUS_MAX, pow(randFloat(), RADIUS_POW));
  mIsDead = false;
  
  int numColors = sizeof(myColors) / sizeof(*myColors);
  int colorIndex = randInt(numColors- 1);
  
  int colorAIndex = randInt(0,4);
  int colorBIndex = randInt(0,4);
  while(colorBIndex == colorAIndex){
    colorBIndex = randInt(0,4);
  }
  mColorA = myColors[colorIndex][colorAIndex];
  mColorB = myColors[colorIndex][colorBIndex];
  
  int numShapes = sizeof(Shapes) / sizeof(*Shapes);
  int shapeIndex = randInt(numShapes);
  mShape = Shapes[shapeIndex];
  
  seekIndex = 4;
  maxSpeed = 200;
}

void Particle::update(float delta, Perlin &perlin) {
  
  mVelocity += mAcceleration;
  mPosition += mVelocity;
  
  mAge += 1;
  
  // flap
  if (mAge >= mLifespan) {
    mIsDead = true;
  }
  
  mAcceleration = Vec3f(0,0,0);
  
//  if(mPosition.x-mRadius > 1280) mPosition.x = -mRadius+640;
//  if(mPosition.x+mRadius < 640) mPosition.x= 1280+mRadius;
//  if(mPosition.y-mRadius > 480) mPosition.y = -mRadius;
//  if(mPosition.y+mRadius < 0) mPosition.y= 480+mRadius;
}



void Particle::addForce(Vec2f aForce){
  mAcceleration += Vec3f(aForce.x, aForce.y, 0);
}



Vec2f Particle::seek(vector<Vec2f> aPoints){
  
  float minDis= windowWidth * 2;
  for(int i = aPoints.size()-1; i >= 0; i--){
    float dis = aPoints[i].distance( Vec2f(mPosition.x, mPosition.y) );
    if(dis < minDis) {
      seekIndex = i;
      minDis = dis;
    }
  }
  
  Vec2f dir = aPoints[seekIndex] - Vec2f(mPosition.x, mPosition.y);
  
  Vec2f steer = dir - Vec2f( mVelocity.x, mVelocity.y);
  steer.normalize();
  steer *= maxSpeed;
  steer.x = randFloat(-0.4f, -0.1f);
  
  return steer* 0.0015;
}

Vec2f Particle::separate(Vec2f movingPoint){
  float dist = Vec2f(mPosition.x, mPosition.y).distance(movingPoint);
  if(dist > 0.0 && dist < SEPARATE_DIST_THRESHOLD){
    Vec2f diff = Vec2f(mPosition.x, mPosition.y) - movingPoint;
    diff.normalize();
    diff *= (maxSpeed / dist) ;
    Vec2f steer = diff - Vec2f( mVelocity.x, mVelocity.y);
    return steer;
  }else return Vec2f(0.0, 0.0);
}


void Particle::draw() {
  gl::color( 1.0, 1.0, 1.0 ) ;
  int flapAge = mAge % mFlapLifespan;
  float flapProgress = (float)flapAge / (float)mFlapLifespan;
  
  // swap colors
  if (flapAge == 0) {
    Color temp = mColorA;
    mColorA = mColorB;
    mColorB = temp;
  }
  
  Color flapColor = flapProgress < 0.5f ? mColorA : mColorB;
  
  // change brightness according to https://www.desmos.com/calculator/udq3jfjvh7
  if (flapProgress < 0.5f) {
    // brighten at bottom
    flapColor *= cos(M_PI * (1.0f - 2.0f * flapProgress)) * 0.2f + 1.2f;
  } else {
    // darken at top
    flapColor *= cos(M_PI * 2.0f * flapProgress) * 0.2f + 0.8f;
  }
  
  const float rectRadius = mRadius * 0.5f;
  
  gl::popMatrices();
  gl::pushModelView();
  
  gl::translate(mPosition);
  
  // rotate according to direction
  //  const float directionImpact = 0.5f;
  
  //  const float directionTheta = atan2(mVelocity.y, mVelocity.x) + M_PI_2;
  //  const float directionThetaDeg = 180.0f * directionTheta / M_PI;
  //  gl::rotate(directionImpact * Vec3f(0.0f, 0.0f, directionThetaDeg));
  
  //    const float directionPhi = atan2(mVelocity.z, mVelocity.x) + M_PI_2;
  //    const float directionPhiDeg = 180.0f * directionPhi / M_PI;
  gl::rotate(Vec3f(0.0f, 0.0f, 172.0f * M_PI_2));
  
  // top (only draw after one flap)
  if (mAge >= mFlapLifespan) {
    gl::color(mColorA);
    
    switch (mShape) {
      case SQUARE:
        gl::drawSolidRect(Rectf(-rectRadius, -rectRadius, rectRadius, 0.0f));
        break;
      case TRIANGLE:
        drawArc(Vec2f::zero(), mRadius, M_PI, 2.0f * M_PI, 1);
        break;
      case HEXAGON:
        drawArc(Vec2f::zero(), mRadius, M_PI, 2.0f * M_PI, 2);
        break;
      case CIRCLE:
        drawArc(Vec2f::zero(), mRadius, M_PI, 2.0f * M_PI, 12);
        break;
    }
  }
  
  // bottom (only draw before last flap)
  if (mAge < mLifespan - mFlapLifespan) {
    gl::color(mColorB);
    
    switch (mShape) {
      case SQUARE:
        gl::drawSolidRect(Rectf(-rectRadius, 0.0f, rectRadius, rectRadius));
        break;
      case TRIANGLE:
        drawArc(Vec2f::zero(), mRadius, 0.0f, M_PI, 1);
        break;
      case HEXAGON:
        drawArc(Vec2f::zero(), mRadius, 0.0f, M_PI, 2);
        break;
      case CIRCLE:
        drawArc(Vec2f::zero(), mRadius, 0.0f, M_PI, 12);
        break;
    }
  }
  
  // flap
  if (mAge < mLifespan) {
    
    gl::color(flapColor);
    gl::rotate(Vec3f( 180.0f * flapProgress,0.0f, 0.0f));
    
    switch (mShape) {
      case SQUARE:
        gl::drawSolidRect(Rectf(-rectRadius, 0.0f, rectRadius, rectRadius));
        break;
      case TRIANGLE:
        drawArc(Vec2f::zero(), mRadius, 0.0f, M_PI, 1);
        break;
      case HEXAGON:
        drawArc(Vec2f::zero(), mRadius, 0.0f, M_PI, 2);
        break;
      case CIRCLE:
        drawArc(Vec2f::zero(), mRadius, 0.0f, M_PI, 12);
        break;
    }
  }
  
  gl::popModelView();
  gl::popMatrices();
}

/**
 *  Converts 24bit hex color to Color
 *
 *  @param uint32_t rgb e.g. 0xff0000
 *
 *  @return Color
 */
Color Particle::colorFromInt(const uint32_t rgb) {
  uint32_t r = (rgb >> 16) & 0xff;
  uint32_t g = (rgb >> 8) & 0xff;
  uint32_t b = rgb & 0xff;
  return Color((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f);
}

/**
 * Loosely based on https://forum.libcinder.org/topic/arc-in-cinder
 * and http://slabode.exofire.net/circle_draw.shtml
 */
void Particle::drawArc(const Vec2f &center, const float radius, const float start, const float stop, const int numSegments){
  
  const int numVertex = numSegments + 1;
  const float angleSpan = stop - start;
  
  gl::begin(GL_TRIANGLE_FAN);
  
  for (int i = 0; i <= numVertex; i++) {
    const float angle = start + angleSpan * (float)i / (float)numVertex;
    const float x = center.x + cos(angle) * radius;
    const float y = center.y + sin(angle) * radius;
    gl::vertex(x, y);
  }
  
  gl::end();
}

