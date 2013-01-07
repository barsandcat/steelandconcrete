#include <pch.h>
#include <GUI.h>

#include <CEGUILocalization.h>

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

void BuildLayout()
{
    CEGUI::Window* main = LoadLayout("Main.layout");

    main->addChildWindow(LoadWindow("ServerBrowser.layout"));
    main->addChildWindow(LoadWindow("MessageBox.layout"));

    CEGUI::System::getSingleton().setGUISheet(main);

    CEGUI::Window* game = LoadLayout("Game.layout");
    game->addChildWindow(LoadWindow("InGameMenu.layout"));
}
