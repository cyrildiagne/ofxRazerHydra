//
//  ofxRazerHydra.h
//  ofxRazerHydraExample
//
//  Created by kikko on 4/17/13.
//
//

#ifndef __ofxRazerHydraExample__ofxRazerHydra__
#define __ofxRazerHydraExample__ofxRazerHydra__

#include "ofMain.h"

#include <sixense.h>
#include <sixense_math.hpp>
#ifdef WIN32
#include <sixense_utils/mouse_pointer.hpp>
#endif
#include <sixense_utils/interfaces.hpp>
#include <sixense_utils/derivatives.hpp>
#include <sixense_utils/button_states.hpp>
#include <sixense_utils/event_triggers.hpp>
#include <sixense_utils/controller_manager/controller_manager.hpp>

struct ofxRazerHydraButtonEventArgs {
    int buttonId;
    int controllerId;
    int baseId;
};

struct ofxRazerHydraTriggerEventArgs {
    int controllerId;
    int baseId;
};

class ofxRazerHydra {
    
public:
    
    ofxRazerHydra(){};
    virtual ~ofxRazerHydra();
    
    ofEvent<ofxRazerHydraButtonEventArgs> buttonPressedEvent;
    ofEvent<ofxRazerHydraTriggerEventArgs> triggerPressedEvent;
    
    void setup();
    void update();
    
    void setFilter(bool on);
    void toggleFilter();
    
    void drawDebug();
    
    inline const sixenseControllerData& getControllerData(int controllerId, int base=0) {
        return datas[base].controllers[controllerId];
    }
    inline const ofNode& getNode(int controllerId, int base=0) {
        return nodes[base*sixenseGetMaxBases()+controllerId];
    }
    
private:
    
    vector<ofNode> nodes;
    vector<sixenseUtils::ButtonStates> btStates;
    
    vector<sixenseAllControllerData> datas;
    
    sixenseUtils::IControllerManager::setup_step currStep;
    
    void updateNodePositions( sixenseAllControllerData *acd, int base );
    
    void check_for_button_presses( sixenseAllControllerData *acd, int base );
};

#endif /* defined(__ofxRazerHydraExample__ofxRazerHydra__) */
