#ifndef AL_LEAP_H
#define AL_LEAP_H

#include "al_console.h"
#include "al_math.h"

struct LeapMotionData {

    struct Arm {
         glm::vec3 elbowPos;
         glm::vec3 wristPos;
    };

    struct Bone {
        glm::vec3 center;
        glm::vec3 direction;
        //glm::mat4 basis;
        float width;
        float boneLength = 0.f;
    };

    struct Finger {
         glm::vec3 tip;
         glm::vec3 direction;
         glm::vec3 velocity;
         float width;
         float length;
         Bone bones[4];
    };

    struct Hand {
        glm::vec3 palmPos;
        glm::vec3 normal;
        glm::vec3 direction;
        glm::vec3 velocity;
        float grab, pinch;
        // fingers
        Finger fingers[5];
        int32_t id;

        bool isLeft = 0;
        bool isVisible = 0;
    };

    struct Pointable {
         glm::vec3 pointablePos;
    };

    // left, right
    Hand hands[2];
    Arm arms[2];

    bool isConnected = 0;
};

#ifdef AL_WIN
#define AL_LEAP_SUPPORTED 1
#include "leap/LeapC.h"
#include "leap/Leap.h"

glm::vec3 toGLM(Leap::Vector v) {
    //return glm::vec3(v.x, v.y, v.z);
    return glm::vec3(-v.x, -v.z, -v.y);
}

#endif

#ifdef AL_LEAP_SUPPORTED

struct LeapMotion : public LeapMotionData, public Leap::Listener {

    Leap::Controller controller; 
    
    bool connect() {
       controller.addListener(*this);
       return true;
    }

    virtual void onConnect(const Leap::Controller&) {
        console.log("Leap connected");
        //controller.addListener(Listener);
        controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);
        controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
        controller.config().setFloat("Gesture.Swipe.MinLength", 200.0);
        controller.config().save();

        isConnected = true;
    }

    virtual void onDisconnect(const Leap::Controller&) {
        console.log("Leap disconnected");

        isConnected = false;
    }

    virtual void onFrame(const Leap::Controller&) {
        Leap::Frame frame = controller.frame();

        hands[0].isVisible = 0;
        hands[1].isVisible = 0;
        
        int count = frame.hands().count();
        Leap::HandList hand_list = frame.hands();

       // printf("hands %d: " , count);

            /*
            Leap::Hand rightmost = hand_list.rightmost();
            Leap::Hand leftmost = hand_list.leftmost();
            int handID_right = rightmost.id();
            int handID_left = leftmost.id();


             for (int j=0; j<count; j++) {
                Leap::Hand leaphand = frame.hands()[j];

                if (leaphand.isRight()) {
                    leaphand = frame.hand(handID_right);
                } else if (leaphand.isLeft()) {
                    leaphand = frame.hand(handID_left);
                }

                console.log("hand %d %d\n", handID_right,  handID_left);

             }*/

        /*
            we would like the hands to remain consistent between frames
            the sdk gives each hand an .id(), which is unique and retained between frames
            but when tracking is lost, the id is incremented to a new one
            how to map these to the h of hands[h] ?

            how to know when a new id appears, and which h to bind it to?

        */

       //set as 1 to turn on, and 0 to turn off

        for (int j=0; j<count; j++) {
            Leap::Hand leaphand = frame.hands()[j];
            // unique ID, preserved across frames
            int32_t id = leaphand.id();
            

            int h = j;//
            Hand& hand = hands[h];
            hand.id = id;
            hand.isLeft = leaphand.isLeft();
            hand.isVisible = true;

           //printf("(%d)%d = %d ", j, h, id);

            hand.id = id;

            hand.palmPos = toGLM(leaphand.palmPosition()) * .001f;
            //Hand normal
            hand.normal = toGLM(leaphand.palmNormal()); 
            //Hand direction
            hand.direction = toGLM(leaphand.direction());
            //Hand velocity
            hand.velocity = toGLM(leaphand.palmVelocity()) * .001f;


            //console.log("hand %d %d %d\n", j, hand.id, leaphand.isLeft());
        
            //Hand grab and pinch strength
            hand.grab = leaphand.grabAngle();
            hand.pinch = leaphand.pinchStrength();


            //Get Fingers
            // Leap::Finger forwardFinger = handR.fingers().frontmost();

            Leap::FingerList fingers = leaphand.fingers();
            auto& outfingers = hand.fingers;
            int i=0;
            for(Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end() && i < 5; fl++, i++){
                Leap::Finger finger = *fl;
                
                LeapMotionData::Finger& outfinger = outfingers[i];
                //Tip position
                outfinger.tip = toGLM(finger.tipPosition()) * .001f;
                //Direction
                outfinger.direction = toGLM(finger.direction());
                //Velocity
                outfinger.velocity = toGLM(finger.tipVelocity()) * .001f;
                //width and length
                outfinger.width = finger.width() * .001f;
                outfinger.length = finger.length() * .001f;
            
                for(int b = 0; b < 4; b++)
                { 

                    Leap::Bone::Type boneType = static_cast<Leap::Bone::Type>(b);
                    Leap::Bone bone = (*fl).bone(boneType);
                    LeapMotionData::Bone& outbone = outfinger.bones[b];
                    //Bone width length
                    outbone.width = bone.width() * .001f;
                    outbone.boneLength = bone.length() * .001f;
                    //Bone center
                    outbone.center = toGLM(bone.center()) * .001f;
                    //Bone direction
                    outbone.direction = toGLM(bone.direction()) * .001f;

                    //std::cout << "Finger index: " << (*fl).type() << " " << bone << std::endl;
                //console.log("finger %d bone %d %f\n", i, b,  hands[0].fingers[i].bones[b].boneLength);
                }
            }
            
            //Get Arm
            Leap::Arm arm = leaphand.arm();
            arms[h].elbowPos = toGLM(arm.elbowPosition());
            arms[h].wristPos = toGLM(arm.wristPosition());

        }
        

        // printf("\n");

        //Leap::PointableList pointables = frame.pointables();
        //Leap::Pointable pointable = frame.pointables().frontmost();
        //Leap::FingerList fingers = frame.fingers();
        
        //Leap::ToolList tools = frame.tools();

    }

    virtual void onServiceConnect(const Leap::Controller& controller) {
        std::cout << "Service connected " << std::endl;
    }

    virtual void onServiceDisconnect(const Leap::Controller&) {
        std::cout << "Service disconnected " << std::endl;
    }

    virtual void onServiceChange(const Leap::Controller& controller) {
        std::cout << "Service state change " << std::endl;
    }

    /*
    int main(int argc, char** argv) {
        std::cout << "Press Enter to quit, or enter 'p' to pause or unpause the service..." << std::endl;

        bool paused = false;
        while (true) {
            char c = std::cin.get();
            if (c == 'p') {
            paused = !paused;
            controller.setPaused(paused);
            std::cin.get(); //skip the newline
            }
            else
            break;
        }
    }
    */

};

#else 

struct LeapMotion : public LeapMotionData {

    bool connect() {
        return false;
    }
};

#endif


#endif