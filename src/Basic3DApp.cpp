#include "cinder/app/AppNative.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Basic3DApp : public AppNative {
  private:
    Vec3f currentRotation;
    Vec3f rotationIncrement;
    Color c;
    gl::Fbo mFbo;
  public:
	void setup();
    void prepareSettings(Settings* settings);
	void mouseDown( MouseEvent event );	
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
}

void Basic3DApp::mouseDown( MouseEvent event )
{
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
    int numCubes = 10;
    for(int i = 0; i < numCubes; i++){
        gl::pushModelView();
        gl::rotate(Vec3f::zAxis() * ((360.f / numCubes * i)) + currentRotation);
        gl::translate(Vec3f(200, 0, 0));
        gl::color(Color(1.0, 1.0, 1.0));
        gl::drawCube(Vec3f::zero(), Vec3f(25, 25, 25));
        for (int j = 0; j < numCubes; j++) {
            gl::pushModelView();
            gl::rotate(Vec3f(currentRotation.x, 360.f / numCubes * j, 0));
            gl::translate(Vec3f(50, 0, 0));
            gl::color(Color(1.0, 0.0, 0.0));
            gl::drawCube(Vec3f::zero(), Vec3f(5, 5, 5));
            for (int k = 0; k < 4; k++) {
                gl::pushModelView();
                gl::rotate(Vec3f(0, 0, 360.f/4 * k));
                gl::translate(Vec3f(10, 0, 0));
                gl::color(Color(1.0, 1.0, 0.0));
                gl::drawCube(Vec3f::zero(), Vec3f(3, 3, 3));
                gl::popModelView();
            }
            gl::popModelView();
        }
        gl::popModelView();
    }
    gl::color(Color(1.0, 1.0, 1.0));
    gl::rotate(currentRotation);
    gl::drawColorCube(Vec3f::zero(), Vec3f(100, 100, 100));
    gl::popModelView();
    mFbo.unbindFramebuffer();
    gl::disableDepthRead();
    gl::draw(mFbo.getTexture());
    
    gl::color(ColorAf(Color::white(), 0.6));
    float leftEdge = getWindowWidth()/2 + (sin(getElapsedSeconds()) * getWindowWidth()/2);
    gl::drawSolidRect(Rectf(Vec2f(leftEdge, 50.0), Vec2f(leftEdge + 50, 100.0)));
    //
    gl::color(ColorAf(0.0, 0.0, 0.0, 0.19));
    gl::drawSolidRect(getWindowBounds());
}

CINDER_APP_NATIVE( Basic3DApp, RendererGl )
