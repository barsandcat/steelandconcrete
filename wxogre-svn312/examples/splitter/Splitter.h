#include "wx/ogre/ogre.h"
#include "wx/ogre/wxFourWaySplitter.h"
#include "wx/notebook.h"

class SplitterFrame : public wxFrame
{
    DECLARE_CLASS(SplitterFrame)

public:
    
    SplitterFrame(wxWindow* parent, wxWindowID id);

    void CreateScene();

private:

    wxFourWaySplitter* m_splitter;
    
    Ogre::SceneManager* m_sm;
};


class SplitterApp : public wxOgreApp
{
public:

    bool OnInit()
    {

        // Initialize Ogre render system
        m_rsys->LoadPlugin("RenderSystem_GL");
        m_rsys->SelectOgreRenderSystem("OpenGL Rendering Subsystem");
        m_rsys->Initialise();

        m_frame = new SplitterFrame(0, wxID_ANY);

        // load resources yeah!
        m_res->LoadResourceFile("resources.cfg");
        m_res->InitialiseAllResources();

        m_frame->Show();
        m_frame->CreateScene();

        return true;
    }

private:

    SplitterFrame* m_frame;
};

DECLARE_APP(SplitterApp)
