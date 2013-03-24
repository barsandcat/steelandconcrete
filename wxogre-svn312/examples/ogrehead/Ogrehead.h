
#include "wx/ogre/ogre.h"
#include "wx/notebook.h"

class OgreHeadFrame : public wxFrame
{
    DECLARE_CLASS(OgreHeadFrame)
    DECLARE_EVENT_TABLE()

public:
    
    OgreHeadFrame(wxWindow* parent, wxWindowID id);

    void AddTab();
    void CreateScene();

private:
    
    virtual bool Destroy();

    void SetMenu();

    void OnNewTab(wxCommandEvent& event);
    void OnCloseTab(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    wxNotebook*     m_notebook;
    wxOgreControl*  m_control;

    Ogre::SceneManager* m_sm;
};


class OgreHeadApp : public wxOgreApp
{
public:

    bool OnInit()
    {

        // Initialize Ogre render system
        m_rsys->LoadPlugin("RenderSystem_GL");
        m_rsys->SelectOgreRenderSystem("OpenGL Rendering Subsystem");
        m_rsys->Initialise();

        m_frame = new OgreHeadFrame(0, wxID_ANY);
        m_frame->AddTab();

        // load resources yeah!
        m_res->LoadResourceFile("resources.cfg");
        m_res->InitialiseAllResources();

        m_frame->Show();
        m_frame->CreateScene();

        return true;
    }

private:

    OgreHeadFrame* m_frame;
};

DECLARE_APP(OgreHeadApp)
