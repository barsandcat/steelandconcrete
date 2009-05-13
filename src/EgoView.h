#ifndef EGOVIEW_H
#define EGOVIEW_H

#include <Ogre.h>
#include <BetaGUI.h>
#include <GameState.h>
#include <BirdCamera.h>

class ClientGame;

class EgoView: public GameState
{
public:
    EgoView(ClientGame& aGame);
    virtual ~EgoView();
    virtual void Frame(unsigned long aFrameTime);
    virtual bool mouseMoved(const OIS::MouseEvent& arg);
    virtual bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
    virtual bool keyPressed(const OIS::KeyEvent& arg);
    virtual bool keyReleased(const OIS::KeyEvent& arg);
protected:
private:
    BetaGUI::Window* mExitWindow;
    BetaGUI::Button* mExitButton;
    ClientGame& mGame;
    BirdCamera* mBirdCamera;
    ClientTile* mSelectedTile;
    Ogre::SceneNode* mSelectionMarker;
    Ogre::SceneManager* mSceneMgr;
    void OnButton(BetaGUI::Button*, BetaGUI::FocusState);
    void UpdateSelectedTilePosition(const OIS::MouseState &aState);
};



#endif // EGOVIEW_H
