#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <OIS.h>
#include <Ogre.h>
#include <BetaGUI.h>

class GameState: public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener, public BetaGUI::GUIListener
{
public:
    virtual ~GameState(){};
    virtual void Frame(unsigned long aFrameTime) = 0;
    virtual bool buttonPressed( const OIS::JoyStickEvent &arg, int button ){return true;}
    virtual bool buttonReleased( const OIS::JoyStickEvent &arg, int button ){return true;}
    virtual bool axisMoved( const OIS::JoyStickEvent &arg, int axis ){return true;}
protected:
private:
};



#endif // GAMESTATE_H
