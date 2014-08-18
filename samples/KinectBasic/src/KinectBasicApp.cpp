#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"

#include "CinderFreenect.h"


#include "Particle Controller.h"

#define MAXTHRESHOLD 43000.0f
#define MINTHRESHOLD 35000.0f

#define NUM_PARTICLES_PER_FRAME 1

using namespace ci;
using namespace ci::app;
using namespace std;


class kinectBasicApp : public AppBasic {
  public:
	void prepareSettings( Settings* settings );
	void setup();
	void mouseUp( MouseEvent event );
	void update();
	void draw();
	
	KinectRef		mKinect;
private:
	gl::Texture		mColorTexture, mDepthTexture;
  ParticleController *controller;
  double mLastTime;
  Perlin mPerlin;
  int count;
  float avgX, avgY;
  Boolean drew = false;
  vector<Vec2f> zone;
};

void kinectBasicApp::prepareSettings( Settings* settings )
{
	settings->setWindowSize( 1280, 480 );
}

void kinectBasicApp::setup()
{
	console() << "There are " << Kinect::getNumDevices() << " Kinects connected." << std::endl;

	mKinect = Kinect::create();
  
  controller = new ParticleController();
  
  for(int i=0; i< 30; i++){
    float xPos = randFloat(0.0, 640.0);
    float yPos = randFloat(0.0, 480.0);
    controller->addParticles(NUM_PARTICLES_PER_FRAME, Vec2f(xPos, yPos), randVec2f());
  }
  
}

void kinectBasicApp::mouseUp( MouseEvent event )
{
	//writeImage( getHomeDirectory() / "kinect_video.png", mKinect->getVideoImage() );
	//writeImage( getHomeDirectory() / "kinect_depth.png", mKinect->getDepthImage() );
	
	// set tilt to random angle
//	mKinect->setTilt( Rand::randFloat() * 62 - 31 );

	// make the LED yellow
//	mKinect->setLedColor( Kinect::LED_YELLOW );
	
	// toggle infrared video
	//mKinect->setVideoInfrared( ! mKinect->isVideoInfrared() );
}

void kinectBasicApp::update()
{	
	if( mKinect->checkNewDepthFrame() )
		mDepthTexture = mKinect->getDepthImage();
	
	if( mKinect->checkNewVideoFrame() )
		mColorTexture = mKinect->getVideoImage();
	
//	console() << "Accel: " << mKinect.getAccel() << std::endl;
  
  if(int(app::getElapsedSeconds()) % 6 == 0){
    float xPos = randFloat(640.0);
    float yPos = randFloat(480.0);
    controller->addParticles(NUM_PARTICLES_PER_FRAME, Vec2f(xPos, yPos), randVec2f());
  }
  
  double currentTime = app::getElapsedSeconds();
  double deltaTime = currentTime - mLastTime;
  float delta = (float)app::getFrameRate() / (1.0 / deltaTime);
  
  mLastTime = currentTime;
  if(drew){
    std::shared_ptr<uint16_t> depth = mKinect->getDepthData();
    
    float sumX = 0.0;
    float sumY = 0.0;
    count = 0;
    float minThreshold = MINTHRESHOLD;
    float maxThreshold = MAXTHRESHOLD;
    
    zone.clear();
    for (int y = 0; y<480; y++){
      for (int x = 0; x<640; x++){
        float data = depth.get()[x+y*640];  // this will return the actual depth value for the pixel (x,y);
        //console() << data << std::endl;
        
        if( data > minThreshold && data < maxThreshold){
          //console() << x << std::endl;
          sumX += x;
          sumY += y;
          count ++;
          zone.push_back(Vec2f(x,y));

        }
      }
    }
    
    if(count != 0){
      avgX = sumX / count;
      avgY = sumY / count;
      //gl::color( 0.0f, 0.8f, 0.0f );
      
      //gl::popMatrices();
    }
	  this->controller->update(delta, mPerlin, Vec2f(avgX, avgY));
    //cout << avgX << endl;
  }else{
    this->controller->update(delta, mPerlin, Vec2f(0.0, 0.0));
  }
  
}

void kinectBasicApp::draw()
{
  gl::clear( Color( 0, 0, 0 ) );
  
  gl::color( 0.0f, 0.8f, 0.0f );
  gl::drawSolidEllipse(Vec2f(avgX, avgY), 14.0, 14.0);

  gl::color( 0.8f, 0.0f, 0.0f );
  for(int i=0; i<zone.size(); i++){
    gl::drawSolidEllipse(zone[i], 1.0, 1.0);
    //cout << zone[i] << endl;
  }
  
  gl::color( 1.0f, 1.0f, 1.0f );

	if( mDepthTexture ){
    gl::pushMatrices();
    gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );
    gl::translate(Vec2f(getWindowWidth()/2, 0));
    gl::scale(Vec3f(-1, 1, 1));
		gl::draw( mDepthTexture );
    gl::popMatrices();
    drew = true;
  
  
  	if( mColorTexture )
    //gl::draw(mColorTexture, Vec2i( 640, 0 ));

    //gl::pushMatrices();
    //gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );
    //gl::translate(Vec2f(getWindowWidth(), 0));
    //gl::scale(Vec3f(-1, 1, 1));
    //gl::draw(mColorTexture, Vec2i( -1280, 0 ));
		//gl::draw( mColorTexture, Vec2i( 0, 0 ) );
    //gl::popMatrices();

  this->controller->draw();
  }
}


CINDER_APP_BASIC( kinectBasicApp, RendererGl )
