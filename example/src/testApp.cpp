
#include "testApp.h"

void testApp::setup(){
    
    ofBackground(80);
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
    hydra.setup();
    ofAddListener(hydra.buttonPressedEvent, this, &testApp::onHydraButtonPressed);
    ofAddListener(hydra.triggerPressedEvent, this, &testApp::onHydraTriggerPressed);
    
    cam.setDistance(800);
}

void testApp::update() {
    
    hydra.update();
    
    sixenseControllerData cdata;
    
    // move the camera forward backward with right controller's joystick
    cdata = hydra.getControllerData(sixenseUtils::ControllerManager::P1R);
    cam.setDistance( cam.getDistance() - cdata.joystick_y );
    
    // orbit the camera around with the left controller's joystick
    cdata = hydra.getControllerData(sixenseUtils::ControllerManager::P1L);
    camLongLat.x += cdata.joystick_x * 0.5;
    camLongLat.y -= cdata.joystick_y * 0.5;
    cam.orbit(camLongLat.x, camLongLat.y, cam.getDistance(), ofPoint::zero());
    
    // update bullets travel
    ofPoint p, z;
    for (int i=0; i<bullets.size(); i++) {
        z = bullets[i].getGlobalTransformMatrix().getRowAsVec3f(2);
        bullets[i].move(z * -40);
    }
    
}

void testApp::draw()
{
    ofSetColor(ofColor::white);
    
    if(!hydra.isReady()) {
        ofDrawBitmapString(hydra.getSetupString(), ofPoint(10, 15));
        return;
    }
    
    glEnable(GL_DEPTH_TEST);
    
    cam.begin();
    
    ofPushMatrix();
    ofRotate(90, 0, 0, 1);
    ofSetColor(200, 200, 200);
    ofDrawGridPlane(1500, 15, false);
    ofPopMatrix();
    
    ofDrawAxis(10);
    hydra.drawDebug();
    
    ofSetColor(ofColor::magenta);
    for (int i=0; i<bullets.size(); i++) {
        bullets[i].transformGL();
        ofDrawBox(10, 10, 30);
        bullets[i].restoreTransformGL();
    }
    
    cam.end();
    
    ofSetColor(ofColor::white);
    
    string buffer = "";
    buffer += "Use joysticks to move the camera and triggers to shoot\n";
    buffer += "Press start to toggle fullscreen\n";
    buffer += ofToString(ofGetFrameRate());
    ofDrawBitmapString(buffer, ofPoint(10, 15));
}

//---------------------------

void testApp::onHydraButtonPressed(ofxRazerHydraButtonEventArgs& args)
{
    string side = args.controllerId == sixenseUtils::ControllerManager::P1L ? "left" : "right";
    ofLog() << "Button " << args.buttonId << " pressed on " << side << " controller";
    
    switch (args.buttonId) {
        case SIXENSE_BUTTON_START:
            ofToggleFullscreen();
            break;
            
        default:
            break;
    }
}

void testApp::onHydraTriggerPressed(ofxRazerHydraTriggerEventArgs& args)
{
    string side = args.controllerId == sixenseUtils::ControllerManager::P1L ? "left" : "right";
    ofLog() << "Trigger pressed on " << side << " controller";
    
    // let's shoot some *fake* bullets
    
    bullets.push_back(ofNode());
    
    ofNode& n = bullets.back();
    n.setTransformMatrix( hydra.getNode(args.controllerId).getGlobalTransformMatrix() );
    
    ofPoint z = n.getGlobalTransformMatrix().getRowAsVec3f(2);
    n.move(z * -100);
    
    if(bullets.size()>20) {
        bullets.erase(bullets.begin());
    }
}