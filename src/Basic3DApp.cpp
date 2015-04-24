#include <deque>
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
    int canvasOffset;
    int frameOffset;
    int currentFrame;
    
    CameraPersp	mCam;
    const static int NUM_TEXTURES = 20;
    deque<ci::gl::Texture> mFrames;
    gl::GlslProgRef mShader;
  public:
	void setup();
    void prepareSettings(Settings* settings);
	void mouseDown( MouseEvent event );
	void update();
	void draw();
    void bindAllTextures(deque<ci::gl::Texture>& textures);
    void unbindAllTextures(deque<ci::gl::Texture>& textures);
};

void Basic3DApp::prepareSettings(cinder::app::AppBasic::Settings *settings){
    settings->setWindowPos(50, 50);
    settings->setWindowSize(800, 800);
    settings->setFrameRate(60);
}

void Basic3DApp::setup()
{
    currentFrame = 0;
    
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
    gl::clear();
    
    //camera
    mCam.lookAt( Vec3f( 0.0f, 0.0f, 500.0f ), Vec3f::zero() );
    mCam.setPerspective( 60, getWindowAspectRatio(), 1, 5000 );
    gl::setMatrices( mCam );
    
    //params
    smallCubeSize = 20.0;
    largeCubeSize = 80.0;
    numCubes = 10;
    radius = 384;
    opacity = 0.31;
    canvasOffset = 100;
    frameOffset = 20;
    
    mParams = params::InterfaceGl(getWindow(), "Parameters", Vec2i(200,140));
    mParams.addParam( "Small Cube Size", &smallCubeSize, "min=0.1 max=20.5 step=0.5" );
    mParams.addParam( "Large Cube Size", &largeCubeSize, "min=10.0 max=100.0 step=0.5" );
    mParams.addParam( "numCubes", &numCubes, "min=0 max=10" );
    mParams.addParam( "radius", &radius, "min=100 max=800");
    mParams.addParam("opacity", &opacity, "min=0 max=1.0 step=0.05");
    mParams.addParam("offset", &canvasOffset, "min=10 max=500 step=5");
    mParams.addParam("frameOffset", &frameOffset, "min=1 max=20 step=1");
    
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
    
    //texture deque setup
    mFrames.resize(NUM_TEXTURES);   
    for(int i = 0; i < NUM_TEXTURES; ++i){
        mFrames[i] = ci::gl::Texture(getWindowWidth(), getWindowHeight());
    }
}

void Basic3DApp::mouseDown( MouseEvent event )
{
}

void Basic3DApp::bindAllTextures(deque<ci::gl::Texture> &textures){
    stringstream ss;
    string texLoc;
    
    for(int i = 0; i < textures.size(); ++i){
        textures[i].enableAndBind();
        ss << "tex" << i;
        texLoc = ss.str();
        mShader->uniform(texLoc, i);
        ss.clear();
        ss.str("");
    }
}

void Basic3DApp::unbindAllTextures(deque<ci::gl::Texture> &textures){
    for(auto tex: textures){
        tex.unbind();
    }
}

void Basic3DApp::update()
{
    currentRotation += rotationIncrement;
}

void Basic3DApp::draw()
{
    //clear the buffer
    gl::clear(Color::black());
    
    mFbo.bindFramebuffer();
    gl::setViewport( mFbo.getBounds());
    gl::clear(ColorAf(Color::black(), 0.0));
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
    
    //manage stack of textures
    if(currentFrame % frameOffset == 0){
        mFrames.push_front(gl::Texture(mFbo.getTexture()));
        mFrames.pop_back();
    }
    ++currentFrame;
    
    gl::setViewport( getWindowBounds());
    
   
//    mFrames[0].bind();
//     mFbo.getTexture().enableAndBind();
    
    
    gl::pushModelView();
    gl::translate(getWindowWidth()/2 - mFbo.getWidth()/2, getWindowHeight()/2 - mFbo.getHeight()/2);
    for(int i = NUM_TEXTURES-1; i >= 0; --i){
        if(mShader){
            mShader->bind();
            mFrames[i].enableAndBind();
            mShader->uniform("tex0", 0);
            mShader->uniform("opacity", opacity);
        }
        gl::pushModelView();
        gl::translate(Vec3f(0, 0, i * -canvasOffset));
        gl::drawSolidRect(Rectf(Vec2f(0, 0), Vec2f(mFbo.getWidth(), mFbo.getHeight())));
        gl::popModelView();
        if(mShader){
            mFrames[i].unbind();
            mShader->unbind();
        }
    }
    
    gl::popModelView();
//    mFbo.getTexture().unbind();
//    mFrames[0].unbind();
//    unbindAllTextures(mFrames);
//    mShader->unbind();
    
    mParams.draw();
}

CINDER_APP_NATIVE( Basic3DApp, RendererGl )
