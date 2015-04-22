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
        gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::Fbo::Format format;
    format.setColorInternalFormat(GL_RGBA32F_ARB );
    mFbo = gl::Fbo(getWindowWidth()/2, getWindowHeight()/2, format);
    mFbo.bindFramebuffer();
    gl::clear();
    mFbo.unbindFramebuffer();
    
    currentRotation = Vec3f::zero();
    rotationIncrement = Vec3f(0.1f, 0.2f, 0.3f);
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
    gl::clear();
    
    //params
    smallCubeSize = 20.0;
    largeCubeSize = 80.0;
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
    mFbo = gl::Fbo(getWindowWidth()/2, getWindowHeight()/2);
}

void Basic3DApp::update()
{
    currentRotation += rotationIncrement;
}

void Basic3DApp::draw()
{
    gl::clear(ColorAf(1.0, 0.0, 0.0, 0.0));
    
    mFbo.bindFramebuffer();
    gl::setViewport( mFbo.getBounds() );
    gl::clear(Color(Color::black()));
    gl::pushModelView();
    //gl origin is in the lower left
    gl::translate(Vec3f(mFbo.getWidth(), mFbo.getHeight(), 0));

    //draw a ring of cubes
    for(int i = 0; i < numCubes; i++){
        gl::pushModelView();
        gl::rotate(Vec3f::zAxis() * (currentRotation.z + 360.f/ numCubes * i));
        gl::translate(Vec3f(100, 0, 0));
        gl::color(Color(1.0, 1.0, 1.0));
        gl::drawCube(Vec3f::zero(), Vec3f(smallCubeSize, smallCubeSize, smallCubeSize));
        gl::popModelView();
    }
    gl::color(Color(1.0, 1.0, 1.0));
    gl::rotate(currentRotation);
    gl::drawColorCube(Vec3f::zero(), Vec3f(largeCubeSize, largeCubeSize, largeCubeSize));
    gl::popModelView();
    mFbo.unbindFramebuffer();
    
    gl::setViewport( getWindowBounds());
    gl::draw(mFbo.getTexture(), Vec2f(getWindowWidth()/4, getWindowHeight()/4));
    
    mParams.draw();
}

CINDER_APP_NATIVE( Basic3DApp, RendererGl )
