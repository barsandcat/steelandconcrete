#include <OgreControl.h>


OgreControl::OgreControl(wxWindow* parent, wxWindowID id,
                const wxPoint& pos, const wxSize& size):
                wxOgreControl(parent, id, pos, size), m_x(-1), m_y(-1)
{
}

Ogre::SceneManager* OgreControl::CreateSceneManager()
{
    Ogre::SceneManager* sm = wxOgreControl::CreateSceneManager(Ogre::ST_GENERIC);
    m_node = sm->getRootSceneNode()->createChildSceneNode();
    m_node->attachObject(m_cam);
    m_cam->setNearClipDistance(1);
    m_cam->moveRelative(Ogre::Vector3(0, 0, 300));
    return sm;
}

void OgreControl::OnMouseMove(wxMouseEvent& event)
{
    // First process the Rotation
    wxPoint pt(event.GetPosition());

    if (event.RightIsDown()) {
        if ((m_x > -1) && (m_y > -1) && event.Dragging()) {
            // We are draggin : move the camera
            // 0.003 is for speed
            float relX = (pt.x - m_x) * 0.003;
            float relY = (pt.y - m_y) * 0.003;

            // Dragging on X GUI (the width) represents turning
            // around the vertical 3D (Y) axis.
            // And dragging around the Y GUI (the height) around
            // the X 3D axis.
            m_node->yaw((Ogre::Radian)-relX);
            m_node->pitch((Ogre::Radian)-relY);
        }
    } else if (event.LeftIsDown()) {
        ProcessSelection(pt);
    }

    m_x = pt.x;
    m_y = pt.y;

    // Then process the zoom
    if (event.GetWheelRotation()) {
        float z = -event.GetWheelRotation() * 0.05;
        TranslateCamera(0, 0, z);
    }
}
