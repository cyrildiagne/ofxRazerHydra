//
//  ofxRazerHydra.cpp
//  ofxRazerHydraExample
//
//  Created by @kikko_fr on 4/17/13.
//
//

#include "ofxRazerHydra.h"


template <typename T>
struct variadic_ : public vector<T> {
	variadic_(const T& t) {
		(*this)(t);
	}
	variadic_& operator()(const T& t) {
		this->push_back(t);
		return *this;
	}
};
typedef variadic_<int> variadic;


void ofxRazerHydra::setup()
{
    // Init sixense
	sixenseInit();
    
    for(int base=0; base<sixenseGetMaxBases(); base++ ) {

        sixenseSetActiveBase(base);
        //ofLog() << sixenseIsBaseConnected(base);
        
        datas.push_back(sixenseAllControllerData());
        
        for(int cont=0; cont<sixenseGetMaxControllers(); cont++ ) {
            
            //if( sixenseIsControllerEnabled( cont ) ) {
                nodes.push_back(ofNode());
                btStates.push_back(sixenseUtils::ButtonStates());
            //}
        }
    }
    
    currStep = sixenseUtils::getTheControllerManager()->getCurrentStep();
    
    // Init the controller manager. This makes sure the controllers are present, assigned to left and right hands, and that
	// the hemisphere calibration is complete.
	sixenseUtils::getTheControllerManager()->setGameType( sixenseUtils::ControllerManager::ONE_PLAYER_TWO_CONTROLLER );
}

ofxRazerHydra::~ofxRazerHydra()
{
    sixenseExit();
}

void ofxRazerHydra::update()
{
    
    // Go through each of the connected systems
    for( int base=0; base<sixenseGetMaxBases(); base++ ) {
        
        if(!sixenseIsBaseConnected(base)) continue;
        
        // update the controller manager with the latest controller data here
        sixenseSetActiveBase(base);
        sixenseAllControllerData& acd = datas[base];
        sixenseGetAllNewestData( &acd );
        sixenseUtils::getTheControllerManager()->update( &acd );
        
        if(currStep == sixenseUtils::IControllerManager::SETUP_COMPLETE
           || currStep == sixenseUtils::IControllerManager::P1C2_IDLE) {
            
            updateNodePositions(&acd, base);
            
            check_for_button_presses( &acd, base );
        }
        else {
            
            sixenseUtils::IControllerManager::setup_step step = sixenseUtils::getTheControllerManager()->getCurrentStep();
            
            if(step != currStep) {
                
                if(step == sixenseUtils::IControllerManager::P1C2_IDLE) {
                    ofLog() << "Controller for player one idle on dock. No need to calibrate";
                }
                else {
                    ofLog() << sixenseUtils::getTheControllerManager()->getStepString();
                }
                
                // We could also load the supplied controllermanager textures using the filename: sixenseUtils::getTheControllerManager()->getTextureFileName();
                
                currStep = step;
            }
        }
    }
}

void ofxRazerHydra::drawDebug()
{
    int base, cont;
    
    // Go through each of the connected systems
    for( base=0; base<sixenseGetMaxBases(); base++ ) {
        
        if(!sixenseIsBaseConnected(base)) continue;
        
        sixenseSetActiveBase(base);
        
        // For each possible controller
        for( cont=0; cont<sixenseGetMaxControllers(); cont++ ) {
    
            ofSetColor(cont==0 ? ofColor::red : ofColor::green);
            
            nodes[cont].transformGL();
            ofDrawSphere(0, 0, 10);
            ofDrawBox(ofPoint(0, 0, -50), 5, 5, 100);
            nodes[cont].restoreTransformGL();
        }
    }
}

//---------


void ofxRazerHydra::setFilter(bool on)
{
    sixenseSetFilterEnabled( on );
}

void ofxRazerHydra::toggleFilter()
{
    int toggle;
    sixenseGetFilterEnabled(&toggle);
    sixenseSetFilterEnabled( !toggle );
}

void ofxRazerHydra::updateNodePositions( sixenseAllControllerData *acd, int base )
{
    int cont, i, j;
    ofMatrix4x4 rot_mat;
        
    sixenseSetActiveBase(base);
    
    // For each possible controller
    for( cont=0; cont<sixenseGetMaxControllers(); cont++ ) {
        
        // See if it's enabled
        if( sixenseIsControllerEnabled( cont ) ) {
            
            for( i=0; i<3; i++ )
                for( j=0; j<3; j++ )
                    rot_mat(i, j) = acd->controllers[cont].rot_mat[i][j];
            
            rot_mat(0, 3) = 0.0f;
            rot_mat(1, 3) = 0.0f;
            rot_mat(2, 3) = 0.0f;
            rot_mat(3, 0) = acd->controllers[cont].pos[0];
            rot_mat(3, 1) = acd->controllers[cont].pos[1];
            rot_mat(3, 2) = acd->controllers[cont].pos[2];
            rot_mat(3, 3) = 1.0f;
            
            nodes[cont].setTransformMatrix(rot_mat);
        }
    }
}


void ofxRazerHydra::check_for_button_presses( sixenseAllControllerData *acd, int base ) {
   
    ofxRazerHydraButtonEventArgs btArgs;
    ofxRazerHydraTriggerEventArgs trArgs;
    
    sixenseSetActiveBase(base);
    
    variadic bt_ids = variadic (SIXENSE_BUTTON_BUMPER)(SIXENSE_BUTTON_JOYSTICK)
        (SIXENSE_BUTTON_1)(SIXENSE_BUTTON_1)(SIXENSE_BUTTON_2)
        (SIXENSE_BUTTON_3)(SIXENSE_BUTTON_4)(SIXENSE_BUTTON_START);
    
    int contId;
    
    // For each possible controller
    for( int cont=0; cont<sixenseGetMaxControllers(); cont++ ) {
        
        // See if it's enabled
        if( sixenseIsControllerEnabled( cont ) ) {
            
            contId = base*sixenseGetMaxBases() + cont;
            
            btStates[contId].update( &acd->controllers[cont] );
            
            for (int i=0; i<bt_ids.size(); i++) {
                if(btStates[contId].buttonJustPressed(bt_ids[i])) {
                    btArgs.baseId = base;
                    btArgs.controllerId = cont;
                    btArgs.buttonId = bt_ids[i];
                    ofNotifyEvent(buttonPressedEvent, btArgs);
                }
            }
            
            if( btStates[contId].triggerJustPressed() ) {
                trArgs.baseId = base;
                trArgs.controllerId = cont;
                ofNotifyEvent(triggerPressedEvent, trArgs);
            }
        }
    }
}