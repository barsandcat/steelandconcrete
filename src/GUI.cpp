#include <pch.h>
#include <GUI.h>

#include <CEGUILocalization.h>

#include <ClientApp.h>

CEGUI::Window* GetWindow(CEGUI::String aWindowName)
{
    return CEGUI::WindowManager::getSingleton().getWindow(aWindowName);
}

CEGUI::Window* LoadLayout(CEGUI::String aLayoutName)
{
    return CEGUI::WindowManager::getSingleton().loadWindowLayout(aLayoutName, "", "", &PropertyCallback);
}

CEGUI::Window* LoadWindow(CEGUI::String aLayoutName)
{
    CEGUI::Window* window = LoadLayout(aLayoutName);
    window->setVisible(false);
    return window;
}

void ShowModal(CEGUI::String aWindowName)
{
    CEGUI::Window* window = GetWindow(aWindowName);
    window->setModalState(true);
    window->setVisible(true);
    window->setAlwaysOnTop(true);
}

void HideModal(CEGUI::String aWindowName)
{
    CEGUI::Window* window = GetWindow(aWindowName);
    window->setModalState(false);
    window->setVisible(false);
    window->setAlwaysOnTop(false);
}

void ShowMessageBox(const char* aMessage)
{
    GetWindow("MessageBox/Message")->setText(aMessage);
    ShowModal("MessageBox");
}

bool OnEscape(const CEGUI::EventArgs& args)
{
    CEGUI::Window* inGameMenu = GetWindow("InGameMenu");
    inGameMenu->setVisible(!inGameMenu->isVisible());
    return true;
}

bool OnBrowse(const CEGUI::EventArgs& args)
{
    ShowModal("ServerBrowser");
    return true;
}

bool OnExit(const CEGUI::EventArgs& args)
{
    boost::throw_exception(std::runtime_error("Exit"));
    return true;
}

bool OnMainMenu(const CEGUI::EventArgs& args)
{
    HideModal("ServerBrowser");
    return true;
}

bool OnCloseMessageBox(const CEGUI::EventArgs& args)
{
    HideModal("MessageBox");
    return true;
}

bool OnClick(const CEGUI::EventArgs& args)
{
    OgreAL::Sound *sound = NULL;
    OgreAL::SoundManager& soundManager = ClientApp::GetSoundMgr();
    if (soundManager.hasSound("click"))
    {
        sound = soundManager.getSound("click");
        sound->stop();
    }
    else
    {
        sound = soundManager.createSound("click", "clickclick.ogg", false, false);
        sound->setRelativeToListener(true);
    }
    sound->play();
    return true;
}

void SubscribeToGUI()
{
    CEGUI::GlobalEventSet::getSingleton().subscribeEvent("PushButton/Clicked", &OnClick);
    CEGUI::GlobalEventSet::getSingleton().subscribeEvent("MenuItem/Clicked", &OnClick);

    GetWindow("Main/Menu/Connect")->subscribeEvent(CEGUI::PushButton::EventClicked, &OnBrowse);
    GetWindow("Main/Menu/ExitApp")->subscribeEvent(CEGUI::PushButton::EventClicked, &OnExit);
    GetWindow("ServerBrowser/Cancel")->subscribeEvent(CEGUI::PushButton::EventClicked, &OnMainMenu);
    GetWindow("MessageBox/Close")->subscribeEvent(CEGUI::PushButton::EventClicked, &OnCloseMessageBox);
    GetWindow("InGameMenu/Exit")->subscribeEvent(CEGUI::PushButton::EventClicked, &OnExit);
    GetWindow("Game/StatusPanel/OpenInGameMenu")->subscribeEvent(CEGUI::PushButton::EventClicked, &OnEscape);
}

void InitGUIData()
{
    GetWindow("ServerBrowser/Port")->setText(FLAGS_port);
    GetWindow("ServerBrowser/Address")->setText(FLAGS_address);
    GetWindow("ServerBrowser/Login")->setText(FLAGS_login);
    GetWindow("ServerBrowser/Password")->setText(FLAGS_password);
}

void InitGUI()
{
    CEGUI::Window* main = LoadLayout("Main.layout");

    main->addChildWindow(LoadWindow("ServerBrowser.layout"));
    main->addChildWindow(LoadWindow("MessageBox.layout"));

    CEGUI::System::getSingleton().setGUISheet(main);

    CEGUI::Window* game = LoadLayout("Game.layout");
    game->addChildWindow(LoadWindow("InGameMenu.layout"));

    SubscribeToGUI();
    InitGUIData();
}
