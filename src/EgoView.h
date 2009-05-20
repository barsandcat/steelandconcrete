#ifndef EGOVIEW_H
#define EGOVIEW_H

#include <Ogre.h>
#include <BirdCamera.h>

class ClientGame;

class EgoView: public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener
{
public:
    EgoView(ClientGame& aGame);
    ~EgoView();
    void Frame(unsigned long aFrameTime);
    bool buttonPressed(const OIS::JoyStickEvent &arg, int button)
    {
        return true;
    }
    bool buttonReleased(const OIS::JoyStickEvent &arg, int button)
    {
        return true;
    }
    bool axisMoved(const OIS::JoyStickEvent &arg, int axis)
    {
        return true;
    }
    bool mouseMoved(const OIS::MouseEvent& arg);
    bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
    bool keyPressed(const OIS::KeyEvent& arg);
    bool keyReleased(const OIS::KeyEvent& arg);
protected:
private:
    ClientGame& mGame;
    BirdCamera* mBirdCamera;
    ClientTile* mSelectedTile;
    Ogre::SceneNode* mSelectionMarker;
    Ogre::SceneManager* mSceneMgr;
    void UpdateSelectedTilePosition(const OIS::MouseState &aState);
};



#endif // EGOVIEW_H
