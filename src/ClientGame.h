#ifndef CLIENTGAME_H
#define CLIENTGAME_H

#include <ClientUnit.h>
#include <ClientGeodesicGrid.h>
#include <LoadingSheet.h>
#include <InGameSheet.h>
#include <sockio.h>

class ClientApp;

class ClientGame
{
public:
    ClientGame(Ogre::SceneManager& aSceneMgr, QuickGUI::GUIManager& aGUIManager, socket_t& aSocket);
    virtual ~ClientGame(); // Для QuicGUI
    void OnExit(const QuickGUI::EventArgs& args);
    void OnTurn(const QuickGUI::EventArgs& args);
    bool IsQuit() const { return mQuit; }
    void UpdateSelectedTilePosition(Ogre::Ray& aRay);
    void Update(unsigned long aFrameTime);
protected:
private:
    Ogre::SceneManager& mSceneMgr;
    QuickGUI::GUIManager& mGUIManager;
    socket_t& mSocket;
    ClientGeodesicGrid* mGrid;
    std::map< int, ClientUnit* > mUnits;
    ClientTile* mSelectedTile;
    Ogre::SceneNode* mSelectionMarker;
    LoadingSheet mLoadingSheet;
    InGameSheet mIngameSheet;
    bool mQuit;
    int mTime;
    bool mTurnDone;
    void CreateUnitEntities() const;
};

#endif // CLIENTGAME_H
