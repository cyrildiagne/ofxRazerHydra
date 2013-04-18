#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxRazerHydra.h"

class testApp : public ofBaseApp{

public:
    
    void setup();
    void update();
    void draw();
    
    void onHydraButtonPressed(ofxRazerHydraButtonEventArgs& args);
    void onHydraTriggerPressed(ofxRazerHydraTriggerEventArgs& args);
    
private:
    ofxRazerHydra hydra;
    ofEasyCam cam;
    
    vector<ofNode> bullets;
    
    ofPoint camLongLat;
};

#endif
