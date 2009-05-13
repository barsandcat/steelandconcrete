#ifndef SETUPVIEW_H
#define SETUPVIEW_H

#include <GameState.h>
#include <ClientApp.h>
#include <Ogre.h>


class SetUpView : public GameState
{
public:
    SetUpView(ClientApp & aApp);
    virtual ~SetUpView();
    virtual void Frame(unsigned long aFrameTime);
    virtual bool mouseMoved(const OIS::MouseEvent &arg);
    virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    virtual bool keyPressed(const OIS::KeyEvent &arg);
    virtual bool keyReleased(const OIS::KeyEvent &arg);
protected:
private:
    BetaGUI::Window * mExitWindow;
    BetaGUI::Button * mExitButton;
    ClientApp & mApp;
    Ogre::SceneManager * mSceneMgr;
    Ogre::Camera * mCamera;
    void OnButton(BetaGUI::Button*, BetaGUI::FocusState);
    void OnContinue(BetaGUI::Button*, BetaGUI::FocusState);
};



#endif // SETUPVIEW_H
