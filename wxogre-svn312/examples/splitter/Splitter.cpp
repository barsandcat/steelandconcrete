#include "Splitter.h"

IMPLEMENT_APP(SplitterApp)

IMPLEMENT_CLASS(SplitterFrame, wxFrame)    

SplitterFrame::SplitterFrame(wxWindow* parent, wxWindowID id)
    : wxFrame(parent, id, _("wxSplitter"), wxDefaultPosition, wxSize(600,400))
{
	m_splitter = new wxFourWaySplitter(this);
	
	for(int i=0; i<4; i++)
		m_splitter->SetWindow(i, new wxOgreControl(m_splitter, wxID_ANY));
}

void SplitterFrame::CreateScene()
{
    m_sm = ((wxOgreControl*) m_splitter->GetWindow(0))->CreateSceneManager(Ogre::ST_GENERIC);
    
 		((wxOgreControl*) m_splitter->GetWindow(1))->SetSceneManager(m_sm);
 		((wxOgreControl*) m_splitter->GetWindow(2))->SetSceneManager(m_sm);
 		((wxOgreControl*) m_splitter->GetWindow(3))->SetSceneManager(m_sm);
 		
    Ogre::Entity* ent = m_sm->createEntity("head", "ogrehead.mesh");
    Ogre::SceneNode* no = m_sm->getRootSceneNode()->createChildSceneNode();
    
    no->setPosition(0, 0, -120);
    no->attachObject(ent);

    m_splitter->Refresh();
}
