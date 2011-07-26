#ifndef OGRECONTROL_H
#define OGRECONTROL_H

#include <wx/ogre/ogre.h>


class OgreControl : public wxOgreControl
{
public:
    OgreControl(wxWindow* parent, wxWindowID id,
                const wxPoint& pos, const wxSize& size);
    Ogre::SceneManager* CreateSceneManager();
private:
    virtual void OnMouseMove(wxMouseEvent& event);
    long m_x;
    long m_y;
    Ogre::SceneNode* m_node;
};

#endif // OGRECONTROL_H
