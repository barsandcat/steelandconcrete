#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include <CEGUI.h>

CEGUI::Window* GetWindow(CEGUI::String aWindowName);

void ShowModal(CEGUI::String aWindowName);

void HideModal(CEGUI::String aWindowName);

void Show(CEGUI::String aWindowName);

void Hide(CEGUI::String aWindowName);

void ShowMessageBox(const char* aMessage);

void InitGUI();


#endif // GUI_H_INCLUDED
