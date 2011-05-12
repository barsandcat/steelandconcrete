
#include "Ogrehead.h"

IMPLEMENT_APP(OgreHeadApp)

BEGIN_EVENT_TABLE(OgreHeadFrame, wxFrame)
    EVT_MENU(wxID_NEW,    OgreHeadFrame::OnNewTab)
    EVT_MENU(wxID_CLOSE,  OgreHeadFrame::OnCloseTab)
    EVT_MENU(wxID_EXIT,   OgreHeadFrame::OnQuit)
    EVT_MENU(wxID_ABOUT,  OgreHeadFrame::OnAbout) 
END_EVENT_TABLE()

IMPLEMENT_CLASS(OgreHeadFrame, wxFrame)    

OgreHeadFrame::OgreHeadFrame(wxWindow* parent, wxWindowID id)
    : wxFrame(parent, id, _("wxOgreHead"), wxDefaultPosition, wxSize(600,400))
{
    wxStatusBar* sb = CreateStatusBar();
    sb->SetStatusText(_T("Welcome to wxOgreHead demo!"));

    SetMenu();

    m_notebook = new wxNotebook(this, wxID_ANY);
}

bool OgreHeadFrame::Destroy()
{
    for (int page = (m_notebook->GetPageCount() -1); page >= 0; page--) {
        m_notebook->GetPage(page)->Destroy();
        m_notebook->DeletePage(page);
    }

    return wxFrame::Destroy();
}

void OgreHeadFrame::CreateScene()
{
    m_sm = m_control->CreateSceneManager(Ogre::ST_GENERIC);
 
    Ogre::Entity* ent = m_sm->createEntity("head", "ogrehead.mesh");
    Ogre::SceneNode* no = m_sm->getRootSceneNode()->createChildSceneNode();
    
    no->setPosition(0, 0, -120);
    no->attachObject(ent);

    m_control->Refresh();
}

void OgreHeadFrame::AddTab()
{
    m_control = new wxOgreControl(m_notebook, wxID_ANY);
    m_notebook->AddPage(m_control, wxT("OgreTab"), true);
}

void OgreHeadFrame::OnNewTab(wxCommandEvent& WXUNUSED(event))
{
    wxOgreControl* control = new wxOgreControl(m_notebook,
                                               wxID_ANY, 
                                               wxDefaultPosition,
                                               m_notebook->GetClientSize());
    control->SetSceneManager(m_sm);
    m_notebook->AddPage(control, wxT("OgreTab"), true);
}

void OgreHeadFrame::OnCloseTab(wxCommandEvent& WXUNUSED(event))
{
    const size_t page = m_notebook->GetSelection();

    if (page != wxNOT_FOUND) {
        m_notebook->DeletePage(page);
    }
}

void OgreHeadFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void OgreHeadFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxT("wxOgreHead\n")
                 wxT("Copryright (c) 2008 Martin Pieuchot\n\n")
                 wxT("Ogre " WXOGRE_OGRE_STR_VER " '" OGRE_VERSION_NAME "'\n")
                 wxT("wxWidgets " wxVERSION_NUM_DOT_STRING  "\n")
                 wxT("wxOgre " WXOGRE_STR_VER "\n"),
                 wxT("About wxOgreHead"), wxICON_INFORMATION);
                 
}

void OgreHeadFrame::SetMenu()
{
    wxMenu* fmenu = new wxMenu();
    fmenu->Append(wxID_NEW, _T("&New Tab\tCtrl+T"), _T("Open a new tab"));
    fmenu->Append(wxID_CLOSE, _T("&Close Tab\tCtrl+W"),_T("Close current tab"));
    fmenu->AppendSeparator();
    fmenu->Append(wxID_EXIT, _T("E&xit\tCtrl+Q"), _T("Quit wxOgreHead"));

    wxMenu* hmenu = new wxMenu();
    hmenu->Append(wxID_ABOUT, _T("&About\tF1"), _T("About wxOgreHead"));

    wxMenuBar* menubar = new wxMenuBar();
    menubar->Append(fmenu, _T("&File"));
    menubar->Append(hmenu, _T("&Help"));

    SetMenuBar(menubar);
}
