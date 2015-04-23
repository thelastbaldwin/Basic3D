#include "cinder/app/AppNative.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"
#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Basic3DApp : public AppNative {
  private:
    Vec3f currentRotation;
    Vec3f rotationIncrement;
    Color c;
    gl::Fbo mFbo;
    //parameters
    params::InterfaceGl mParams;
    float smallCubeSize;
    int numCubes;
    float largeCubeSize;
    float radius;
    float opacity;
    
    CameraPersp	mCam;
    
    gl::GlslProgRef mShader;
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
    console() << "Begin setup" << endl;
    
    gl::enableAlphaBlending();
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    
    gl::Fbo::Format format;
    format.setColorInternalFormat(GL_RGBA32F_ARB );
    mFbo = gl::Fbo(getWindowWidth(), getWindowHeight(), format);
    mFbo.bindFramebuffer();
    gl::clear();
    mFbo.unbindFramebuffer();
    
    currentRotation = Vec3f::zero();
    rotationIncrement = Vec3f(0.1f, 0.2f, 0.3f);
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
    gl::clear();
    
    //camera
    mCam.lookAt( Vec3f( 0.0f, 0.0f, 500.0f ), Vec3f::zero() );
    mCam.setPerspective( 60, getWindowAspectRatio(), 1, 5000 );
    gl::setMatrices( mCam );
    
    //params
    smallCubeSize = 20.0;
    largeCubeSize = 80.0;
    numCubes = 10;
    radius = 300;
    opacity = 0.01;
    
    mParams = params::InterfaceGl(getWindow(), "Parameters", Vec2i(200,120));
    mParams.addParam( "Small Cube Size", &smallCubeSize, "min=0.1 max=20.5 step=0.5" );
    mParams.addParam( "Large Cube Size", &largeCubeSize, "min=10.0 max=100.0 step=0.5" );
    mParams.addParam( "numCubes", &numCubes, "min=0 max=10" );
    mParams.addParam( "radius", &radius, "min=100 max=800");
    mParams.addParam("opacity", &opacity, "min=0 max=1.0 step=0.05");
    
    //shader setup
    try {
        mShader = gl::GlslProg::create( loadResource(RES_VERT_GLSL), loadResource(RES_FRAG_GLSL));
    }
    catch( gl::GlslProgCompileExc &exc ) {
        console() << "Shader compile error: " << std::endl;
        console() << exc.what();
    }
    catch( ... ) {
        console() << "Unable to load shader" << std::endl;
    }
}

void Basic3DApp::mouseDown( MouseEvent event )
{
}

void Basic3DApp::resize(){
    //subtle memory leak in here
    mFbo.reset();
    mFbo = gl::Fbo(getWindowWidth(), getWindowHeight());
}

void Basic3DApp::update()
{
    currentRotation += rotationIncrement;
}

void Basic3DApp::draw()
{
    //clear the buffer
    gl::clear();
    
    mFbo.bindFramebuffer();
    gl::setViewport( mFbo.getBounds());
    gl::clear(ColorAf(Color::black(), 0.0));
//    gl::enableWireframe();
    gl::pushModelView();
    gl::translate(Vec3f(mFbo.getWidth()/2, mFbo.getHeight()/2, 0));

    //draw a ring of cubes
    for(int i = 0; i < numCubes; i++){
        gl::pushModelView();
        gl::rotate(Vec3f::zAxis() * (currentRotation.z + 360.f/ numCubes * i));
        gl::translate(Vec3f(radius, 0, 0));
        gl::color(Color(1.0, 1.0, 1.0));
        gl::drawCube(Vec3f::zero(), Vec3f(smallCubeSize, smallCubeSize, smallCubeSize));
        gl::popModelView();
    }
    gl::color(Color(1.0, 1.0, 1.0));
    gl::rotate(currentRotation);
    gl::drawColorCube(Vec3f::zero(), Vec3f(largeCubeSize, largeCubeSize, largeCubeSize));
    gl::popModelView();
    mFbo.unbindFramebuffer();
    //    gl::draw(mFbo.getTexture(), Vec2f(getWindowWidth()/4, getWindowHeight()/4));
    
    gl::setViewport( getWindowBounds());
    
    mFbo.getTexture().enableAndBind();
    if(mShader){
        mShader->bind();
        //    mShader->uniform( "tex0", 0 );
        mShader->uniform("opacity", opacity);
    }
    gl::pushModelView();
    gl::translate(getWindowWidth()/2 - mFbo.getWidth()/2, getWindowHeight()/2 - mFbo.getHeight()/2);
    for(int i = 10; i >=0 ; --i){
        gl::pushModelView();
        gl::translate(Vec3f(0, 0, i * -100));
        gl::drawSolidRect(Rectf(Vec2f(0, 0), Vec2f(mFbo.getWidth(), mFbo.getHeight())));
        gl::popModelView();
    }
    
    gl::popModelView();
    mFbo.getTexture().unbind();
    mShader->unbind();
    
    mParams.draw();
}

CINDER_APP_NATIVE( Basic3DApp, RendererGl )
