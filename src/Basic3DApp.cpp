#include "cinder/app/AppNative.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Basic3DApp : public AppNative {
  private:
    Vec3f currentRotation;
    Vec3f rotationIncrement;
    Color c;
    gl::Fbo mFbo;
    params::InterfaceGl mParams;
    float smallCubeSize;
    int numCubes;
    float largeCubeSize;
    
    //parameters
  public:
	void setup();
    void prepareSettings(Settings* settings);
	void mouseDown( MouseEvent event );
    void resize();
	void update();
	void draw();
};

void Basic3DApp::prepareSettings(cinder::app::AppBasic::Settings *settings){
    settings->setWindowPos(50, 50);
    settings->setWindowSize(800, 800);
    settings->setFrameRate(60);
}

void Basic3DApp::setup()
{
    gl::enableAlphaBlending();
    
    gl::Fbo::Format format;
    format.setColorInternalFormat(GL_RGBA32F_ARB );
    mFbo = gl::Fbo(800, 800, format);
    mFbo.bindFramebuffer();
    gl::clear();
    mFbo.unbindFramebuffer();
    
    currentRotation = Vec3f::zero();
    rotationIncrement = Vec3f(0.1f, 0.2f, 0.3f);
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
    gl::clear();
    
    //params
    smallCubeSize = 5.0;
    largeCubeSize = 15.0;
    numCubes = 10;
    
    mParams = params::InterfaceGl(getWindow(), "Parameters", Vec2i(200,100));
    mParams.addParam( "Small Cube Size", &smallCubeSize, "min=0.1 max=20.5 step=0.5" );
    mParams.addParam( "Large Cube Size", &largeCubeSize, "min=10.0 max=100.0 step=0.5" );
    mParams.addParam( "numCubes", &numCubes, "min=0 max=10" );
}

void Basic3DApp::mouseDown( MouseEvent event )
{
}

void Basic3DApp::resize(){
    mFbo.reset();
    mFbo = gl::Fbo(getWindowWidth(), getWindowHeight());
}

void Basic3DApp::update()
{
    currentRotation += rotationIncrement;
}

void Basic3DApp::draw()
{
    mFbo.bindFramebuffer();
    gl::clear(ColorAf(Color::black(), 0.0));
    gl::enableDepthRead();
    gl::pushModelView();
    gl::translate(Vec3f(getWindowWidth()/2, getWindowHeight()/2, 0));

    //draw a ring of cubes
    for(int i = 0; i < numCubes; i++){
        gl::pushModelView();
        gl::rotate(Vec3f::zAxis() * (currentRotation.z + 360.f/ numCubes * i));
        gl::translate(Vec3f(200, 0, 0));
        gl::color(Color(1.0, 1.0, 1.0));
        gl::drawCube(Vec3f::zero(), Vec3f(smallCubeSize, smallCubeSize, smallCubeSize));
        gl::popModelView();
    }
    gl::color(Color(1.0, 1.0, 1.0));
    gl::rotate(currentRotation);
    gl::drawColorCube(Vec3f::zero(), Vec3f(largeCubeSize, largeCubeSize, largeCubeSize));
    gl::popModelView();
    mFbo.unbindFramebuffer();
    
    gl::disableDepthRead();
    gl::draw(mFbo.getTexture(), Vec2f(0, 0));
    
    gl::color(ColorAf(Color::white(), 0.6));
    float leftEdge = getWindowWidth()/2 + (sin(getElapsedSeconds()) * getWindowWidth()/2);
    gl::drawSolidRect(Rectf(Vec2f(leftEdge, 50.0), Vec2f(leftEdge + 50, 100.0)));
    //
    gl::color(ColorAf(0.0, 0.0, 0.0, 0.19));
    gl::drawSolidRect(getWindowBounds());
    mParams.draw();
}

CINDER_APP_NATIVE( Basic3DApp, RendererGl )
