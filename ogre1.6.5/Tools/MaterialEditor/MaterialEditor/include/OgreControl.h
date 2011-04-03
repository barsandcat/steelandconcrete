#ifndef OGRECONTROL_H
#define OGRECONTROL_H

#include <wx/ogre/ogre.h>


class OgreControl : public wxOgreControl
{
public:
    OgreControl(wxWindow* parent, wxWindowID id,
                const wxPoint& pos, const wxSize& size):
                wxOgreControl(parent, id, pos, size)
{
}

protected:
    virtual void OnMouseMove(wxMouseEvent& event);
};

#endif // OGRECONTROL_H
